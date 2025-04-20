#include "pmt/parserbuilder/grm_number.hpp"

#include "pmt/parserbuilder/grm_ast.hpp"
#include "pmt/util/smrt/generic_ast.hpp"

#include <cassert>
#include <cmath>
#include <unordered_map>

namespace pmt::parserbuilder {
using namespace pmt::util::smrt;

auto GrmNumber::single_char_as_value(GenericAst const& ast_) -> NumberType {
  if (ast_.get_id() == GrmAst::TkStringLiteral) {
    std::string const& token = ast_.get_string();
    assert(token.size() == 1);
    return token[0];
  }

  if (ast_.get_id() == GrmAst::TkIntegerLiteral) {
    auto const [base_str, number_str] = split_number(ast_.get_string());
    NumberType const base = number_convert(base_str, 10);
    NumberType const number = number_convert(number_str, base);
    return number;
  }

  throw std::runtime_error("Invalid single character value");
}

auto GrmNumber::split_number(std::string_view str_) -> std::pair<std::string_view, std::string_view> {
  size_t const pos = str_.find('#');
  if (pos == std::string_view::npos) {
    return {str_, {}};
  }

  return {str_.substr(0, pos), str_.substr(pos + 1)};
}

auto GrmNumber::number_convert(std::string_view str_, NumberType base_) -> NumberType {
  // clang-format off
  static std::unordered_map<char, NumberType> const CHAR_TO_NUM{
   {'0',  0}, {'1',  1}, {'2',  2}, {'3',  3}, {'4',  4}, {'5',  5},
   {'6',  6}, {'7',  7}, {'8',  8}, {'9',  9}, {'a', 10}, {'b', 11},
   {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'g', 16}, {'h', 17},
   {'i', 18}, {'j', 19}, {'k', 20}, {'l', 21}, {'m', 22}, {'n', 23},
   {'o', 24}, {'p', 25}, {'q', 26}, {'r', 27}, {'s', 28}, {'t', 29},
   {'u', 30}, {'v', 31}, {'w', 32}, {'x', 33}, {'y', 34}, {'z', 35}};
  // clang-format on

  if (base_ < 2 || base_ > CHAR_TO_NUM.size()) {
    throw std::runtime_error("Invalid base: " + std::to_string(base_));
  }

  // Skip leading zeros
  while (!str_.empty() && str_.starts_with('0')) {
    str_.remove_prefix(1);
  }

  NumberType ret = 0;
  for (NumberType i = str_.size(); i-- > 0;) {
    auto const itr = CHAR_TO_NUM.find(str_[i]);
    if (itr == CHAR_TO_NUM.end() || itr->second >= base_) {
      throw std::runtime_error("Invalid number: " + std::string(str_));
    }

    NumberType const step = itr->second * std::pow(base_, str_.size() - i - 1);

    // Check for overflow
    if (std::numeric_limits<NumberType>::max() - step < ret) {
      throw std::runtime_error("Number too large: " + std::string(str_));
    }

    ret += step;
  }

  return ret;
}

auto GrmNumber::get_repetition_number(GenericAst const& token_) -> std::optional<NumberType> {
  if (token_.get_id() == GrmAst::TkComma) {
    return std::nullopt;
  }

  assert(token_.get_id() == GrmAst::TkIntegerLiteral);
  auto const [base_str, number_str] = split_number(token_.get_string());
  NumberType const base = number_convert(base_str, 10);
  NumberType const number = number_convert(number_str, base);

  return number;
}

auto GrmNumber::get_repetition_range(GenericAst const& repetition_) -> RepetitionRangeType {
  assert(repetition_.get_id() == GrmAst::NtTerminalRepetitionRange);

  switch (repetition_.get_children_size()) {
    case 1: {
      std::optional<NumberType> const mid = get_repetition_number(*repetition_.get_child_at(0));
      return {mid.value_or(0), mid};
    }
    case 2: {
      std::optional<NumberType> const lhs = get_repetition_number(*repetition_.get_child_at(0));
      std::optional<NumberType> const rhs = get_repetition_number(*repetition_.get_child_at(1));
      return {lhs.value_or(0), rhs};
    }
    case 3: {
      std::optional<NumberType> const lhs = get_repetition_number(*repetition_.get_child_at(0));
      std::optional<NumberType> const rhs = get_repetition_number(*repetition_.get_child_at(2));
      return {lhs.value_or(0), rhs};
    }
    default:
      throw std::runtime_error("Invalid repetition range");
  }
}

}  // namespace pmt::parserbuilder