#pragma once

#include "pmt/util/smrt/generic_id.hpp"

namespace pmt::parserbuilder {

class GrmAst {
 public:
  enum : pmt::util::smrt::GenericId::IdType {
   #include "pmt/parserbuilder/grm_id_constants-inl.hpp"
   NtTerminalRepetition,
   NtNonterminalRepetition,
   NtTerminalHidden,
   TkComma,
   NtTerminalRange,
  };

  static auto id_to_string(pmt::util::smrt::GenericId::IdType id_) -> std::string;
};

}  // namespace pmt::parserbuilder