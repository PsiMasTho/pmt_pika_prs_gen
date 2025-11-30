#include "pmt/parser/builder/terminal_graph_writer.hpp"

#include "pmt/parser/builder/packed_symbol.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/parser/util.hpp"

#include <cassert>

namespace pmt::parser::builder {
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::base;

namespace {

auto to_displayable(SymbolValueType symbol_) -> std::string {
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

TerminalGraphWriter::TerminalGraphWriter(StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, AcceptsToLabelFn terminal_to_name_fn_)
 : GraphWriter(state_machine_, _os_graph)
 , _terminal_to_name_fn(std::move(terminal_to_name_fn_))
 , _os_graph(output_filename_)
 , _title(title_) {
}

auto TerminalGraphWriter::accepts_to_label(size_t accepts_) -> std::string {
 return _terminal_to_name_fn(accepts_);
}

auto TerminalGraphWriter::symbols_to_label(SymbolKindType kind_, IntervalSet<SymbolValueType> const& symbols_) -> std::string {
 std::string ret;
 std::string delim;

 switch (kind_) {
  case SymbolKindCharacter:
  case SymbolKindHiddenCharacter: {
   symbols_.for_each_interval([&](Interval<SymbolValueType> interval_) {
    ret += std::exchange(delim, ", ");
    if (interval_.get_lower() == interval_.get_upper()) {
     ret += to_displayable(interval_.get_lower());
    } else {
     ret += "[" + to_displayable(interval_.get_lower()) + ".." + to_displayable(interval_.get_upper()) + "]";
    }
   });
  } break;
  case SymbolKindOpen: {
   assert(symbols_.size() == 1 && symbols_.get_by_index(0).get_lower() == symbols_.get_by_index(0).get_upper());
   return "+";
  } break;
  case SymbolKindClose: {
   ret += "-";
   symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + _terminal_to_name_fn(symbol_); });
  } break;
  case SymbolKindOrderedEpsilonOpen: {
   ret += "+";
   symbols_.for_each_key([&](SymbolValueType symbol_) {
    PackedSymbol sym(symbol_);
    ret += std::exchange(delim, ", ") + "[" + std::to_string(sym.get_unpacked_lower()) + " : " + std::to_string(sym.get_unpacked_upper()) + "]";
   });
  } break;
  case SymbolKindOrderedEpsilonClose: {
   ret += "-";
   symbols_.for_each_key([&](SymbolValueType symbol_) {
    PackedSymbol sym(symbol_);
    ret += std::exchange(delim, ", ") + "[" + std::to_string(sym.get_unpacked_lower()) + " : " + std::to_string(sym.get_unpacked_upper()) + "]";
   });
  } break;
 }

 return ret;
}

auto TerminalGraphWriter::symbol_kind_to_edge_style(pmt::util::sm::SymbolKindType kind_) -> EdgeStyle {
 switch (kind_) {
  case SymbolKindClose:
   return GraphWriter::EdgeStyle::Dashed;
  default:
   return GraphWriter::EdgeStyle::Solid;
 }
}

auto TerminalGraphWriter::symbol_kind_to_edge_color(SymbolKindType kind_) -> Color {
 switch (kind_) {
  case SymbolKindHiddenCharacter:
   return GraphWriter::Color{._r = 191, ._g = 191, ._b = 191};
  case SymbolKindOrderedEpsilonOpen:
   return GraphWriter::Color{._r = 0, ._g = 127, ._b = 0};
  case SymbolKindOrderedEpsilonClose:
   return GraphWriter::Color{._r = 255, ._g = 142, ._b = 142};
  default:
   return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
 }
}

auto TerminalGraphWriter::symbol_kind_to_font_flags(SymbolKindType kind_) -> FontFlags {
 switch (kind_) {
  case SymbolKindClose:
   return FontFlags::Bold;
  default:
   return FontFlags::None;
 }
}

auto TerminalGraphWriter::get_layout_direction() const -> LayoutDirection {
 return GraphWriter::LayoutDirection::LeftToRight;
}

auto TerminalGraphWriter::get_graph_title() const -> std::string {
 return _title;
}

}  // namespace pmt::parser::builder