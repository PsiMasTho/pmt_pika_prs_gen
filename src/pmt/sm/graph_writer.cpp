#include "pmt/sm/graph_writer.hpp"

#include "pmt/asserts.hpp"
#include "pmt/base/hash.hpp"
#include "pmt/util/timestamp.hpp"

#include <fstream>
#include <iomanip>
#include <ios>
#include <set>
#include <sstream>
#include <utility>

namespace pmt::sm {
using namespace pmt::base;

auto GraphWriter::Color::hash() const -> size_t {
 size_t seed = Hash::Phi64;
 Hash::combine(_r, seed);
 Hash::combine(_g, seed);
 Hash::combine(_b, seed);
 return seed;
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

GraphWriter::GraphWriter(StateMachine const& state_machine_, std::ostream& os_graph_, std::ostream& os_accepts_table_, AcceptsToLabelFn accepts_to_label_fn_)
 : _state_machine(state_machine_)
 , _os_graph(os_graph_)
 , _os_accepts_table(os_accepts_table_)
 , _accepts_to_label_fn(std::move(accepts_to_label_fn_)) {
 if (!_accepts_to_label_fn) {
  _accepts_to_label_fn = [](AcceptsIndexType accepts_) {
   return std::to_string(accepts_);
  };
 }
 std::ifstream is_graph_skel("/home/pmt/repos/pmt/skel/pmt/sm/state_machine-skel.dot");
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
 replace_graph_title(_graph);

 _os_graph << _graph;
 write_accepts_table();
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

 for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
  if (_state_machine.get_state(state_nr)->get_accepts().popcnt() != 0) {
   accepting_nodes_replacement += std::exchange(delim, " ") + std::to_string(state_nr);
  }
 }

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

 for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
  _state_machine.get_state(state_nr)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) { epsilon_edges_replacement += std::exchange(space, " ") + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next_) + "\n"; });
 }

 replace_skeleton_label(str_, "EPSILON_EDGES", epsilon_edges_replacement);
}

void GraphWriter::replace_symbol_edges(std::string& str_) {
 struct TransitionGroupingInfo : Hashable<TransitionGroupingInfo> {
  Color _edge_color;
  EdgeStyle _edge_style;

  auto hash() const -> size_t {
   size_t seed = Hash::Phi64;
   Hash::combine(_edge_color, seed);
   Hash::combine(_edge_style, seed);
   return seed;
  }

  auto operator==(TransitionGroupingInfo const& other_) const -> bool {
   return _edge_color == other_._edge_color && _edge_style == other_._edge_style;
  }
 };

 struct TransitionLeftoverInfo {
  std::string _label;
  Color _font_color;
  FontFlags _font_flags;
 };

 // <transition_grouping_info -> <state_nr_from -> <state_nr_next -> transition_leftover_info>>
 std::unordered_map<TransitionGroupingInfo, std::unordered_map<StateNrType, std::unordered_map<StateNrType, TransitionLeftoverInfo>>> grouped_transitions;

 {
  std::vector<Transition> transitions;
  for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
   State const& state = *_state_machine.get_state(state_nr);

   std::unordered_map<StateNrType, IntervalSet<SymbolType>> symbol_intervals_per_state_nr_next;
   state.get_symbols().for_each_key([&](SymbolType symbol_) { symbol_intervals_per_state_nr_next[state.get_symbol_transition(symbol_)].insert(Interval(symbol_)); });

   {
    for (auto const& [state_nr_next, symbol_intervals] : symbol_intervals_per_state_nr_next) {
     symbol_set_to_transitions(symbol_intervals, transitions);
     for (auto const& transition : transitions) {
      TransitionGroupingInfo grouping_info{
       ._edge_color = transition._edge_color,
       ._edge_style = transition._edge_style,
      };

      TransitionLeftoverInfo leftover_info{
       ._label = transition._label,
       ._font_color = transition._font_color,
       ._font_flags = transition._font_flags,
      };

      grouped_transitions[grouping_info][state_nr][state_nr_next] = leftover_info;
     }
     transitions.clear();
    }
   }
  }
 }

 std::string space;
 std::string symbol_edges_replacement;
 for (auto const& [grouping_info, state_nr_next_and_labels] : grouped_transitions) {
  symbol_edges_replacement += std::exchange(space, " ") + "edge [color=" + to_string(grouping_info._edge_color) + ", style=" + to_string(grouping_info._edge_style) + "]\n";
  for (auto const& [state_nr, state_nr_next_and_label] : state_nr_next_and_labels) {
   for (auto const& [state_nr_next, leftover_info] : state_nr_next_and_label) {
    symbol_edges_replacement += " " + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next) + " [label=<" + apply_font_color(apply_font_flags(leftover_info._label, leftover_info._font_flags), leftover_info._font_color) + ">]\n";
   }
  }
 }

 replace_skeleton_label(str_, "SYMBOL_EDGES", symbol_edges_replacement);
}

void GraphWriter::write_accepts_table() {
 std::unordered_map<AcceptsIndexType, std::set<StateNrType>> accepts;
 for (StateNrType const state_nr : _state_machine.get_state_nrs()) {
  State const& state = *_state_machine.get_state(state_nr);
  state.get_accepts().for_each_key([&](AcceptsIndexType accept_index_) { accepts[accept_index_].insert(state_nr); });
 }

 _os_accepts_table << "Accept:StateNrs\n";
 for (auto const& [accept, state_nrs_accepted] : accepts) {
  _os_accepts_table << _accepts_to_label_fn(accept) << ": ";
  std::string delim;
  for (StateNrType state_nr : state_nrs_accepted) {
   _os_accepts_table << std::exchange(delim, ", ") << std::to_string(state_nr);
  }
  _os_accepts_table << '\n';
 }
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

}  // namespace pmt::sm
