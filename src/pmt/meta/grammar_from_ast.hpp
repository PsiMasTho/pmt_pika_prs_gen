#pragma once

#include "pmt/ast/ast.hpp"
#include "pmt/meta/grammar.hpp"

namespace pmt::meta {

auto grammar_from_ast(pmt::ast::Ast::UniqueHandle const& ast_) -> Grammar;

}  // namespace pmt::meta