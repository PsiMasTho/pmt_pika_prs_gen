#include "pmt/meta/literal_to_str.hpp"

#include "pmt/meta/charset_literal.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <utility>

namespace pmt::meta {
using namespace pmt::rt;
using namespace pmt::container;

namespace {

auto interval_is_range(Interval<SymbolType> interval_) -> bool {
 return interval_.get_lower() != interval_.get_upper();
}

auto symbol_set_is_single_character(Charset::SetType const& values_) -> bool {
 return values_.size() == 1 && !interval_is_range(values_.get_by_index(0));
}

auto symbol_set_is_single_quotable_character(Charset::SetType const& values_) -> bool {
 return symbol_set_is_single_character(values_) && get_string_body_characters().contains(values_.get_by_index(0).get_lower());
}

}  // namespace

auto get_string_body_characters() -> IntervalSet<SymbolType> const& {
 static auto const ret = []() {
  IntervalSet<SymbolType> tmp;
  tmp.insert(Interval<SymbolType>(32, 33));
  tmp.insert(Interval<SymbolType>(35, 126));
  return tmp;
 }();
 return ret;
}

auto character_to_integer_literal(SymbolType symbol_, size_t base_) -> std::string {
 static std::string_view const ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
 base_ = std::min(std::max<size_t>(base_, pmt::util::min_value_base), ALPHABET.size());
 return std::to_string(base_) + "#" + pmt::util::uint_to_string(symbol_, 0, ALPHABET.substr(0, base_));
}

auto character_to_grammar_string(SymbolType symbol_) -> std::string {
 return get_string_body_characters().contains(symbol_) ? ("\"" + std::string(1, symbol_) + "\"") : character_to_integer_literal(symbol_);
}

auto interval_to_range_grammar_string(Interval<SymbolType> interval_) -> std::string {
 std::string ret = character_to_grammar_string(interval_.get_lower());

 if (interval_is_range(interval_)) {
  ret += ".." + character_to_grammar_string(interval_.get_upper());
 }

 return ret;
}

auto symbol_set_to_grammar_string(Charset::SetType const& charset_) -> std::string {
 std::string ret;
 if (symbol_set_is_single_character(charset_)) {  // Can be written as a character literal without braces
  ret += character_to_grammar_string(charset_.get_by_index(0).get_lower());
 } else {
  ret += "[";
  std::string delim;
  charset_.for_each_interval([&](Interval<SymbolType> interval_) { ret += std::exchange(delim, ", ") + interval_to_range_grammar_string(interval_); });
  ret += "]";
 }
 return ret;
}

auto charset_literal_to_grammar_string(CharsetLiteral const& charset_literal_) -> std::string {
 std::string ret;

 std::string sp;
 for (size_t i = 0; i < charset_literal_.size();) {
  std::string quotable;
  while (i < charset_literal_.size() && symbol_set_is_single_quotable_character(charset_literal_.get_symbol_set_at(i))) {
   quotable += std::string(1, charset_literal_.get_symbol_set_at(i).get_by_index(0).get_lower());
   ++i;
  }
  if (!quotable.empty()) {
   ret += std::exchange(sp, " ") + "\"" + quotable + "\"";
  }

  if (i < charset_literal_.size() && symbol_set_is_single_character(charset_literal_.get_symbol_set_at(i))) {
   ret += std::exchange(sp, " ") + character_to_integer_literal(charset_literal_.get_symbol_set_at(i).get_by_index(0).get_lower());
   ++i;
   continue;
  }

  if (i < charset_literal_.size()) {
   ret += std::exchange(sp, " ") + symbol_set_to_grammar_string(charset_literal_.get_symbol_set_at(i));
   ++i;
  }
 }

 return ret;
}

}  // namespace pmt::meta