#include "test_runner.h"
#include "profile.h"
#include <cstdint>
#include <iterator>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>


using namespace std;

template <typename String>
using Group = vector<String>;

template <typename String>
using Char = typename String::value_type;

template<typename String>
using Key = String;

template <typename String>
Key<String> MakeKey(const String& string) {
	String chars = string;
	sort(begin(chars), end(chars));
	chars.erase(unique(begin(chars), end(chars)), end(chars));
	return chars;
}

template <typename String>
vector<Group<String>> GroupHeavyStrings(vector<String> strings) {
	map<Key<String>, Group<String>> groups_map;
	for (String& str : strings) {
		groups_map[MakeKey(str)].push_back(move(str));
	}
	vector<Group<String>> result;
	for (auto& [key, value] : groups_map) {
		result.push_back(move(value));
	}
	return result;

}


void TestGroupingABC() {
	vector<string> strings = { "caab", "abc", "cccc", "bacc", "c" };
	auto groups = GroupHeavyStrings(strings);
	ASSERT_EQUAL(groups.size(), 2);
	sort(begin(groups), end(groups));  
	ASSERT_EQUAL(groups[0], vector<string>({ "caab", "abc", "bacc" }));
	ASSERT_EQUAL(groups[1], vector<string>({ "cccc", "c" }));
}

void TestGroupingReal() {
	vector<string> strings = { "law", "port", "top", "laptop", "pot", "paloalto", "wall", "awl" };
	auto groups = GroupHeavyStrings(strings);
	ASSERT_EQUAL(groups.size(), 4);
	sort(begin(groups), end(groups));  
	ASSERT_EQUAL(groups[0], vector<string>({ "laptop", "paloalto" }));
	ASSERT_EQUAL(groups[1], vector<string>({ "law", "wall", "awl" }));
	ASSERT_EQUAL(groups[2], vector<string>({ "port" }));
	ASSERT_EQUAL(groups[3], vector<string>({ "top", "pot" }));
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestGroupingABC);
	RUN_TEST(tr, TestGroupingReal);
	return 0;
}