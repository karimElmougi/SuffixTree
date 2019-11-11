#pragma once

#include <array>
#include <memory>
#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include "ptr.h"
#include "Node.h"

using std::array;
using std::shared_ptr;
using std::string;
using std::make_shared;
using std::distance;
using std::all_of;
using std::move;

struct UkkonenNode {
	UkkonenNode(const shared_ptr<UkkonenNode> suffix_link, const ptr<const char> begin, const ptr<const ptr<const char>> end) noexcept :
		children{},
		suffix_link{ suffix_link },
		begin{ begin },
		end{ end },
		suffix_index{ -1 } {}
	UkkonenNode(const UkkonenNode&) noexcept = default;
	UkkonenNode(UkkonenNode&&) noexcept = default;
	~UkkonenNode() = default;

	UkkonenNode& operator=(const UkkonenNode&) noexcept = default;
	UkkonenNode& operator=(UkkonenNode&&) noexcept = default;

	bool is_leaf() const noexcept {
		return all_of(children.cbegin(), children.cend(), [](auto c) { return c == nullptr; });
	}

	size_t edge_length() const noexcept {
		assert(begin <= *end);
		return distance(begin, *end) + 1;
	}

	array<shared_ptr<UkkonenNode>, 256> children;
	shared_ptr<UkkonenNode> suffix_link;
	ptr<const char> begin;
	ptr<const ptr<const char>> end;
	int suffix_index;
};

struct ActivePoint {
	ActivePoint(const shared_ptr<UkkonenNode> node, const ptr<const char> edge, const size_t length) noexcept :
		node{ node },
		edge{ edge },
		length{ length } {}
	ActivePoint(const ActivePoint&) noexcept = default;
	ActivePoint(ActivePoint&&) noexcept = default;
	~ActivePoint() = default;

	ActivePoint& operator=(const ActivePoint&) noexcept = default;
	ActivePoint& operator=(ActivePoint&&) noexcept = default;

	shared_ptr<UkkonenNode>& get_node_of_active_edge() const noexcept {
		return node->children[*edge];
	}

	shared_ptr<UkkonenNode> node;
	ptr<const char> edge;
	size_t length;
};

class UkkonenTreeBuilder {
public:
	UkkonenTreeBuilder(const string& text) : text{ text } {}

	shared_ptr<Node> build() noexcept;

private:
	ActivePoint extend(const ptr< const ptr<const char>> ptr_to_cursor, ActivePoint&& active_point) noexcept;

	const string& text;
	shared_ptr<UkkonenNode> root = nullptr;
	size_t nb_remaining_suffixes = 0;
};

