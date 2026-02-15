#pragma once

#include "pmt/pika/rt/ast.hpp"

#include <functional>
#include <string>

namespace pmt::pika::rt {

class AstToString {
public:
 // -$ Types / Constants $-
 using IndentWidthType = uint32_t;
 using IdToStringFnType = std::function<std::string(IdType)>;

 enum : IndentWidthType {
  IndentWidthDefault = 2,
 };

private:
 // -$ Data $-
 IdToStringFnType _id_to_string_fn;
 IndentWidthType _indent_width;

public:
 // -$ Functions $-
 // --$ Lifetime $--
 AstToString(IdToStringFnType id_to_string_fn_, IndentWidthType indent_width_ = IndentWidthDefault);

 // --$ Other $--
 auto to_string(Ast const &ast_) const -> std::string;

 void set_id_to_string_fn(IdToStringFnType id_to_string_fn_);
 auto get_id_to_string_fn() const -> IdToStringFnType;

 void set_indent_width(IndentWidthType indent_width_);
 auto get_indent_width() const -> IndentWidthType;

private:
 auto id_to_string_internal(IdType id_) const -> std::string;
};

}  // namespace pmt::pika::rt