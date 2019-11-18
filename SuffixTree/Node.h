#pragma once

#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <utility>
#include "ptr.h"

using std::array;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::vector;
using std::pair;
using std::optional;
using std::cout;

struct Node {
	Node(shared_ptr<Node> parent, string_view&& edge) noexcept :
		parent{ parent },
		children{},
		edge{ edge },
		suffix_index{}  {}
	Node(shared_ptr<Node> parent, string_view&& edge, size_t suffix_index) noexcept :
		parent{ parent },
		children{},
		edge{ edge },
		suffix_index{ suffix_index } {}
	Node(const Node&) noexcept = default;
	Node(Node&&) noexcept = default;
	~Node() = default;

	Node& operator=(const Node&) noexcept = default;
	Node& operator=(Node&&) noexcept = default;

	string suffix() const noexcept {
		auto v = vector<string_view>{ edge };
		for (auto curr = parent; curr; curr = curr->parent) {
			v.push_back(curr->edge);
		}

		auto s = string{};

		for (auto curr = v.crbegin(); curr != v.crend(); ++curr)
			s += *curr;

		return s;
	}

	bool is_leaf() const noexcept {
		return suffix_index.has_value();
	}

	shared_ptr<const Node> parent;
	vector<pair<char, shared_ptr<Node>>> children;
	string_view edge;
	optional<size_t> suffix_index;
};
