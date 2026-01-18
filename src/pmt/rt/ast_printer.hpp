#pragma once

#include "pmt/rt/ast_id.hpp"

#include <iosfwd>

namespace pmt::rt {
class Ast;

class AstPrinter {
public:
 // -$ Types / Constants $-
 using IndentWidthType = uint32_t;

 enum : IndentWidthType {
  IndentWidthDefault = 2,
 };

private:
 // -$ Data $-
 IndentWidthType _indent_width;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 AstPrinter(IndentWidthType indent_width_ = IndentWidthDefault);

 // --$ Virtual $--
 [[nodiscard]] virtual auto id_to_string(AstId::IdType id_) const -> std::string = 0;

 // --$ Other $--
 void print(Ast const& ast_, std::ostream& out_) const;

 void set_indent_width(IndentWidthType indent_width_);
 auto get_indent_width() const -> IndentWidthType;

private:
 auto id_to_string_internal(AstId::IdType id_) const -> std::string;
};

}  // namespace pmt::rt