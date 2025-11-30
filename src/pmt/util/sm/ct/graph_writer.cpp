#include "pmt/util/sm/ct/graph_writer.hpp"

#include "pmt/asserts.hpp"
#include "pmt/util/sm/ct/state_machine.hpp"
#include "pmt/util/timestamp.hpp"

#include <fstream>
#include <iomanip>
#include <ios>
#include <set>
#include <sstream>
#include <utility>

namespace pmt::util::sm::ct {
using namespace pmt::base;

auto GraphWriter::accepts_to_label(size_t accepts_) -> std::string {
 return std::to_string(accepts_);
}

auto GraphWriter::symbol_kind_to_edge_color(SymbolKindType) -> Color {
 return Color{._r = 0, ._g = 0, ._b = 0};
}

auto GraphWriter::symbol_kind_to_edge_style(SymbolKindType) -> EdgeStyle {
 return EdgeStyle::Solid;
}

auto GraphWriter::symbol_kind_to_font_flags(SymbolKindType kind_) -> FontFlags {
 return FontFlags::None;
}

auto GraphWriter::symbol_kind_to_font_color(SymbolKindType) -> Color {
 return Color{._r = 0, ._g = 0, ._b = 0};
}

auto GraphWriter::get_accepting_node_color() const -> Color {
 return Color{._r = 0, ._g = 0, ._b = 255};
}

auto GraphWriter::get_accepting_node_shape() const -> NodeShape {
 return NodeShape::DoubleCircle;
}

auto GraphWriter::get_nonaccepting_node_color() const -> Color {
 return Color{._r = 0, ._g = 0, ._b = 0};
}

auto GraphWriter::get_nonaccepting_node_shape() const -> NodeShape {
 return NodeShape::Circle;
}

auto GraphWriter::get_epsilon_edge_color() const -> Color {
 return Color{._r = 0, ._g = 255, ._b = 0};
}

auto GraphWriter::get_epsilon_edge_style() const -> EdgeStyle {
 return EdgeStyle::Solid;
}

auto GraphWriter::get_layout_direction() const -> LayoutDirection {
 return LayoutDirection::LeftToRight;
}

auto GraphWriter::get_graph_title() const -> std::string {
 return "State Machine";
}

auto GraphWriter::get_accepts_title() const -> std::string {
 return "Accepts";
}

GraphWriter::GraphWriter(StateMachine const& state_machine_, std::ostream& os_graph_)
 : _state_machine(state_machine_)
 , _os_graph(os_graph_) {
 std::ifstream is_graph_skel("/home/pmt/repos/pmt/skel/pmt/util/sm/ct/state_machine-skel.dot");
 _graph = std::string(std::istreambuf_iterator<char>(is_graph_skel), std::istreambuf_iterator<char>{});
}

void GraphWriter::write_dot() {
 replace_timestamp(_graph);
 replace_layout_direction(_graph);
 replace_accepting_node_shape(_graph);
 replace_accepting_node_color(_graph);
 replace_accepting_nodes(_graph);
 replace_nonaccepting_node_shape(_graph);
 replace_nonaccepting_node_color(_graph);
 replace_epsilon_edge_color(_graph);
 replace_epsilon_edges(_graph);
 replace_symbol_edges(_graph);
 replace_accepts_label(_graph);
 replace_accepts_table(_graph);
 replace_graph_title(_graph);

 _os_graph << _graph;
}

void GraphWriter::replace_layout_direction(std::string& str_) {
 replace_skeleton_label(str_, "LAYOUT_DIRECTION", to_string(get_layout_direction()));
}

void GraphWriter::replace_accepting_node_shape(std::string& str_) {
 replace_skeleton_label(str_, "ACCEPTING_NODE_SHAPE", to_string(get_accepting_node_shape()));
}

void GraphWriter::replace_accepting_node_color(std::string& str_) {
 replace_skeleton_label(str_, "ACCEPTING_NODE_COLOR", to_string(get_accepting_node_color()));
}

void GraphWriter::replace_accepting_nodes(std::string& str_) {
 std::string accepting_nodes_replacement;
 std::string delim;
 _state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  if (_state_machine.get_state(state_nr_)->get_accepts().popcnt() != 0) {
   accepting_nodes_replacement += std::exchange(delim, " ") + std::to_string(state_nr_);
  }
 });
 replace_skeleton_label(str_, "ACCEPTING_NODES", accepting_nodes_replacement);
}

void GraphWriter::replace_nonaccepting_node_shape(std::string& str_) {
 replace_skeleton_label(str_, "NONACCEPTING_NODE_SHAPE", to_string(get_nonaccepting_node_shape()));
}

void GraphWriter::replace_nonaccepting_node_color(std::string& str_) {
 replace_skeleton_label(str_, "NONACCEPTING_NODE_COLOR", to_string(get_nonaccepting_node_color()));
}

void GraphWriter::replace_epsilon_edge_color(std::string& str_) {
 replace_skeleton_label(str_, "EPSILON_EDGE_COLOR", to_string(get_epsilon_edge_color()));
}

void GraphWriter::replace_epsilon_edges(std::string& str_) {
 std::string epsilon_edges_replacement;
 std::string space;
 _state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) { _state_machine.get_state(state_nr_)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) { epsilon_edges_replacement += std::exchange(space, " ") + std::to_string(state_nr_) + " -> " + std::to_string(state_nr_next_) + "\n"; }); });
 replace_skeleton_label(str_, "EPSILON_EDGES", epsilon_edges_replacement);
}

