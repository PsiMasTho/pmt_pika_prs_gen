#include "pmt/parser/grammar/string_literal.hpp"

#include "pmt/parser/generic_ast.hpp"

#include <cassert>

namespace pmt::parser::grammar {

namespace {

auto unquoted(std::string const &str_) -> std::string {
 assert(!str_.empty() && str_.front() == '"' && str_.back() == '"');
 return str_.substr(1, str_.size() - 2);
}

}  // namespace

StringLiteral::StringLiteral(pmt::parser::GenericAst const &ast_)
 : _value(unquoted(ast_.get_string())) {
}

auto StringLiteral::get_value() const -> StringType {
 return _value;
}

}  // namespace pmt::parser::grammar