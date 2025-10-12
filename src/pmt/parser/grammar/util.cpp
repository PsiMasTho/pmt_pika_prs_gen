#include "pmt/parser/grammar/util.hpp"

#include "parser/grammar/charset_literal.hpp"
#include "pmt/base/interval_container_common.hpp"
#include "pmt/parser//util.hpp"
#include "pmt/parser/primitives.hpp"

#include <cassert>

namespace pmt::parser::grammar {
using namespace pmt::base;

namespace {

auto unprintable_symbol_to_string(SymbolType symbol_) {
 assert(!is_symbol_printable(symbol_));
 return "10#" + std::to_string(symbol_);
}

auto symbol_to_string(SymbolType symbol_) -> std::string {
 return is_symbol_printable(symbol_) ? std::string(1, symbol_) : unprintable_symbol_to_string(symbol_);
}

template <CharsetLiteral::IsHidden IS_HIDDEN_>
auto charset_literal_to_printable_string_body(CharsetLiteral const& charset_literal_) {
 std::string ret;

 size_t i = 0;
 size_t const n = charset_literal_.size();

 while (i < n) {
  while (i < n && charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).empty()) {
   ret += "\"\"";
   ++i;
  }

  if (i >= n) {
   break;
  }

  bool in_printable = false;
  while (i < n && charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_lower() == charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_upper() && is_symbol_printable(charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_lower())) {
   ret += in_printable ? "" : "\"";
   ret += +charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_lower();
   ++i;
   in_printable = true;
  }
  ret += in_printable ? "\"" : "";

  if (i >= n) {
   break;
  }

  while (i < n && charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_lower() == charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_upper() && !is_symbol_printable(charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_lower())) {
   ret += unprintable_symbol_to_string(charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_lower());
   ++i;
  }

  if (i >= n) {
   break;
  }

  while (i < n && charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_lower() != charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).get_by_index(0).get_upper()) {
   ret += "[";
   charset_literal_.get_symbol_set_at<IS_HIDDEN_>(i).for_each_interval([&](Interval<SymbolType> interval_) { ret += symbol_to_string(interval_.get_lower()) + ".." + symbol_to_string(interval_.get_upper()); });
   ret += "]";
   ++i;
  }
 }

 return ret;
}

}  // namespace

auto charset_literal_to_printable_string(CharsetLiteral const& charset_literal_) -> std::string {
 return "{" + charset_literal_to_printable_string_body<CharsetLiteral::IsHidden::No>(charset_literal_) + " / " + charset_literal_to_printable_string_body<CharsetLiteral::IsHidden::Yes>(charset_literal_) + "}";
}

}  // namespace pmt::parser::grammar