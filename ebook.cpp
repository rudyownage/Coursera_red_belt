#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "profile.h"
#include "test_runner.h"
#include <iomanip>

using namespace std;

class ReadingManager {
public:
    ReadingManager()
        : pages_rating(MAX_PAGES_COUNT_ + 1, 0) {}

    void Read(int user_id, int page_count) {
        ++pages_rating[page_count];
        if (users_to_pages.count(user_id) == 1) {
            --pages_rating[users_to_pages[user_id]];
        }
        users_to_pages[user_id] = page_count;
    }

    double Cheer(int user_id) const {
        if (users_to_pages.count(user_id) == 0) {
            return 0;
        }
        if (users_to_pages.size() == 1) {
            return 1;
        }
        int page = users_to_pages.at(user_id);
        int count = 0;
        for (int i = 0; i < page; ++i) {
            count += pages_rating[i];
        }
     
        return count * 1.0 / (users_to_pages.size() - 1);
    }

private:

    static const int MAX_PAGES_COUNT_ = 1000;

    vector<int> pages_rating;
    map<int, int> users_to_pages;
    

};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ReadingManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        int user_id;
        cin >> user_id;

        if (query_type == "READ") {
            int page_count;
            cin >> page_count;
            manager.Read(user_id, page_count);
        }
        else if (query_type == "CHEER") {
            cout << setprecision(6) << manager.Cheer(user_id) << "\n";
        }
    }

    return 0;
}