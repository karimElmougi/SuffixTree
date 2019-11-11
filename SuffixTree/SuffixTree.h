#pragma once

#include <memory>
#include <string>
#include "Node.h"
#include "UkkonenBuilder.h"

using std::shared_ptr;
using std::string;

class SuffixTree {
public:
	SuffixTree(string&& text) noexcept : text{ text } {
		auto b = UkkonenTreeBuilder{ this->text };
		root = b.build();
	}

	void print_all_suffixes() const noexcept {
		root->print_all_suffixes();
	}

private:
	shared_ptr<Node> root;
	string text;
};
