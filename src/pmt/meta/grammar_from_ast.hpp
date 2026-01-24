#pragma once

#include "pmt/meta/grammar.hpp"
#include "pmt/rt/ast.hpp"

namespace pmt::meta {

class GrammarFromAst {
public:
 // --$ Other $--
 static auto make(pmt::rt::Ast::UniqueHandle ast_) -> Grammar;
};

}  // namespace pmt::meta