#pragma once

#include "pmt/base/hashable.hpp"
#include "pmt/base/interval_set.hpp"
#include "pmt/sm/primitives.hpp"
#include "pmt/sm/state_machine.hpp"
#include "pmt/util/skeleton_replacer_base.hpp"

#include <limits>

namespace pmt::sm {

class GraphWriter : public pmt::util::SkeletonReplacerBase {
public:
 // -$ Types / Constants $-
 enum class EdgeStyle : size_t {
  Dashed,
  Dotted,
  Solid,
  Bold
 };

 enum class NodeShape : size_t {
  Box,
  Circle,
  DoubleCircle,
  Ellipse,
  Octagon,
  DoubleOctagon,
 };

 enum class LayoutDirection : size_t {
  TopToBottom,
  LeftToRight,
 };

 enum FontFlags : size_t {
  None = 0,
  Bold = 1 << 0,
  Italic = 1 << 1,
 };

 struct Color : pmt::base::Hashable<Color> {
  uint8_t _r = std::numeric_limits<uint8_t>::max();
  uint8_t _g = std::numeric_limits<uint8_t>::max();
  uint8_t _b = std::numeric_limits<uint8_t>::max();

  auto hash() const -> size_t;
 };

 struct Transition {
  std::string _label;
  Color _edge_color;
  Color _font_color;
  EdgeStyle _edge_style = EdgeStyle::Solid;
  FontFlags _font_flags = FontFlags::None;
 };

private:
 // -$ Data $-
 std::string _graph;
 StateMachine const& _state_machine;
 std::ostream& _os_graph;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 GraphWriter(StateMachine const& state_machine_, std::ostream& os_graph_);

 // --$ Other $--
 void write_dot();

private:
 virtual auto accepts_to_label(size_t accepts_) -> std::string;
 virtual void symbol_set_to_transitions(pmt::base::IntervalSet<SymbolType> const& in_symbol_intervals_, std::vector<Transition>& out_transitions_) = 0;
 virtual auto get_accepting_node_color() const -> Color;
 virtual auto get_accepting_node_shape() const -> NodeShape;
 virtual auto get_nonaccepting_node_color() const -> Color;
 virtual auto get_nonaccepting_node_shape() const -> NodeShape;
 virtual auto get_epsilon_edge_color() const -> Color;
 virtual auto get_epsilon_edge_style() const -> EdgeStyle;
 virtual auto get_layout_direction() const -> LayoutDirection;
 virtual auto get_graph_title() const -> std::string;
 virtual auto get_accepts_title() const -> std::string;

 void replace_layout_direction(std::string& str_);
 void replace_accepting_node_shape(std::string& str_);
 void replace_accepting_node_color(std::string& str_);
 void replace_accepting_nodes(std::string& str_);
 void replace_nonaccepting_node_shape(std::string& str_);
 void replace_nonaccepting_node_color(std::string& str_);
 void replace_epsilon_edge_color(std::string& str_);
 void replace_epsilon_edges(std::string& str_);
 void replace_symbol_edges(std::string& str_);
 void replace_accepts_label(std::string& str_);
 void replace_accepts_table(std::string& str_);
 void replace_graph_title(std::string& str_);

 void replace_timestamp(std::string& str_);

 static auto to_string(EdgeStyle edge_style_) -> std::string;
 static auto to_string(NodeShape node_shape_) -> std::string;
 static auto to_string(LayoutDirection layout_direction_) -> std::string;
 static auto to_string(Color color_) -> std::string;

 static auto apply_font_flags(std::string str_, FontFlags flags_) -> std::string;
 static auto apply_font_color(std::string str_, Color color_) -> std::string;
};

}  // namespace pmt::sm