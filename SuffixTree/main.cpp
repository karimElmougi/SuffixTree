#include <iostream>
#include <vector>
#include <string>
#include "SuffixTree.h"

using std::vector;
using std::string;
using std::cout;

void test_suffix_tree() {
	const auto s = "abababa";
	auto tree = SuffixTree{ s, '$' };
	tree.print_all_suffixes();

	cout << "\nlongest repeated substrings:" << '\n';
	tree.print_longest_repeated_substring();
}

void test_generalized() {
	const auto strings = vector<string>{ "banana", "ananas" };
	const auto terminators = vector<char>{ '$', '#', '%', '&', '*' };
	auto tree = SuffixTree{ strings, terminators };
	tree.print_all_suffixes();

	cout << "\nlongest common substrings (DP):" << '\n';
	for (const auto& s : tree.find_longest_common_substrings_dp(strings[0], strings[1])) {
		cout << '\t' << s << '\n';
	}

	//for (const auto& s : tree.find_longest_common_substrings(strings[0], terminators[0], strings[1], terminators[1])) {
	//	cout << s << '\n';
	//}
}

int main() {
	test_suffix_tree();
}

