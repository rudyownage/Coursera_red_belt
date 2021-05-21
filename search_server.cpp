#include "search_server.h"
#include "iterator_range.h"
#include "profile.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include "parse.h"
#include <future>

vector<string> SplitIntoWords(const string& line) {
    istringstream words_input(line);
    return { istream_iterator<string>(words_input), istream_iterator<string>() };
}

SearchServer::SearchServer(istream& document_input) {
    UpdateDocumentBase(document_input);
}

InvertedIndex BuildIndex(istream& document_input) {
    InvertedIndex new_index;

    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    return new_index;
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    InvertedIndex new_index;
    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    lock_guard guard(server_m);
    swap(index, new_index);
}

void SearchServer::AddQueriesStream(
    istream& query_input, ostream& search_results_output
) {
    vector<future<vector<string>>> futures;
    for (int i = 0; i < 6; ++i) {
        futures.push_back(async(Search, ref(query_input), ref(search_results_output), this));
    }
    for (auto& f : futures) {
        for (auto& item : f.get()) {
            search_results_output << item;
        }
    }
    //Search(query_strings, search_results_output, this, make_pair(query_strings.begin(), query_strings.end()));
}


void InvertedIndex::Add(const string& document) {
    docs.push_back(document);

    const size_t docid = docs.size() - 1;
    unordered_map<string, size_t> tmp;
    for (const string& word : SplitIntoWords(document)) {
        tmp[word]++;
    }
    for (const auto& [word, hitcount] : tmp) {
        index[word].push_back({docid, hitcount});
    }
}

vector<pair<size_t, size_t>> InvertedIndex::Lookup(const string& word) const{
    if (auto it = index.find(word); it != index.end()) {
        return it->second;
    }
    else {
        return {};
    }
}

void InvertedIndex::Print() const {
    for (const auto& [key, value] : index) {
        cout << key << "{" << endl;
        for (auto& [docid, hitcount] : value) {
            cout << "docid: " << docid << " hitcount: " << hitcount << endl;
        }
        cout << "}" << endl;
    }
}

vector<string> Search(istream& query_input, ostream& search_results_output, SearchServer* server)
{
    vector<pair<size_t, size_t>> docid_count(50000);
    string current_query;
    string result;
    vector<string> r;
    for (string current_query; getline(query_input, current_query);) {       
        const auto words = SplitIntoWords(current_query);
        for (const auto& word : words) {
            for (const auto docid : server->index.Lookup(word)) {
                docid_count[docid.first].first = docid.first;
                docid_count[docid.first].second += docid.second;
            }
        }    
            partial_sort(
                begin(docid_count), begin(docid_count) + 5,
                end(docid_count),
                [](pair<size_t, size_t>& lhs, pair<size_t, size_t>& rhs) {
                    if (lhs.second == rhs.second) {
                        return lhs.first < rhs.first;
                    }
                    return lhs.second > rhs.second;
                }
            );                 
                result + current_query +':';
                for (size_t i = 0; i < 5; ++i) {
                    if (docid_count[i].second != 0) {
                        result + " {"
                            + "docid: " + to_string(docid_count[i].first) + ", "
                            + "hitcount: " + to_string(docid_count[i].second) + '}';
                    }
                }
                result + "\n";
                r.push_back(move(result));
            docid_count.assign(50000, { 0,0 });
    }
    return r;
}