#pragma once

#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <utility>

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
		edge{ edge } {}
	Node(shared_ptr<Node> parent, string_view&& edge, size_t suffix_index) noexcept :
		parent{ parent },
		edge{ edge },
		suffix_index{ suffix_index } {}
	Node(const Node&) noexcept = default;
	Node(Node&&) noexcept = default;
	~Node() = default;

	Node& operator=(const Node&) noexcept = default;
	Node& operator=(Node&&) noexcept = default;

	string_view suffix() const noexcept {
		auto begin = edge.data();
		for (auto curr = parent; !curr->edge.empty(); curr = curr->parent) {
			begin -= curr->edge.size();
		}

		return { begin };
	}

	bool is_leaf() const noexcept {
		return suffix_index.has_value();
	}

	shared_ptr<const Node> parent;
	vector<pair<char, shared_ptr<Node>>> children;
	string_view edge;
	optional<size_t> suffix_index;
};
