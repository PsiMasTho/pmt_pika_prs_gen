#pragma once

#include "pmt/meta/grammar.hpp"
#include "pmt/rt/ast.hpp"

namespace pmt::meta {

auto grammar_from_ast(pmt::rt::Ast::UniqueHandle ast_) -> Grammar;

}  // namespace pmt::meta