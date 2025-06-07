#include "pmt/util/smct/graph_writer.hpp"

#include "pmt/asserts.hpp"
#include "pmt/util/smct/state_machine.hpp"
#include "pmt/util/timestamp.hpp"

#include <ios>
#include <set>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <utility>
#include <iomanip>

namespace pmt::util::smct {
using namespace pmt::base;
using namespace pmt::util::smrt;

GraphWriter::StyleArgs::StyleArgs()
: _accepts_to_label_fn(accepts_to_label_default),
   _symbols_to_label_fn(symbols_to_label_default),
   _symbol_kind_to_edge_color_fn(symbol_kind_to_edge_color_default),
   _symbol_kind_to_edge_style_fn(symbol_kind_to_edge_style_default),
   _symbol_kind_to_font_flags_fn(symbol_kind_to_font_flags_default),
   _symbol_kind_to_font_color_fn(symbol_kind_to_font_color_default),
   _title("State Machine"),
   _accepts_label("Accepts"),
   _accepting_node_color{._r = 0, ._g = 0, ._b = 255},
   _epsilon_edge_color{._r = 0, ._g = 255, ._b = 0},
   _epsilon_edge_style(EdgeStyle::Solid)
{}

auto GraphWriter::accepts_to_label_default(size_t accepts_) -> std::string {
 return std::to_string(accepts_);
}

auto GraphWriter::symbols_to_label_default(SymbolKindType, IntervalSet<SymbolValueType> const& symbols_) -> std::string {
 std::string ret;
 std::string delim;

 symbols_.for_each_interval([&](Interval<SymbolValueType> interval_) {
   ret += std::exchange(delim, ", ");
   if (interval_.get_lower() == interval_.get_upper()) {
     ret += to_displayable(interval_.get_lower());
   } else {
     ret += "[" + to_displayable(interval_.get_lower()) + ".." + to_displayable(interval_.get_upper()) + "]";
   }
 });

 return ret;
}

auto GraphWriter::symbol_kind_to_edge_color_default(SymbolKindType) -> Color {
 return Color{._r = 0, ._g = 0, ._b = 0};
}

auto GraphWriter::symbol_kind_to_edge_style_default(SymbolKindType) -> EdgeStyle {
 return EdgeStyle::Solid;
}

auto GraphWriter::symbol_kind_to_font_flags_default(SymbolKindType kind_) -> FontFlags {
 return FontFlags::None;
}

auto GraphWriter::symbol_kind_to_font_color_default(SymbolKindType) -> Color {
 return Color{._r = 0, ._g = 0, ._b = 0};
}

void GraphWriter::write_dot(WriterArgs& writer_args_, StyleArgs style_args_) {
 _writer_args = &writer_args_;
 _style_args = std::move(style_args_);

 std::string graph(std::istreambuf_iterator<char>(_writer_args->_is_graph_skel), std::istreambuf_iterator<char>());

 replace_timestamp(graph);
 replace_layout_direction(graph);
 replace_accepting_node_shape(graph);
 replace_accepting_node_color(graph);
 replace_accepting_nodes(graph);
 replace_nonaccepting_node_shape(graph);
 replace_nonaccepting_node_color(graph);
 replace_epsilon_edge_color(graph);
 replace_epsilon_edges(graph);
 replace_symbol_edges(graph);
 replace_accepts_label(graph);
 replace_accepts_table(graph);
 replace_graph_title(graph);

 _writer_args->_os_graph << graph;
}

void GraphWriter::replace_layout_direction(std::string& str_) {
 replace_skeleton_label(str_, "LAYOUT_DIRECTION", to_string(_style_args._layout_direction));
}

void GraphWriter::replace_accepting_node_shape(std::string& str_) {
 replace_skeleton_label(str_, "ACCEPTING_NODE_SHAPE", to_string(_style_args._accepting_node_shape));
}

void GraphWriter::replace_accepting_node_color(std::string& str_) {
 replace_skeleton_label(str_, "ACCEPTING_NODE_COLOR", to_string(_style_args._accepting_node_color));
}

void GraphWriter::replace_accepting_nodes(std::string& str_) {
 std::string accepting_nodes_replacement;
 std::string delim;
 _writer_args->_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  if (_writer_args->_state_machine.get_state(state_nr_)->get_accepts().popcnt() != 0) {
   accepting_nodes_replacement += std::exchange(delim, " ") + std::to_string(state_nr_);
  }
 });
 replace_skeleton_label(str_, "ACCEPTING_NODES", accepting_nodes_replacement);
}

void GraphWriter::replace_nonaccepting_node_shape(std::string& str_) {
 replace_skeleton_label(str_, "NONACCEPTING_NODE_SHAPE", to_string(_style_args._nonaccepting_node_shape));
}

void GraphWriter::replace_nonaccepting_node_color(std::string& str_) {
 replace_skeleton_label(str_, "NONACCEPTING_NODE_COLOR", to_string(_style_args._nonaccepting_node_color));
}

void GraphWriter::replace_epsilon_edge_color(std::string& str_){
 replace_skeleton_label(str_, "EPSILON_EDGE_COLOR", to_string(_style_args._epsilon_edge_color));
}

