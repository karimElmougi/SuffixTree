#pragma once

#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include "ptr.h"

using std::array;
using std::shared_ptr;
using std::string_view;
using std::vector;
using std::optional;
using std::cout;

class Node {
public:
	Node(string_view&& edge) noexcept :
		children{},
		edge{ edge },
		suffix_index{}  {}
	Node(string_view&& edge, size_t suffix_index) noexcept :
		children{},
		edge{ edge },
		suffix_index{ suffix_index } {}
	Node(const Node&) noexcept = default;
	Node(Node&&) noexcept = default;
	~Node() = default;

	Node& operator=(const Node&) noexcept = default;
	Node& operator=(Node&&) noexcept = default;

	void print_all_suffixes() const noexcept {
		auto path = vector<ptr<const Node>>{};
		print_all_suffixes(path);
	}

	bool is_leaf() const noexcept {
		return suffix_index.has_value();
	}

private:
	void print_all_suffixes(vector<ptr<const Node>>& path) const noexcept {
		if (is_leaf()) {
			for (auto p : path) {
				cout << p->edge;
			}

			cout << edge << " [" << suffix_index.value() << "]\n";
			return;
		}

		path.push_back(this);

		for (auto child : children) {
			if (child)
				child->print_all_suffixes(path);
		}

		path.pop_back();
	}

public:
	array<shared_ptr<Node>, 256> children;

private:
	string_view edge;
	optional<size_t> suffix_index;
};
