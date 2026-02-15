#pragma once

#include "pmt/pika/meta/grammar.hpp"
#include "pmt/pika/rt/ast.hpp"

namespace pmt::pika::meta {

auto grammar_from_ast(pmt::pika::rt::Ast::UniqueHandle ast_) -> Grammar;

}  // namespace pmt::pika::meta