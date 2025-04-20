#pragma once

#include "pmt/util/smrt/generic_ast.hpp"
#include "pmt/util/smrt/generic_id.hpp"
#include "pmt/util/smrt/source_position.hpp"

namespace pmt::util::smrt {

class Token {
 public:
  // - Data -
  std::string_view _token;
  SourcePosition _source_position;
  GenericId::IdType _id = GenericId::IdUninitialized;

  // - Member functions -
  // - Lifetime -
  Token() = default;
  Token(std::string_view token_, SourcePosition source_position_);

  // - Conversion -
  auto to_ast() const -> GenericAst::UniqueHandle;
};

}  // namespace pmt::util::smrt