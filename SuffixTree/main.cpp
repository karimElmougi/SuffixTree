#include <iostream>
#include <vector>
#include <string>
#include "SuffixTree.h"

using std::vector;
using std::string;
using std::cout;

void test_suffix_tree() {
	const auto s = "aaaababcab";
	auto tree = SuffixTree{ s, '$' };
	tree.print_all_suffixes();

	cout << "\nlongest repeated substrings:" << '\n';
	tree.print_longest_repeated_substring();

	cout << "\nis_substring(baba):\n";
	cout << tree.is_substring("baba") << '\n';
}

void test_generalized() {
	const auto strings = vector<string>{ "ananas", "banana" };
	//const auto terminators = vector<char>{ '$', '#', '%', '&', '*' };
	const auto terminators = vector<char>{ '#', '$', '%', '&', '*' };
	auto tree = SuffixTree{ strings, terminators };
	tree.print_all_suffixes();

	cout << "\nlongest common substring:" << '\n';
	const auto s = tree.find_longest_common_substring({ terminators[0], terminators[1] });
	cout << '\t' << s << '\n';
}

int main() {
	test_generalized();
}

