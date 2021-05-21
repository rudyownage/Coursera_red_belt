#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "profile.h"
#include "test_runner.h"
#include <queue>
#include <cstdint>

using namespace std;

struct Event {
    int64_t time;
    string hotel;
    int client;
    int rooms;
};

class Booking {
public:
    Booking() {}
    void Book(int64_t time, const string& hotel_name, int client, int rooms) {
        events.push_back({ time, hotel_name, client, rooms });
        hotels_rooms[hotel_name] += rooms;
        ++hotels_clients[hotel_name][client];
        UpdateQue(time, hotel_name, client);
    }
    int Clients(const string& hotel_name) const{
        if (hotels_clients.count(hotel_name) == 0) {
            return 0;
        }
        return hotels_clients.at(hotel_name).size();
    }
    int Rooms(const string& hotel_name) const{
        if (hotels_rooms.count(hotel_name) == 0) {
            return 0;
        }
        return hotels_rooms.at(hotel_name);
    }

private:
    static const int SECONDS_IN_DAY = 86400;
    deque<Event> events;
    map<string, int> hotels_rooms;
    map<string, map<int,int>> hotels_clients;

    void UpdateQue(int64_t time, const string& hotel, int client) {
        if (events.empty()) {
            return;
        }
        Event old = events.front();
        while (!events.empty() && old.time <= time - SECONDS_IN_DAY) {
            hotels_rooms[old.hotel] -= old.rooms;
            events.pop_front();
            --hotels_clients[old.hotel][old.client];
            if (hotels_clients[old.hotel][old.client] == 0) {              
                hotels_clients[old.hotel].erase(old.client);
            }
            old = events.front();
        }     
   }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Booking manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        if (query_type == "BOOK") {
            int64_t time;
            int room_count, client_id;
            string hotel;
            cin >> time >> hotel >> client_id >> room_count;
            manager.Book(time, hotel, client_id, room_count);
        }
        else if (query_type == "CLIENTS") {
            string hotel;
            cin >> hotel;
            cout << manager.Clients(hotel) << endl;
        }
        else if (query_type == "ROOMS") {
            string hotel;
            cin >> hotel;
            cout << manager.Rooms(hotel) << endl;
        }
    }

    return 0;
}