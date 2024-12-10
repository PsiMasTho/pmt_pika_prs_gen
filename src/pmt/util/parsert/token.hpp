#pragma once

#include "pmt/util/parsert/generic_ast.hpp"
#include "pmt/util/parsert/generic_id.hpp"
#include "pmt/util/parsert/source_position.hpp"

namespace pmt::util::parsert {

class Token {
 public:
  // - Data -
  std::string_view _token;
  SourcePosition _source_position;
  GenericId::IdType _id = GenericId::IdUninitialized;

  // - Member functions -
  //- Lifetime -
  Token(std::string_view token_, SourcePosition source_position_);
  Token() = default;

  // - Conversion -
  auto to_ast() const -> GenericAst::UniqueHandle;
};

}  // namespace pmt::util::parsert