void GraphWriter::replace_epsilon_edges(std::string& str_){
 std::string epsilon_edges_replacement;
 std::string space;
 _writer_args->_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) { _writer_args->_state_machine.get_state(state_nr_)->get_epsilon_transitions().for_each_key([&](StateNrType state_nr_next_) { epsilon_edges_replacement += std::exchange(space, " ") + std::to_string(state_nr_) + " -> " + std::to_string(state_nr_next_) + "\n"; }); });
 replace_skeleton_label(str_, "EPSILON_EDGES", epsilon_edges_replacement);
}

void GraphWriter::replace_symbol_edges(std::string& str_) {
 // <symbol_kind -> <state_nr -> <state_nr_next -> label_text>>
 std::unordered_map<SymbolKindType, std::unordered_map<StateNrType, std::unordered_map<StateNrType, std::string>>> labels;

 _writer_args->_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
  State const& state = *_writer_args->_state_machine.get_state(state_nr_);

  state.get_symbol_kinds().for_each_key([&](SymbolKindType kind_) {
   state.get_symbol_transitions(kind_).for_each_interval([&](StateNrType state_nr_next_, Interval<SymbolValueType> interval_) {
    std::string& label = labels[kind_][state_nr_][state_nr_next_];
    if (!label.empty()) {
      label += ", ";
    }
    label += _style_args._symbols_to_label_fn(kind_, IntervalSet<SymbolValueType>(interval_));
   });
  });
 });

 std::string space;
 std::string symbol_edges_replacement;
 for (auto const& [kind, state_nr_next_and_labels] : labels) {
  symbol_edges_replacement += std::exchange(space, " ") + "edge [color=" + to_string(_style_args._symbol_kind_to_edge_color_fn(kind)) + ", style=" + to_string(_style_args._symbol_kind_to_edge_style_fn(kind)) + "]\n";
  for (auto const& [state_nr, state_nr_next_and_label] : state_nr_next_and_labels) {
   for (auto const& [state_nr_next, label] : state_nr_next_and_label) {    
    symbol_edges_replacement += " " + std::to_string(state_nr) + " -> " + std::to_string(state_nr_next) + " [label=<" + apply_font_color(apply_font_flags(label, _style_args._symbol_kind_to_font_flags_fn(kind)), _style_args._symbol_kind_to_font_color_fn(kind)) + ">]\n";
   }
  }
 }
 
 replace_skeleton_label(str_, "SYMBOL_EDGES", symbol_edges_replacement);
}

void GraphWriter::replace_accepts_label(std::string& str_) {
 replace_skeleton_label(str_, "ACCEPTS_LABEL", _style_args._accepts_label);
}

void GraphWriter::replace_accepts_table(std::string& str_) {
 std::unordered_map<AcceptsIndexType, std::set<StateNrType>> accepts;
 _writer_args->_state_machine.get_state_nrs().for_each_key([&](StateNrType state_nr_) {
   State const& state = *_writer_args->_state_machine.get_state(state_nr_);
   state.get_accepts().for_each_key([&](AcceptsIndexType accept_index_) {
    accepts[accept_index_].insert(state_nr_);
   });
 });

 std::string accepts_table_replacement = R"(<TABLE BORDER="0" CELLBORDER="1">)";
 for (auto const& [accept, state_nrs_accepted] : accepts) {
   std::string const lhs = _style_args._accepts_to_label_fn(accept);
   accepts_table_replacement += "<TR><TD>" + lhs + ": ";
   std::string delim;
   for (StateNrType state_nr : state_nrs_accepted) {
    accepts_table_replacement += std::exchange(delim, ", ") + std::to_string(state_nr);
   }
   accepts_table_replacement += "</TD></TR>";
 }

 accepts_table_replacement += "</TABLE>";

 replace_skeleton_label(str_, "ACCEPTS_TABLE", accepts_table_replacement);
}

void GraphWriter::replace_graph_title(std::string& str_) {
 replace_skeleton_label(str_, "GRAPH_TITLE", _style_args._title);
}

void GraphWriter::replace_timestamp(std::string& str_) {
 replace_skeleton_label(str_, "TIMESTAMP", pmt::util::get_timestamp());
}

auto GraphWriter::is_displayable(SymbolValueType symbol_) -> bool {
  static std::unordered_set<SymbolValueType> const DISPLAYABLES = {
   // clang-format off
      '!', '#', '$', '%', '(', ')', '*', ',', '-', '.', '@', '^', '_',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
   // clang-format on
  };

  return DISPLAYABLES.contains(symbol_);
}

auto GraphWriter::to_displayable(SymbolValueType symbol_) -> std::string {
  if (is_displayable(symbol_)) {
    return R"(')" + std::string(1, symbol_) + R"(')";
  } else if (symbol_ == SymbolValueEoi) {
    return "@eoi";
  }

  std::stringstream ss;
  ss << std::hex << symbol_;
  std::string ret = ss.str();
  std::transform(ret.begin(), ret.end(), ret.begin(), [](auto const c_) { return std::toupper(c_); });
  return "0x" + ret;
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

}  // namespace pmt::util::smct