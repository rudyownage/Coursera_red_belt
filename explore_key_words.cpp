#include "test_runner.h"
#include "profile.h"
#include <cstdint>
#include <iterator>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include <numeric>
#include <future>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator += (const Stats& other);
};

void Stats::operator+=(const Stats& other) {
    for (auto [key, value] : other.word_frequences) {
        word_frequences[key] += value;
    }
}

template <typename Iterator>
class IteratorRange {
private:
    Iterator first, last;
    size_t size_;
public:
    IteratorRange(Iterator b, Iterator e) :
        first(b),
        last(e),
        size_(distance(first, last)) {}
    Iterator begin() const {
        return first;
    }
    Iterator end() const {
        return last;
    }

    size_t size() const {
        return size_;
    }
};

template <typename It>
class Paginator {
public:
    Paginator(It b, It e, size_t size) {
        for (size_t left = distance(b, e); left > 0;) {
            size_t current_page_size = min(size, left);
            It current_page_end = next(b, current_page_size);
            pages.push_back({ b, current_page_end });
            left -= current_page_size;
            b = current_page_end;
        }
    }
    auto begin() const {
        return pages.begin();
    }
    auto end() const {
        return pages.end();
    }
    size_t size() const {
        return pages.size();
    }

private:
    vector<IteratorRange<It>> pages;
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
    vector<string> words;
    string tmp;
    while (input >> tmp) {
        words.push_back(tmp);
    }
    Stats result;
    vector<future<Stats>> futures;
    for (auto part : Paginate(words, words.size() / 8)) {
        futures.push_back(async([part, &key_words]() {
            Stats res;
            for (string& w : part) {
                if (key_words.count(w) != 0) {
                    ++res.word_frequences[w];
                }
            }
            return res;
            }));
    }
    for (auto& f : futures) {
        result += f.get();
    }
    return result;
}

void TestBasic() {
    
    const set<string> key_words = { "yangle", "rocks", "sucks", "all" };

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";
    const auto stats = ExploreKeyWords(key_words, ss);
    const map<string, int> expected = {
      {"yangle", 6},
      {"rocks", 2},
      {"sucks", 1}
    };
    ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
    TestRunner tr; {
        LOG_DURATION("123");

        RUN_TEST(tr, TestBasic);
    }
    return 0;
}