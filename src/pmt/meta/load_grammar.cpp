#include "pmt/meta/load_grammar.hpp"

#include "pmt/meta/extract_hidden_expressions.hpp"
#include "pmt/meta/grammar_from_ast.hpp"
#include "pmt/meta/inline_rules.hpp"
#include "pmt/meta/pika_program.hpp"
#include "pmt/meta/prune_grammar.hpp"
#include "pmt/meta/shrink_grammar.hpp"
#include "pmt/rt/pika_parser.hpp"

namespace pmt::meta {
using namespace pmt::rt;
namespace {}

auto load_grammar(std::string_view input_grammar_) -> Grammar {
 pmt::meta::PikaProgram const pika_program;

 MemoTable const memo_table = PikaParser::populate_memo_table(input_grammar_, pika_program);

 Ast::UniqueHandle ast = PikaParser::memo_table_to_ast(memo_table, input_grammar_, pika_program);
 if (!ast) {
  throw std::runtime_error("Failed to parse grammar input.");
 }

 extract_hidden_expressions(*ast);
 Grammar grammar = grammar_from_ast(ast);
 prune_grammar(grammar);
 inline_rules(grammar);
 shrink_grammar(grammar);

 return grammar;
}

}  // namespace pmt::meta