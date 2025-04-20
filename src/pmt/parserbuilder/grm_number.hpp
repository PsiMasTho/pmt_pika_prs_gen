#pragma once

#include "pmt/fw_decl.hpp"

#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

PMT_FW_DECL_NS_CLASS(pmt::util::smrt, GenericAst)

namespace pmt::parserbuilder {

class GrmNumber {
 public:
  using NumberType = uint64_t;
  using RepetitionRangeType = std::pair<NumberType, std::optional<NumberType>>;

  static auto single_char_as_value(pmt::util::smrt::GenericAst const &ast_) -> NumberType;
  static auto split_number(std::string_view str_) -> std::pair<std::string_view, std::string_view>;
  static auto number_convert(std::string_view str_, NumberType base_) -> NumberType;
  static auto get_repetition_number(pmt::util::smrt::GenericAst const &token_) -> std::optional<NumberType>;
  static auto get_repetition_range(pmt::util::smrt::GenericAst const &repetition_) -> RepetitionRangeType;
};

}  // namespace pmt::parserbuilder