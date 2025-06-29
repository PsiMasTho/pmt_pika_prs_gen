#include "pmt/parser/builder/parser_lookahead_graph_writer.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parser/primitives.hpp"

namespace pmt::parser::builder {
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::base;

namespace {}

ParserLookaheadGraphWriter::ParserLookaheadGraphWriter(StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, AcceptsToLabelFn terminal_to_name_fn_)
 : GraphWriter(state_machine_, _os_graph)
 , _terminal_to_name_fn(std::move(terminal_to_name_fn_))
 , _os_graph(output_filename_)
 , _title(title_) {
}

auto ParserLookaheadGraphWriter::accepts_to_label(size_t accepts_) -> std::string {
 return "conflict_" + std::to_string(accepts_);
}

auto ParserLookaheadGraphWriter::symbols_to_label(SymbolKindType kind_, IntervalSet<SymbolValueType> const& symbols_) -> std::string {
 std::string delim;
 std::string ret;

 switch (kind_) {
  case SymbolKindTerminal:
   symbols_.for_each_key([&](SymbolValueType symbol_) { ret += std::exchange(delim, ", ") + _terminal_to_name_fn(symbol_); });
   break;
  default:
   pmt::unreachable();
 }

 return ret;
}

auto ParserLookaheadGraphWriter::symbol_kind_to_edge_color(SymbolKindType kind_) -> Color {
 switch (kind_) {
  case SymbolKindHiddenCharacter:
   return GraphWriter::Color{._r = 191, ._g = 191, ._b = 191};
  default:
   return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
 }
}

auto ParserLookaheadGraphWriter::symbol_kind_to_font_flags(pmt::util::sm::SymbolKindType kind_) -> FontFlags {
 switch (kind_) {
  case SymbolKindTerminal:
   return GraphWriter::FontFlags::Italic;
  default:
   return GraphWriter::FontFlags::None;
 }
}

auto ParserLookaheadGraphWriter::get_layout_direction() const -> LayoutDirection {
 return GraphWriter::LayoutDirection::LeftToRight;
}

auto ParserLookaheadGraphWriter::get_graph_title() const -> std::string {
 return _title;
}

}  // namespace pmt::parser::builder