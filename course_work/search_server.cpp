#include "search_server.h"
#include "iterator_range.h"
#include <utility>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <future>
#include <fstream>
#include <utility>
#include "parse.h"
#include "profile.h"
#include <unordered_map>
#include "paginate.h"
#include <string_view>

vector<string> SplitIntoWords(const string& line) {
    istringstream words_input(line);
    vector<string> result;
    string word;
    while (words_input >> word) {
        result.push_back(move(word));
    }
    //return { istream_iterator<string>(words_input), istream_iterator<string>() };
    return result;
}

InvertedIndex BuildNewIndex(istream& document_input) {
    InvertedIndex new_index;
    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    return new_index;
}

SearchServer::SearchServer(istream& document_input) {
    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
 
    InvertedIndex new_index;

    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    swap(index, new_index);
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output)
{
    vector <string> all;
    //all.reserve(12000);
    size_t page_size = 600;
    vector<future<string>> futures;

    for (string line; getline(query_input, line); )
    {
        all.push_back(move(line));
    }
    for (auto& page : Paginate(all, page_size))
    {
        futures.push_back(async([=] {return process_chunk(this, page); }));
    }
    for (auto& f : futures)
    {
        search_results_output << f.get();
    }
}

string SearchServer::AddQueriesStream_s(string current_query) {
    string res;
    vector<pair<size_t, size_t>> search_results(index.docs_size(), { 0,0 });
    const auto words = SplitIntoWords(current_query);
    for (const auto& word : words)
    {
        for (const auto& docid : index.Lookup(word)) {
            search_results[docid.first].first = docid.first;
            search_results[docid.first].second += docid.second;
        }
    }

    {
        int middle = 5;
        if (search_results.size() < 5) middle = search_results.size();
        partial_sort
        (
            search_results.begin(), search_results.begin() + middle,
            search_results.end(),
            [](pair<size_t, size_t>& lhs, pair<size_t, size_t>& rhs) {
                    if (lhs.second == rhs.second) {
                        return lhs.first < rhs.first;
                    }
                    return lhs.second > rhs.second;               
            }
        );
    }
    {
        swap(res, current_query);
        res += ":";
        for (auto& [docid, hitcount] : Head(search_results, 5))
        {
            if (hitcount > 0)
                res += " {docid: " + to_string(docid) + ", "
                + "hitcount: " + to_string(hitcount) + '}';
        }
        return res;
    }
}

void InvertedIndex::Add(string document) {
    docs.push_back(move(document));

    const size_t docid = docs.size() - 1;
    unordered_map<string, size_t> tmp;
    for (string& word : SplitIntoWords(docs.back())) {
        ++tmp[move(word)];
    }
    for (auto& [word, hitcount] : tmp) {
        index[move(word)].push_back({ docid, hitcount });
    }
}

vector< pair < size_t, size_t>> InvertedIndex::Lookup(const string& word) const {
    if (auto it = index.find(word); it != index.end()) {
        return it->second;
    }
    return {};
}