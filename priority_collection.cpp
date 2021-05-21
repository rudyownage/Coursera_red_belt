
#include <algorithm>
#include <set>
#include <cstdint>
#include <iterator>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include <numeric>
#include <algorithm>

using namespace std;

bool operator<(pair<int, int> lhs, pair<int, int> rhs) {
    return lhs.first < lhs.second;
}

template <typename T>
class PriorityCollection {
public:
    using Id = int;

    Id Add(T object) {
        const Id new_id = collection.size();
        collection.push_back({ move(object) });
        priority.insert({ 0, new_id });
        return new_id;
    }


    template <typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end,
        IdOutputIt ids_begin) {
        while (range_begin != range_end) {
            *ids_begin++ = Add(move(*range_begin++));            
        }
    }

    bool IsValid(Id id) const {
        return id >= 0 && id < collection.size() && collection[id].priority != NONE_PRIORITY;
    }

    const T& Get(Id id) const {
        return collection[id].data;
    }

    void Promote(Id id) {
        auto& item = collection[id];
        const int old_priority = item.priority;
        const int new_priority = ++item.priority;
        priority.erase({ old_priority, id });
        priority.insert({ new_priority, id });
    }

    pair<const T&, int> GetMax() const {
        const auto& item = collection[prev(priority.end())->second];
        return { item.data, item.priority };
    }

    pair<T, int> PopMax() {
        const auto priority_it = prev(priority.end());
        auto& item = collection[priority_it->second];
        priority.erase(priority_it);
        const int pr = item.priority;
        item.priority = NONE_PRIORITY;
        return { move(item.data), pr };
    }

private:
    struct ObjectItem {
        T data;
        int priority = 0;
    };
    static const int NONE_PRIORITY = -1;
    set<pair<int, Id>> priority;
    vector<ObjectItem> collection;
    int count = 0;
};