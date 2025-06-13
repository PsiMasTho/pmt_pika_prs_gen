#pragma once

#include "pmt/parser/generic_id.hpp"

namespace pmt::parser::grammar {

class Ast {
 public:
  enum : GenericId::IdType {
#include "pmt/parser/grammar/id_constants-inl.hpp"
    NtTerminalHidden,
  };

  static auto id_to_string(GenericId::IdType id_) -> std::string;
};

}  // namespace pmt::parser::grammar