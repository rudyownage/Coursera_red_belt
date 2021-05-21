#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "profile.h"
#include "test_runner.h"

using namespace std;

class Learner {
private:
    vector<string> dict;

public:
    int Learn(const vector<string>& words) {
        int newWords = 0;
        set<string> new_dict(dict.begin(), dict.end());
        new_dict.insert(words.begin(), words.end());
        newWords = new_dict.size() - dict.size();
        dict.clear();
        dict.insert(dict.begin(),new_dict.begin(), new_dict.end());
        return newWords;
    }

    vector<string> KnownWords() {
        return dict;
    }
};



int main() {
    Learner learner;
    string line;
    while (getline(cin, line)) {
        vector<string> words;
        stringstream ss(line);
        string word;
        while (ss >> word) {
            words.push_back(word);
        }
        cout << learner.Learn(words) << "\n";
    }
    cout << "=== known words ===\n";
    for (auto word : learner.KnownWords()) {
        cout << word << "\n";
    }
    return 0;
}