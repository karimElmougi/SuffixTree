#pragma once

#include <array>
#include <memory>
#include <algorithm>
#include <cassert>
#include <vector>
#include <string>
#include <optional>
#include "ptr.h"
#include "Node.h"

using std::array;
using std::shared_ptr;
using std::string;
using std::vector;
using std::make_shared;
using std::distance;
using std::find_if;
using std::move;

struct UkkonenNode {
	UkkonenNode(const shared_ptr<UkkonenNode> suffix_link, const ptr<const char> begin, const ptr<const ptr<const char>> end) noexcept :
		suffix_link{ suffix_link },
		begin{ begin },
		end{ end } {}
	UkkonenNode(const UkkonenNode&) noexcept = default;
	UkkonenNode(UkkonenNode&&) noexcept = default;
	~UkkonenNode() = default;

	UkkonenNode& operator=(const UkkonenNode&) noexcept = default;
	UkkonenNode& operator=(UkkonenNode&&) noexcept = default;

	bool is_leaf() const noexcept {
		return children.empty();
	}

	size_t edge_length() const noexcept {
		assert(begin <= *end);
		return distance(begin, *end) + 1;
	}

	void push_or_update_child(char c, shared_ptr<UkkonenNode> node) {
		auto it = find_if(children.begin(), children.end(), [c](auto p) { return p.first == c; });
		if (it == children.end()) {
			children.emplace_back(c, node);
		}
		else {
			it->second = node;
		}
	}

	vector<pair<char, shared_ptr<UkkonenNode>>> children;
	shared_ptr<UkkonenNode> suffix_link;
	ptr<const char> begin;
	ptr<const ptr<const char>> end;
	optional<size_t> suffix_index;
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

	bool active_edge_has_node() const noexcept {
		for (auto cur = node->children.begin(); cur != node->children.end(); ++cur) {
			if (cur->first == *edge)
				return true;
		}

		return false;
	}

	shared_ptr<UkkonenNode>& get_node_of_active_edge() const noexcept {
		auto c = *edge;
		return find_if(node->children.begin(), node->children.end(), [c](auto p) { return p.first == c; })->second;
	}

	shared_ptr<UkkonenNode> node;
	ptr<const char> edge;
	size_t length;
};

class UkkonenBuilder {
public:
	UkkonenBuilder(const string& text) noexcept : text{ text } {}

	shared_ptr<Node> build() noexcept;

	shared_ptr<Node> build_generalized(const string& terminators) noexcept;

private:
	ActivePoint extend(const ptr< const ptr<const char>> ptr_to_cursor, ActivePoint&& active_point) noexcept;

	const string& text;
	shared_ptr<UkkonenNode> root = nullptr;
	size_t nb_remaining_suffixes = 0;
};
