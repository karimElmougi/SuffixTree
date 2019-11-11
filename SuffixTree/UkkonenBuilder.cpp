#include "UkkonenBuilder.h"

shared_ptr<Node> node_from_builder_node(shared_ptr<UkkonenNode> builder_node) {
	auto view = builder_node->begin ? string_view{ builder_node->begin, builder_node->edge_length() } : string_view{};
	auto node = builder_node->is_leaf()
		? make_shared<Node>(move(view), builder_node->suffix_index)
		: make_shared<Node>(move(view));

	for (char c = 0; c < builder_node->children.size(); ++c) {
		if (builder_node->children[c])
			node->children[c] = node_from_builder_node(builder_node->children[c]);
	}

	return node;
}

bool shortcut(shared_ptr<UkkonenNode> node, const ActivePoint& active_point) noexcept {
	return active_point.length >= node->edge_length();
}

void set_suffix_index(shared_ptr<UkkonenNode> n, int label_height, const string& text) noexcept {
	if (n->is_leaf()) {
		n->suffix_index = text.size() - label_height;
	}
	else {
		for (auto c : n->children) {
			if (c)
				set_suffix_index(c, label_height + c->edge_length(), text);
		}
	}
}

ActivePoint UkkonenTreeBuilder::extend(const ptr< const ptr<const char>> ptr_to_cursor, ActivePoint&& active_point) noexcept {
	const auto cursor = *ptr_to_cursor;
	nb_remaining_suffixes++;
	shared_ptr<UkkonenNode> latest_active_edge_node = nullptr;

	for (; nb_remaining_suffixes > 0; ) {
		if (active_point.length == 0)
			active_point.edge = cursor;

		auto& node_of_active_edge = active_point.get_node_of_active_edge();

		if (node_of_active_edge) {
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

			node_of_active_edge->children[*cursor] = make_shared<UkkonenNode>(root, cursor, ptr_to_cursor);
			previous_child->begin += active_point.length;
			node_of_active_edge->children[*previous_child->begin] = previous_child;

			if (latest_active_edge_node != nullptr) {
				latest_active_edge_node->suffix_link = node_of_active_edge;
			}

			latest_active_edge_node = node_of_active_edge;
		}
		else {
			// Rule 2
			node_of_active_edge = make_shared<UkkonenNode>(root, cursor, ptr_to_cursor);

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

	return active_point;
}

shared_ptr<Node> UkkonenTreeBuilder::build() noexcept {
	root = make_shared<UkkonenNode>(nullptr, nullptr, nullptr);

	auto active_point = ActivePoint{ root, text.c_str(), 0 };

	auto curr = text.c_str();
	for (; curr != text.c_str() + text.size(); curr++)
		active_point = extend(&curr, move(active_point));
	curr--;

	set_suffix_index(root, 0, text);

	return node_from_builder_node(root);
}