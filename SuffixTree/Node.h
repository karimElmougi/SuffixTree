#pragma once

#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <utility>
#include <unordered_set>
#include <algorithm>

using std::array;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::vector;
using std::unordered_set;
using std::pair;
using std::optional;
using std::cout;
using std::all_of;
using std::distance;

struct Node {
	Node(shared_ptr<Node> parent, string_view edge) noexcept :
		parent{ parent },
		edge{ edge } {}
	Node(shared_ptr<Node> parent, string_view edge, size_t suffix_index) noexcept :
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
		if (!begin)
			return {};
		const auto end = begin + edge.size();

		for (auto cur = parent; !cur->edge.empty(); cur = cur->parent) {
			begin -= cur->edge.size();
		}

		const auto size = static_cast<size_t>(distance(begin, end));
		return { begin, size };
	}

	bool is_leaf() const noexcept {
		return suffix_index.has_value();
	}

	bool contains_all_terminators(const vector<char>& terminators) const noexcept {
		return all_of(terminators.cbegin(), terminators.cend(), [&](auto t) { return this->terminators.find(t) != this->terminators.cend(); });
	}

	shared_ptr<Node> parent;
	vector<pair<char, shared_ptr<Node>>> children;
	string_view edge;
	unordered_set<char> terminators;
	optional<size_t> suffix_index;
};
