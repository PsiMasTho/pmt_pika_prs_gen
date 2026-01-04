#include "parser/builder/pika_program_printer.hpp"
#include "pmt/parser/builder/pika_program.hpp"
#include "pmt/parser/builder/tui/args.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/generic_ast_printer.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/grammar_from_ast.hpp"
#include "pmt/parser/grammar/grammar_printer.hpp"
#include "pmt/parser/grammar/grammar_simplifier.hpp"
#include "pmt/parser/grammar/lexer_tables.hpp"
#include "pmt/parser/grammar/nonterminal_inliner.hpp"
#include "pmt/parser/grammar/parser_tables.hpp"
#include "pmt/parser/grammar/post_parse.hpp"
#include "pmt/parser/rt/generic_lexer.hpp"
#include "pmt/parser/rt/generic_parser.hpp"
#include "pmt/parser/rt/pika_parser.hpp"

#include <iostream>

using namespace pmt::parser::builder;
using namespace pmt::parser::grammar;
using namespace pmt::parser::rt;
using namespace pmt::parser;
using namespace pmt::parser::builder::tui;
using namespace pmt::sm;
using namespace pmt::base;

namespace {

auto get_grammar_ast(std::string const& input_grammar_) -> GenericAst::UniqueHandle {
 pmt::parser::grammar::LexerTables const lexer_tables;
 GenericLexer lexer(input_grammar_, lexer_tables);
 pmt::parser::grammar::ParserTables const parser_tables;
 GenericAst::UniqueHandle ast = GenericParser::parse(GenericParser::Args(lexer, parser_tables));
 PostParse::transform(PostParse::Args{._ast_root = *ast});

 GenericAstPrinter::Args ast_printer_args{._id_to_string_fn = [](GenericId::IdType id_) { return Ast::id_to_string(id_); }, ._out = std::cout, ._ast = *ast, ._indent_width = 2};
 GenericAstPrinter::print(ast_printer_args);

 return ast;
}

void write_typed_grammar(Grammar const& grammar_) {
 std::ofstream of("output.grm");
 GrammarPrinter::print_as_tree(GrammarPrinter::Args{._out = of, ._grammar = grammar_});
}

void write_program_to_file(PikaProgram const& program_) {
 std::ofstream of("output.asm");
 PikaProgramPrinter::print(PikaProgramPrinter::Args{._out = of, ._program = program_});
}

}  // namespace

auto main(int argc, char const* const* argv) -> int try {
 Args args(argc, argv);

 std::string const input_grammar((std::istreambuf_iterator<char>(args._input_grammar_file)), std::istreambuf_iterator<char>());

 GenericAst::UniqueHandle ast_grammar = get_grammar_ast(input_grammar);

 Grammar grammar = GrammarFromAst::make(GrammarFromAst::Args{._ast = *ast_grammar});
 GrammarSimplifier::simplify(GrammarSimplifier::Args{._grammar = grammar});
 write_typed_grammar(grammar);

 PikaProgram const program(grammar);
 write_program_to_file(program);

 if (!args._input_test_file.has_value()) {
  return 1;
 }

 std::string const input_test((std::istreambuf_iterator<char>(*args._input_test_file)), std::istreambuf_iterator<char>());

 GenericAst::UniqueHandle ast_testfile = PikaParser::parse(program, input_test);
 GenericAstPrinter::Args ast_printer_args{._id_to_string_fn = [&](GenericId::IdType id_) { return program.get_id_table().id_to_string(id_); }, ._out = std::cout, ._ast = *ast_testfile, ._indent_width = 2};
 GenericAstPrinter::print(ast_printer_args);

} catch (std::exception const& e) {
 std::cerr << std::string(e.what()) << '\n';
 return 1;
} catch (...) {
 std::cerr << "Unhandled exception!\n";
 return 1;
}