void GraphWriter::replace_symbol_edges(std::string& str_) {
 // <symbol_kind -> <state_nr -> <state_nr_next -> label_text>>
 std::unordered_map<SymbolKindType, std::unordered_map<StateNrType, std::unordered_map<StateNrType, std::string>>> labels;

 _state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_state_machine.get_state(state_nr_);

  state.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
   state.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_next_, Interval<SymbolValueType> interval_) {
    std::string& label = labels[kind_][state_nr_][state_nr_next_];
    if (!label.empty()) {
     label += ", ";
    }
    label += symbols_to_label(kind_, IntervalSet<SymbolValueType>(interval_));
   });
  });
 });

 std::string space;
 std::string symbol_edges_replacement;
 for (auto const& [kind, state_nr_next_and_labels] : labels) {
  symbol_edges_replacement += std::exchange(space, " ") + "edge [color=" + to_string(symbol_kind_to_edge_color(kind)) + ", style=" + to_string(symbol_kind_to_edge_style(kind)) + "]\n";
  for (auto const& [state_nr, state_nr_next_and_label] : state_nr_next_and_labels) {
   for (auto const& [state_nr_next, label] : state_nr_next_and_label) {
    symbol_edges_replacement += " " + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next) + " [label=<" + apply_font_color(apply_font_flags(label, symbol_kind_to_font_flags(kind)), symbol_kind_to_font_color(kind)) + ">]\n";
   }
  }
 }

 replace_skeleton_label(str_, "SYMBOL_EDGES", symbol_edges_replacement);
}

void GraphWriter::replace_accepts_label(std::string& str_) {
 replace_skeleton_label(str_, "ACCEPTS_LABEL", get_accepts_title());
}

void GraphWriter::replace_accepts_table(std::string& str_) {
 std::unordered_map<AcceptsIndexType, std::set<StateNrType>> accepts;
 _state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_state_machine.get_state(state_nr_);
  state.get_accepts().for_each_key([&](AcceptsIndexType accept_index_) { accepts[accept_index_].insert(state_nr_); });
 });

 std::string accepts_table_replacement = R"(<TABLE BORDER="0" CELLBORDER="1">)";
 for (auto const& [accept, state_nrs_accepted] : accepts) {
  std::string const lhs = accepts_to_label(accept);
  accepts_table_replacement += "<TR><TD>" + lhs + ": ";
  std::string delim;
  for (StateNrType state_nr : state_nrs_accepted) {
   accepts_table_replacement += std::exchange(delim, ", ") + std::to_string(state_nr);
  }
  accepts_table_replacement += "</TD></TR>";
 }

 // Empty table causes errors unless we add something so add this
 if (accepts.empty()) {
  accepts_table_replacement += "<TR><TD></TD></TR>";
 }

 accepts_table_replacement += "</TABLE>";

 replace_skeleton_label(str_, "ACCEPTS_TABLE", accepts_table_replacement);
}

void GraphWriter::replace_graph_title(std::string& str_) {
 replace_skeleton_label(str_, "GRAPH_TITLE", get_graph_title());
}

void GraphWriter::replace_timestamp(std::string& str_) {
 replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}

auto GraphWriter::to_string(EdgeStyle edge_style_) -> std::string {
 switch (edge_style_) {
  case EdgeStyle::Dashed:
   return "dashed";
  case EdgeStyle::Dotted:
   return "dotted";
  case EdgeStyle::Solid:
   return "solid";
  case EdgeStyle::Bold:
   return "bold";
  default:
   pmt::unreachable();
 }
}

auto GraphWriter::to_string(NodeShape node_shape_) -> std::string {
 switch (node_shape_) {
  case NodeShape::Box:
   return "box";
  case NodeShape::Circle:
   return "circle";
  case NodeShape::DoubleCircle:
   return "doublecircle";
  case NodeShape::Ellipse:
   return "ellipse";
  case NodeShape::Octagon:
   return "octagon";
  case NodeShape::DoubleOctagon:
   return "doubleoctagon";
  default:
   pmt::unreachable();
 }
}

auto GraphWriter::to_string(LayoutDirection layout_direction_) -> std::string {
 switch (layout_direction_) {
  case LayoutDirection::TopToBottom:
   return "TB";
  case LayoutDirection::LeftToRight:
   return "LR";
  default:
   pmt::unreachable();
 }
}

auto GraphWriter::to_string(Color color_) -> std::string {
 std::stringstream ss;
 ss << R"("#)" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(color_._r) << std::setw(2) << static_cast<int>(color_._g) << std::setw(2) << static_cast<int>(color_._b) << R"(")";
 return ss.str();
}

auto GraphWriter::apply_font_flags(std::string str_, FontFlags flags_) -> std::string {
 if (flags_ & FontFlags::Bold) {
  str_ = "<b>" + str_ + "</b>";
 }
 if (flags_ & FontFlags::Italic) {
  str_ = "<i>" + str_ + "</i>";
 }

 return str_;
}

auto GraphWriter::apply_font_color(std::string str_, Color color_) -> std::string {
 std::string color_str = to_string(color_);
 str_ = "<font color=" + color_str + ">" + str_ + "</font>";
 return str_;
}

}  // namespace pmt::util::sm::ct