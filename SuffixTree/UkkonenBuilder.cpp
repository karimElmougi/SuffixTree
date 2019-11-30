#include "UkkonenBuilder.h"

shared_ptr<Node> node_from_builder_node(shared_ptr<UkkonenNode> builder_node, shared_ptr<Node> parent) {
	auto view = builder_node->begin ? string_view{ builder_node->begin, builder_node->edge_length() } : string_view{};
	auto node = builder_node->is_leaf()
		? make_shared<Node>(parent, view, builder_node->suffix_index.value())
		: make_shared<Node>(parent, view);

	for (auto [c, child] : builder_node->children) {
		node->children.emplace_back(c, node_from_builder_node(child, node));
	}

	return node;
}

bool shortcut(const shared_ptr<UkkonenNode> node, const ActivePoint& active_point) noexcept {
	return active_point.length >= node->edge_length();
}

void set_suffix_index(shared_ptr<UkkonenNode> n, int edge_length, const string& text) noexcept {
	if (n->is_leaf()) {
		n->suffix_index = text.size() - edge_length;
	}
	else {
		for (auto [c, child] : n->children) {
			set_suffix_index(child, edge_length + child->edge_length(), text);
		}
	}
}

ActivePoint UkkonenBuilder::extend(const ptr< const ptr<const char>> ptr_to_cursor, ActivePoint&& active_point) noexcept {
	const auto cursor = *ptr_to_cursor;
	nb_remaining_suffixes++;
	shared_ptr<UkkonenNode> latest_active_edge_node = nullptr;

	for (; nb_remaining_suffixes > 0; ) {
		if (active_point.length == 0)
			active_point.edge = cursor;

		if (active_point.active_edge_has_node()) {
			auto& node_of_active_edge = active_point.get_node_of_active_edge();

			// skip/count trick
			if (shortcut(node_of_active_edge, active_point)) {
				const auto length = node_of_active_edge->edge_length();
				active_point.edge += length;
				active_point.length -= length;
				active_point.node = node_of_active_edge;
				continue;
			}

			// Rule 3
			if (node_of_active_edge->begin[active_point.length] == *cursor) {
				if (latest_active_edge_node != nullptr && active_point.node != root) {
					latest_active_edge_node->suffix_link = active_point.node;
					latest_active_edge_node = nullptr;
				}

				active_point.length++;
				break;
			}

			// This will never be freed, need a workaround
			const auto split_point = new ptr<const char>{ node_of_active_edge->begin + active_point.length - 1 };

			auto previous_child = node_of_active_edge;
			node_of_active_edge = make_shared<UkkonenNode>(root, node_of_active_edge->begin, split_point);

			node_of_active_edge->push_or_update_child(*cursor, make_shared<UkkonenNode>(root, cursor, ptr_to_cursor));
			previous_child->begin += active_point.length;
			node_of_active_edge->push_or_update_child(*previous_child->begin, previous_child);

			if (latest_active_edge_node != nullptr) {
				latest_active_edge_node->suffix_link = node_of_active_edge;
			}

			latest_active_edge_node = node_of_active_edge;
		}
		else {
			// Rule 2
			active_point.node->push_or_update_child(*active_point.edge, make_shared<UkkonenNode>(root, cursor, ptr_to_cursor));

			if (latest_active_edge_node != nullptr) {
				latest_active_edge_node->suffix_link = active_point.node;
				latest_active_edge_node = nullptr;
			}
		}

		nb_remaining_suffixes--;
		if (active_point.node == root && active_point.length > 0) {
			active_point.length--;
			active_point.edge = cursor - nb_remaining_suffixes + 1;
		}
		else if (active_point.node != root) {
			active_point.node = active_point.node->suffix_link;
		}
	}

	return std::move(active_point);
}

shared_ptr<Node> UkkonenBuilder::build() noexcept {
	assert(root == nullptr);
	root = make_shared<UkkonenNode>(nullptr, nullptr, nullptr);

	auto active_point = ActivePoint{ root, text.c_str(), 0 };

	auto cur = text.c_str();
	for (; cur != text.c_str() + text.size(); cur++)
		active_point = extend(&cur, move(active_point));
	cur--;

	set_suffix_index(root, 0, text);

	return node_from_builder_node(root, nullptr);
}

void trim(shared_ptr<Node> n, const string& terminators) noexcept {
	const auto pos = n->edge.find_first_of(terminators);
	if (pos != string_view::npos) {
		n->edge.remove_suffix(n->edge.size() - pos - 1);
	}

	for (auto [c, child] : n->children) {
		trim(child, terminators);
	}
}

shared_ptr<Node> UkkonenBuilder::build_generalized(const string& terminators) noexcept {
	auto root_node = build();
	trim(root_node, terminators);
	return root_node;
}