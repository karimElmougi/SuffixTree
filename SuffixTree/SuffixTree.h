#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include "Node.h"
#include "UkkonenBuilder.h"

using std::shared_ptr;
using std::string;
using std::string_view;
using std::vector;
using std::distance;
using std::transform;
using std::max_element;
using std::back_inserter;

class SuffixTree {
public:
	
	SuffixTree(string&& text, const char terminator) noexcept : text{ text + terminator } {
		auto b = UkkonenBuilder{ this->text };
		root = b.build();
		leaves = get_leaves(root);
		assign_terminators(leaves, { terminator });
	}

	SuffixTree(const vector<string>& strings, const vector<char>& terminators) noexcept {
		assert(strings.size() <= terminators.size());

		const auto terminators_str = string{ terminators.cbegin(), terminators.cend() };
		for (size_t i = 0; i < strings.size(); ++i) {
			text += strings[i] + terminators_str[i];
		}

		auto b = UkkonenBuilder{ this->text };
		root = b.build_generalized(terminators_str);
		leaves = get_leaves(root);
		assign_terminators(leaves, terminators_str);
	}

	bool is_substring(const string& s) const noexcept {
		auto stack = vector<pair<shared_ptr<Node>, ptr<const char>>>{ {root, s.c_str()} };
		const auto end = s.c_str() + s.size();

		for (; !stack.empty();) {
			auto [node, cursor] = stack.back(); stack.pop_back();
			if (cursor == end)
				return true;

			if (node->is_leaf()) {
				for (const auto c : node->edge) {
					if (*cursor != c)
						break;
					++cursor;
				}
				if (cursor == end)
					return true;
			}
			else {
				for (const auto& [c, child] : node->children) {
					if (*cursor == c) {
						for (const auto c : child->edge) {
							if (*cursor == c)
								++cursor;
							else if (cursor == end)
								return true;
							else
								goto ctn;
						}
						stack.emplace_back(child, cursor);
					}
				ctn:;
				}
			}
		}

		return false;
	}

	void print_all_suffixes() const noexcept {
		for_each_suffix([](auto node) noexcept { cout << node->suffix() << " [" << node->suffix_index.value() << "]\n"; });
	}

	vector<string_view> get_all_suffixes() const noexcept {
		auto suffixes = vector<string_view>{};
		for_each_suffix([&suffixes](auto node) noexcept { suffixes.push_back(node->suffix()); });
		return suffixes;
	}

	void print_longest_repeated_substring() const noexcept {
		cout << get_longest_repeated_substring(text) << '\n';
	}

	string_view get_longest_repeated_substring(const string& text) const noexcept {
		auto max_size = size_t{ 0 };
		auto i = size_t{ 0 };
		auto stack = vector<shared_ptr<Node>>{ root };

		for (; !stack.empty();) {
			const auto node = stack.back(); stack.pop_back();

			if (!node->is_leaf()) {
				for (const auto& [c, child] : node->children) {
					stack.push_back(child);
				}
			}
			else if (max_size < node->suffix().size() - node->edge.size()) {
				max_size = node->suffix().size() - node->edge.size();
				i = node->suffix_index.value();
			}
		}

		auto view = string_view{ text.c_str() + i, max_size + 1 };
		view.remove_suffix(1);

		return view;
	}

	vector<string_view> find_all_common_substrings(const vector<char>& terminators) const noexcept {
		auto max_size = size_t{ 0 };
		auto i = size_t{ 0 };
		auto stack = vector<shared_ptr<Node>>{ root };
		auto common_nodes = vector<shared_ptr<Node>>{};

		for (; !stack.empty();) {
			const auto node = stack.back(); stack.pop_back();

			if (!node->is_leaf()) {
				for (const auto& [c, child] : node->children) {
					stack.push_back(child);
				}

				if (node->contains_all_terminators(terminators)) {
					common_nodes.push_back(node);
				}
			}
		}

		auto substrings = vector<string_view>{};
		transform(common_nodes.cbegin(), common_nodes.cend(), back_inserter(substrings), [](auto n) { return n->suffix(); });
		return substrings;
	}

	string_view find_longest_common_substring(const vector<char>& terminators) const noexcept {
		const auto common_substrings = find_all_common_substrings(terminators);
		const auto max_elem = max_element(common_substrings.cbegin(), common_substrings.cend(), [](auto a, auto b) { return a.size() < b.size(); });
		return *max_elem;
	}

private:

	template<typename Func>
	void for_each_suffix(Func func) const noexcept {
		auto stack = vector<shared_ptr<Node>>{ root };

		for (; !stack.empty();) {
			const auto node = stack.back(); stack.pop_back();

			if (node->is_leaf()) {
				func(node);
				continue;
			}

			for (const auto [c, child] : node->children) {
				stack.push_back(child);
			}
		}
	}

	static vector<shared_ptr<Node>> get_leaves(shared_ptr<Node> root) noexcept {
		auto stack = vector<shared_ptr<Node>>{ root };
		auto leaves = vector<shared_ptr<Node>>{};

		for (; !stack.empty();) {
			const auto node = stack.back(); stack.pop_back();

			if (node->is_leaf()) {
				leaves.push_back(node);
			}
			else {
				for (const auto& [c, child] : node->children) {
					stack.push_back(child);
				}
			}
		}

		return leaves;
	}

	static void assign_terminators(const vector<shared_ptr<Node>> &leaves, const string& terminators) noexcept {
		for (auto leaf : leaves) {
			for (const auto c : leaf->edge) {
				if (terminators.find(c) != string::npos) {
					leaf->terminators.insert(c);
					for (auto cur = leaf->parent; cur; cur = cur->parent) {
						cur->terminators.insert(c);
					}
				}
			}
		}
	}

	shared_ptr<Node> root;
	vector<shared_ptr<Node>> leaves;
	string text;
};
