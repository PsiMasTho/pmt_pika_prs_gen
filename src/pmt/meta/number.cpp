#include "pmt/meta/number.hpp"

#include "pmt/meta/ids.hpp"
#include "pmt/rt/ast.hpp"

#include <cassert>
#include <cmath>
#include <string_view>
#include <unordered_map>

namespace pmt::meta {
using namespace pmt::rt;

namespace {
auto split_number(std::string_view str_) -> std::pair<std::string_view, std::string_view> {
 size_t const pos = str_.find('#');
 if (pos == std::string_view::npos) {
  return {str_, {}};
 }

 return {str_.substr(0, pos), str_.substr(pos + 1)};
}

auto number_convert(std::string_view num_, Number::NumberType base_, uintmax_t max_value_) -> Number::NumberType {
 // clang-format off
  static std::unordered_map<char, Number::NumberType> const CHAR_TO_NUM{
   {'0',  0}, {'1',  1}, {'2',  2}, {'3',  3}, {'4',  4}, {'5',  5},
   {'6',  6}, {'7',  7}, {'8',  8}, {'9',  9}, {'a', 10}, {'b', 11},
   {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'g', 16}, {'h', 17},
   {'i', 18}, {'j', 19}, {'k', 20}, {'l', 21}, {'m', 22}, {'n', 23},
   {'o', 24}, {'p', 25}, {'q', 26}, {'r', 27}, {'s', 28}, {'t', 29},
   {'u', 30}, {'v', 31}, {'w', 32}, {'x', 33}, {'y', 34}, {'z', 35}};
 // clang-format on

 if (base_ < 2 || base_ > CHAR_TO_NUM.size()) {
  throw std::runtime_error("Invalid integer literal base: " + std::to_string(base_));
 }

 // Skip leading zeros
 while (!num_.empty() && num_.starts_with('0')) {
  num_.remove_prefix(1);
 }

 Number::NumberType ret = 0;
 for (Number::NumberType i = num_.size(); i-- > 0;) {
  auto const itr = CHAR_TO_NUM.find(std::tolower(num_[i]));
  if (itr == CHAR_TO_NUM.end() || itr->second >= base_) {
   throw std::runtime_error("Invalid integer literal: " + std::to_string(base_) + "#" + std::string(num_));
  }

  uintmax_t const step = itr->second * std::pow(base_, num_.size() - i - 1);

  // Check for overflow
  if (step > max_value_ || max_value_ - step < ret) {
   throw std::runtime_error("Integer literal too large: " + std::to_string(base_) + "#" + std::string(num_));
  }

  ret += step;
 }

 return ret;
}

auto single_char_as_value(Ast const& ast_, uintmax_t max_value_) -> Number::NumberType {
 switch (ast_.get_id()) {
  case Ids::CharacterLiteral: {
   return ast_.get_string().front();
  } break;
  case Ids::IntegerLiteral: {
   auto const [base_str, number_str] = split_number(ast_.get_string());
   Number::NumberType const base = number_convert(base_str, 10, std::numeric_limits<uintmax_t>::max());
   Number::NumberType const number = number_convert(number_str, base, max_value_);
   return number;
  } break;
  default: {
   throw std::runtime_error("Invalid single character value");
  } break;
 }
}
}  // namespace

Number::Number(pmt::rt::Ast const& ast_, uintmax_t max_value_)
 : _value(single_char_as_value(ast_, max_value_)) {
}

auto Number::get_value() const -> NumberType {
 return _value;
}

}  // namespace pmt::meta