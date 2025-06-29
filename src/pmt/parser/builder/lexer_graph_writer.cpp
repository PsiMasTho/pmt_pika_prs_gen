#include "pmt/parser/builder/lexer_graph_writer.hpp"

#include "pmt/parser/primitives.hpp"

#include <unordered_set>

namespace pmt::parser::builder {
using namespace pmt::util::sm::ct;
using namespace pmt::util::sm;
using namespace pmt::base;

namespace {

auto is_displayable(SymbolValueType symbol_) -> bool {
 static std::unordered_set<SymbolValueType> const DISPLAYABLES = {
  // clang-format off
      '!', '#', '$', '%', '(', ')', '*', ',', '-', '.', '@', '^', '_', '[', ']', '{', '}', '~',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  // clang-format on
 };

 return DISPLAYABLES.contains(symbol_);
}

auto to_displayable(SymbolValueType symbol_) -> std::string {
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
}  // namespace

LexerGraphWriter::LexerGraphWriter(StateMachine const& state_machine_, std::string const& title_, std::string const& output_filename_, AcceptsToLabelFn terminal_to_name_fn_)
 : GraphWriter(state_machine_, _os_graph)
 , _terminal_to_name_fn(std::move(terminal_to_name_fn_))
 , _os_graph(output_filename_)
 , _title(title_) {
}

auto LexerGraphWriter::accepts_to_label(size_t accepts_) -> std::string {
 return _terminal_to_name_fn(accepts_);
}

auto LexerGraphWriter::symbols_to_label(SymbolKindType kind_, IntervalSet<SymbolValueType> const& symbols_) -> std::string {
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

auto LexerGraphWriter::symbol_kind_to_edge_color(SymbolKindType kind_) -> Color {
 switch (kind_) {
  case SymbolKindHiddenCharacter:
   return GraphWriter::Color{._r = 191, ._g = 191, ._b = 191};
  default:
   return GraphWriter::Color{._r = 0, ._g = 0, ._b = 0};
 }
}

auto LexerGraphWriter::get_layout_direction() const -> LayoutDirection {
 return GraphWriter::LayoutDirection::LeftToRight;
}

auto LexerGraphWriter::get_graph_title() const -> std::string {
 return _title;
}

}  // namespace pmt::parser::builder