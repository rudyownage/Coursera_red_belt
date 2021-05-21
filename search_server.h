#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include "iterator_range.h"
using namespace std;

class InvertedIndex {
public:
    void Add(const string& document);
    vector<pair<size_t, size_t>> Lookup(const string& word) const;

    const string& GetDocument(size_t id) const {
        return docs[id];
    }
    void Print() const;



private:
    unordered_map<string, vector<pair<size_t, size_t>>> index;
    vector<string> docs;
};

class SearchServer {
public:
    SearchServer() = default;
    explicit SearchServer(istream& document_input);
    void UpdateDocumentBase(istream& document_input);
    void AddQueriesStream(istream& query_input, ostream& search_results_output);

    mutex server_m;
    InvertedIndex index;
};

vector<string> Search(istream& query_input, ostream& search_results_output, SearchServer* server);