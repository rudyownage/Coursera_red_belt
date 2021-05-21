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

vector<vector<int>> GenerateMatrix(int row, int col) {
    vector<vector<int>> result(row);
    for (int i = 0; i < row; ++i) {
        vector<int> tmp(col);
        result[i] = tmp;
        for (int j = 0; j < col; ++j) {
            result[i][j] = i + j;
        }
    }
    return result;
}

template<typename Container>
int64_t SumSingleThread(const Container& matrix) {
    int64_t result = 0;
    for (const auto& row : matrix) {
        for (auto item : row) {
            result += item;
        }
    }
    return result;
}

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
    vector<future<int64_t>> futures;
    for (auto& page : Paginate(matrix, 2000)) {
        futures.push_back(async([page]() {
            return SumSingleThread(page);
            }));}  
    int64_t result = 0;
    for (auto& f : futures) {
        result += f.get();
    }
    return result;
}

void TestCalculateMatrixSum() {
    const vector<vector<int>> matrix = {
      {1, 2, 3, 4},
      {5, 6, 7, 8},
      {9, 10, 11, 12},
      {13, 14, 15, 16}
    };
    {
        LOG_DURATION("SingleThreadSum");
        cout << SumSingleThread(GenerateMatrix(9000, 9000)) << endl;
    }
    {
        LOG_DURATION("MultiThreadSum");
        cout << CalculateMatrixSum(GenerateMatrix(9000, 9000)) << endl;
    }
    ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestCalculateMatrixSum);
}