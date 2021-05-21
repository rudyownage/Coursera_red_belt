#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "profile.h"
#include "test_runner.h"

using namespace std;

class RouteManager {
public:
    void AddRoute(int start, int finish) {
        reachable_lists_[start].insert(finish); 
        reachable_lists_[finish].insert(start);   
    }
    int FindNearestFinish(int start, int finish) const {
        int result = abs(start - finish);
        if (reachable_lists_.count(start) < 1) {
            return result;
        }
        const set<int>& reachable_stations = reachable_lists_.at(start);
        auto finish_pos = reachable_stations.lower_bound(finish);
        if (finish_pos != reachable_stations.end()) {
            result = min(result, abs(*finish_pos - finish));
        }
        if (finish_pos != reachable_stations.begin()) {
            result = min(result, abs(*prev(finish_pos) - finish));
        }
        return result;
    }
private:
    map<int, set<int>> reachable_lists_;
};


int main() {
    RouteManager routes;
    int query_count;
    cin >> query_count;


    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        int start, finish;
        cin >> start >> finish;
        if (query_type == "ADD") {           
            routes.AddRoute(start, finish);
        }
        else if (query_type == "GO") {         
            cout << routes.FindNearestFinish(start, finish) << "\n";
        }
    }

    return 0;
}