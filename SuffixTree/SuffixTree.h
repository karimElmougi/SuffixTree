#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Node.h"
#include "UkkonenBuilder.h"

using std::shared_ptr;
using std::string;
using std::string_view;
using std::vector;
using std::distance;

class SuffixTree {
public:
	
	SuffixTree(string&& text, const char terminator) noexcept : text{ text + terminator } {
		auto b = UkkonenBuilder{ this->text };
		root = b.build();
	}

	SuffixTree(const vector<string>& strings, const vector<char>& terminators) noexcept : text{}  {
		assert(strings.size() <= terminators.size());

		const auto terminators_str = string{ terminators.cbegin(), terminators.cend() };
		for (size_t i = 0; i < strings.size(); ++i) {
			text += strings[i] + terminators_str[i];
		}

		auto b = UkkonenBuilder{ this->text };
		root = b.build_generalized(terminators_str);
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

	vector<string_view> find_longest_common_substrings(const string& s, const char s_terminator, const string& t, const char t_terminator) const noexcept {
	}

	vector<string_view> find_longest_common_substrings_dp(const string& s, const string& t) const noexcept {
		auto L = vector<vector<size_t>>{};
		for (size_t i = 0; i < s.size(); ++i) L.emplace_back(t.size(), 0);

		auto z = size_t{ 0 };
		auto ret = vector<string_view>{};

		for (size_t i = 0; i < s.size(); ++i) {
			for (size_t j = 0; j < t.size(); ++j) {
				if (s[i] == t[j]) {
					if (i == 0 || j == 0) {
						L[i][j] = 1;
					}
					else {
						L[i][j] = L[i - 1][j - 1] + 1;
					}
					if (L[i][j] > z) {
						z = L[i][j];
						ret.clear();
						const auto begin = s.c_str() + i - z + 1;
						const auto end = s.c_str() + z + 1;
						ret.emplace_back(begin, static_cast<size_t>(distance(begin, end)));
					}
					else if (L[i][j] == z) {
						const auto begin = s.c_str() + i - z + 1;
						const auto end = s.c_str() + z + 1;
						ret.emplace_back(begin, static_cast<size_t>(distance(begin, end)));
					}
				}
				else {
					L[i][j] = 0;
				}
			}
		}

		return ret;
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

	shared_ptr<Node> root;
	string text;
};
