#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>

namespace pmt::sandbox::glyphscribe {

using NodeIdType = std::uint64_t;

class Node {
 // Symbol -> <NodeIdNext, Weight>
 std::unordered_map<char, std::pair<NodeIdType, double>> _transitions;

public:
 void add_transition(char symbol_, NodeIdType node_id_next_, double cost_);
 auto get_transition(char symbol_) -> std::pair<NodeIdType, double> const*;
 void remove_transition(char symbol_);
};

class WeightedTrie {
private:
 // -$ Data $-
 std::map<NodeIdType, Node> _nodes;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 WeightedTrie() = default;

 // --$ Other $--
 auto get_node(NodeIdType node_id_) -> Node*;
 auto get_node(NodeIdType node_id_) const -> Node const*;

 auto get_or_create_node(NodeIdType node_id_) -> Node&;
 auto get_unused_node_nr() const -> NodeIdType;
 auto create_new_node() -> NodeIdType;

 void remove_node(NodeIdType node_id_);

 auto get_node_count() const -> size_t;
};

}  // namespace pmt::sandbox::glyphscribe