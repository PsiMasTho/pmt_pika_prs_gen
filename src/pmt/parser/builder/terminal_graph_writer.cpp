#include "pmt/parser/builder/terminal_graph_writer.hpp"

#include "pmt/parser/primitives.hpp"
#include "pmt/parser/util.hpp"

#include <cassert>

namespace pmt::parser::builder {
using namespace pmt::sm;
using namespace pmt::base;

namespace {

auto to_displayable(SymbolType symbol_) -> std::string {
 if (is_symbol_printable(symbol_)) {
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

}  // namespace

TerminalGraphWriter::TerminalGraphWriter(StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, std::string const& accepts_table_filename_, AcceptsToLabelFn terminal_to_name_fn_)
 : GraphWriter(state_machine_, _os_graph, _os_accepts_table, std::move(terminal_to_name_fn_))
 , _os_graph(output_filename_)
 , _os_accepts_table(accepts_table_filename_)
 , _title(title_) {
}

void TerminalGraphWriter::symbol_set_to_transitions(pmt::base::IntervalSet<pmt::sm::SymbolType> const& in_symbol_intervals_, std::vector<Transition>& out_transitions_) {
 GraphWriter::Transition transition;
 transition._edge_color = GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
 transition._font_color = transition._edge_color;
 std::string delim;
 in_symbol_intervals_.for_each_interval([&](Interval<SymbolType> interval_) {
  std::string delim;
  if (interval_.get_lower() == interval_.get_upper()) {
   transition._label += to_displayable(interval_.get_lower());
  } else {
   transition._label += "[" + to_displayable(interval_.get_lower()) + ".." + to_displayable(interval_.get_upper()) + "]";
  }
 });
 out_transitions_.push_back(transition);
}

auto TerminalGraphWriter::get_layout_direction() const -> LayoutDirection {
 return GraphWriter::LayoutDirection::LeftToRight;
}

auto TerminalGraphWriter::get_graph_title() const -> std::string {
 return _title;
}

}  // namespace pmt::parser::builder
