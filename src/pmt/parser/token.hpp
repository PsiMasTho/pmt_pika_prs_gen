#pragma once

#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/generic_id.hpp"
#include "pmt/parser/source_position.hpp"

namespace pmt::parser {

class Token {
 public:
  // -$ Data $-
  std::string_view _token;
  SourcePosition _source_position;
  GenericId::IdType _id = GenericId::IdUninitialized;

  // -$ Functions $-
  // --$ Lifetime $--
  Token() = default;
  Token(std::string_view token_, SourcePosition source_position_);

  // --$ Other $--
  auto to_ast() const -> GenericAst::UniqueHandle;
};

}  // namespace pmt::util::smrt