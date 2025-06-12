#pragma once

#include "pmt/fw_decl.hpp"

#include <string>

PMT_FW_DECL_NS_CLASS(pmt::parser, GenericAst)

namespace pmt::parser::grammar {

class StringLiteral {
 public:
  // -$ Types / Constants $-
  using StringType = std::string;

  private:
  // -$ Data $-
  StringType _value;

  public:
  // -$ Functions $-
  // --$ Lifetime $--
  explicit StringLiteral(pmt::parser::GenericAst const &ast_);

  // --$ Other $--
  [[nodiscard]] auto get_value() const -> StringType;
};

}  // namespace pmt::parserbuilder