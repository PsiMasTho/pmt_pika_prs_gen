#include "pmt/parser/grammar/util.hpp"

#include "pmt/base/interval_container_common.hpp"
#include "pmt/parser/grammar/charset_literal.hpp"
#include "pmt/parser/primitives.hpp"
#include "pmt/parser/util.hpp"

#include <cassert>

namespace pmt::parser::grammar {
using namespace pmt::base;
using namespace pmt::util::sm;

namespace {

auto unprintable_symbol_to_string(SymbolType symbol_) {
 assert(!is_symbol_printable(symbol_));
 return "10#" + std::to_string(symbol_);
}

auto symbol_to_string(SymbolType symbol_) -> std::string {
 return is_symbol_printable(symbol_) ? ("\"" + std::string(1, symbol_) + "\"") : unprintable_symbol_to_string(symbol_);
}

auto interval_to_string(Interval<SymbolType> interval_) -> std::string {
 std::string ret;
 if (interval_.get_lower() == interval_.get_upper()) {  // One symbol interval
  ret += symbol_to_string(interval_.get_lower());
 } else {
  ret += symbol_to_string(interval_.get_lower()) + ".." + symbol_to_string(interval_.get_upper());
 }

 return ret;
}

auto charset_to_string(IntervalSet<SymbolType> const& charset_) -> std::string {
 if (charset_.empty()) {
  return "_";
 }

 std::string ret;
 if (charset_.size() == 1 && charset_.get_by_index(0).get_lower() == charset_.get_by_index(0).get_upper()) {  // One interval
  ret += interval_to_string(charset_.get_by_index(0));
 } else {
  ret += "[";
  charset_.for_each_interval([&](Interval<SymbolType> interval_) { ret += interval_to_string(interval_); });
  ret += "]";
 }
 return ret;
}

auto charset_literal_to_printable_string_body(CharsetLiteral const& charset_literal_) {
 std::string ret;
 std::string delim;
 for (size_t i = 0; i < charset_literal_.size(); ++i) {
  ret += std::exchange(delim, ", ") + charset_to_string(charset_literal_.get_symbol_set_at(i));
 }

 return ret;
}

}  // namespace

auto charset_literal_to_printable_string(CharsetLiteral const& charset_literal_) -> std::string {
 return "{" + charset_literal_to_printable_string_body(charset_literal_) + "}" + (charset_literal_.is_hidden() ? "~" : "");
}

}  // namespace pmt::parser::grammar