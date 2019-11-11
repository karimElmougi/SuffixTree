#include <iostream>
#include "SuffixTree.h"


int main() {
	const auto s = "abcabxabcd$";
	auto tree = SuffixTree{ s };
	tree.print_all_suffixes();
}

