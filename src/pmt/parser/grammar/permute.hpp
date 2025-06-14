#pragma once

#include "pmt/fw_decl.hpp"

#include <cstddef>

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst)

namespace pmt::parser::grammar {

class Permute {
  // -$ Data $-
  size_t _sequence_length;
  size_t _min_items;
  size_t _max_items;

 public:
  explicit Permute(pmt::parser::GenericAst const &ast_);  // Should be NtPermute or NtPermuteDelimited

  auto get_sequence_length() const -> size_t;
  auto get_min_items() const -> size_t;
  auto get_max_items() const -> size_t;
};

}  // namespace pmt::parser::grammar