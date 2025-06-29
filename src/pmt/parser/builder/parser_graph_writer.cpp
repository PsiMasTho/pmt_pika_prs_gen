#include "pmt/parser/builder/parser_graph_writer.hpp"

#include "pmt/parser/primitives.hpp"

namespace pmt::parser::builder {
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::base;

ParserGraphWriter::ParserGraphWriter(StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, AcceptsToLabelFn terminal_to_name_fn_, AcceptsToLabelFn nonterminal_to_name_fn_)
 : GraphWriter(state_machine_, _os_graph)
 , _terminal_to_name_fn(std::move(terminal_to_name_fn_))
 , _nonterminal_to_name_fn(std::move(nonterminal_to_name_fn_))
 , _os_graph(output_filename_)
 , _title(title_) {
}

auto ParserGraphWriter::accepts_to_label(size_t accepts_) -> std::string {
 return _nonterminal_to_name_fn(accepts_);
}

auto ParserGraphWriter::symbols_to_label(SymbolKindType kind_, IntervalSet<SymbolValueType> const& symbols_) -> std::string {
 std::string delim;
 std::string ret;

 switch (kind_) {
  case SymbolKindTerminal:
  case SymbolKindHiddenTerminal:
   symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + _terminal_to_name_fn(symbol_); });
   break;
  case SymbolKindOpen:
   symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + "+"; });
   break;
  case SymbolKindClose:
   symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + _nonterminal_to_name_fn(symbol_); });
   break;
  case SymbolKindConflict:
   symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + std::to_string(symbol_); });
   break;
  default:
   symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + "unknown"; });
   break;
 }

 return ret;
}

auto ParserGraphWriter::symbol_kind_to_edge_color(SymbolKindType kind_) -> Color {
 switch (kind_) {
  case SymbolKindConflict:
   return GraphWriter::Color{._r = 255, ._g = 165, ._b = 0};
  case SymbolKindHiddenTerminal:
   return GraphWriter::Color{._r = 191, ._g = 191, ._b = 191};
  default:
   return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
 }
}

auto ParserGraphWriter::symbol_kind_to_edge_style(SymbolKindType kind_) -> EdgeStyle {
 switch (kind_) {
  case SymbolKindClose:
   return GraphWriter::EdgeStyle::Dashed;
  default:
   return GraphWriter::EdgeStyle::Solid;
 }
}

auto ParserGraphWriter::symbol_kind_to_font_flags(SymbolKindType kind_) -> FontFlags {
 switch (kind_) {
  case SymbolKindTerminal:
  case SymbolKindHiddenTerminal:
   return GraphWriter::FontFlags::Italic;
  case SymbolKindClose:
   return GraphWriter::FontFlags::Bold;
  default:
   return GraphWriter::FontFlags::None;
 }
}

auto ParserGraphWriter::symbol_kind_to_font_color(SymbolKindType kind_) -> Color {
 switch (kind_) {
  case SymbolKindConflict:
   return GraphWriter::Color{._r = 255, ._g = 165, ._b = 0};
  case SymbolKindHiddenTerminal:
   return GraphWriter::Color{._r = 191, ._g = 191, ._b = 191};
  default:
   return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
 }
}

auto ParserGraphWriter::get_layout_direction() const -> LayoutDirection {
 return GraphWriter::LayoutDirection::TopToBottom;
}

auto ParserGraphWriter::get_graph_title() const -> std::string {
 return _title;
}

}  // namespace pmt::parser::builder