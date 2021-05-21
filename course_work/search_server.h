#pragma once

#include <istream>
#include <ostream>
#include <set>

#include <vector>
#include <map>
#include <string>
#include <shared_mutex>
#include <unordered_map>
using namespace std;


#include <mutex>
using namespace std;

class InvertedIndex {
public:
    InvertedIndex() {
      //  docs.reserve(12000);
    }
    void Add(string document);
    vector<pair < size_t, size_t>> Lookup(const string& word) const;

    const string& GetDocument(size_t id) const {
        return docs[id];
    }
    size_t docs_size() const
    {
        return docs.size();
    }

private:

    unordered_map<string, vector<pair<size_t, size_t>>> index;
    vector<string> docs;
};

template <typename T>
class Synchronized {
public:
    explicit Synchronized(T initial = T()) :
        value(move(initial)) {}

    struct Access {
        T& ref_to_value;
        lock_guard<mutex> guard;
    };

    Access GetAccess() {
        return { value, lock_guard(m) };
    }
private:
    T value;
    mutex m;
};


class SearchServer {
public:
    SearchServer() = default;
    explicit SearchServer(istream& document_input);
    void UpdateDocumentBase(istream& document_input);
    void AddQueriesStream(istream& query_input, ostream& search_results_output);
    string AddQueriesStream_s(string current_query);
    mutex m;
    size_t num_of_threads;
private:
    InvertedIndex index;

};

template <typename ContainerOfstring>
string process_chunk(SearchServer* s, const ContainerOfstring& input)
{
    string res;
    for (auto& current : input)
    {
        res += s->AddQueriesStream_s(move(current));

        res.push_back('\n');
    }
    return res;
}