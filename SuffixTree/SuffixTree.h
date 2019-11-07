#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <utility>
#include <unordered_set>
#include <memory>

struct Node {
	Node(size_t begin, size_t end) noexcept : transitions{}, suffix_link{}, slice{ begin, end } {
	}

	std::unordered_set<char, std::shared_ptr<Node>> transitions;
	std::pair<size_t, size_t> slice;
	std::optional<uint64_t> suffix_link;
};

enum class ConstructionMethod {
	Ukkonen,
	FarachColton,
};

class SuffixTree {
public:
	using Alphabet = std::unordered_set<char>;

	SuffixTree(const ConstructionMethod method, const Alphabet&& alphabet, std::string&& str) noexcept : nodes{}  {
		if (method == ConstructionMethod::Ukkonen) {
			nodes = ukkonen(std::move(alphabet), std::move(str));
		}
	}

private:
	static std::vector<std::shared_ptr<Node>> ukkonen(const Alphabet&& alphabet, std::string&& str) {
		validate_string(alphabet, str);

		auto nodes = std::vector<std::shared_ptr<Node>>{};

		auto root = std::make_shared<Node>(0, 1);
		auto sink = std::make_shared<Node>(0, 0);
		root->suffix_link = 1;
		sink->suffix_link = 0;

		nodes = { root, sink };
		
		str.push_back('$');

		auto curr_node = root;
		for (auto i = 0; i < str.size(); ++i) {

		}

		return nodes;
	}

	static void validate_string(const Alphabet& alphabet, const std::string& str) {
		const auto char_set = std::unordered_set<char>{ str.cbegin(), str.cend() };
		if (char_set != alphabet) {
			throw std::exception{ "str contains characters that are not contained in the alphabet" };
		}
	}

	std::vector<std::shared_ptr<Node>> nodes;
};
