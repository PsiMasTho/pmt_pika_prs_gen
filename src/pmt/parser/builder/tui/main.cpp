#include "pmt/parser/builder/grammar_to_program.hpp"
#include "pmt/parser/builder/program.hpp"
#include "pmt/parser/builder/tui/args.hpp"
#include "pmt/parser/builder/write_program.hpp"
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
#include "pmt/parser/rt/generic_vm.hpp"

#include <iostream>

using namespace pmt::parser::builder;
using namespace pmt::parser::grammar;
using namespace pmt::parser::rt;
using namespace pmt::parser;
using namespace pmt::parser::builder::tui;
using namespace pmt::util::sm;
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
 GrammarPrinter::print(GrammarPrinter::Args{._out = of, ._grammar = grammar_});
}

void write_program_to_file(Program const& program_) {
 std::ofstream of("output.asm");
 write_program(of, program_);
}

}  // namespace

auto main(int argc, char const* const* argv) -> int try {
 Args args(argc, argv);

 std::string const input_grammar((std::istreambuf_iterator<char>(args._input_grammar_file)), std::istreambuf_iterator<char>());

 GenericAst::UniqueHandle ast = get_grammar_ast(input_grammar);

 Grammar grammar = GrammarFromAst::make(GrammarFromAst::Args{._ast = *ast});
 GrammarSimplifier::simplify(GrammarSimplifier::Args{._grammar = grammar});
 write_typed_grammar(grammar);

 return 1;

 Program const program = grammar_to_program(grammar);
 write_program_to_file(program);

 if (!args._input_test_file.has_value()) {
  return 1;
 }

 std::string const input_test_file((std::istreambuf_iterator<char>(*args._input_test_file)), std::istreambuf_iterator<char>());

 GenericVm vm(program, input_test_file);

 while (vm.get_status() == GenericVm::Status::Running) {
  std::cout << "PC: " << vm.get_pc() << std::endl;
  vm.run(1);
 }

 switch (vm.get_status()) {
  case GenericVm::Status::Succeeded: {
   std::cout << "Success" << std::endl;
   GenericAst::UniqueHandle const test_file_ast = vm.get_result();
   GenericAstPrinter::Args ast_printer_args{._id_to_string_fn = [](GenericId::IdType id_) { return Ast::id_to_string(id_); }, ._out = std::cout, ._ast = *test_file_ast, ._indent_width = 2};
   GenericAstPrinter::print(ast_printer_args);
  } break;
  case GenericVm::Status::Failed:
   std::cout << "Fail" << std::endl;
   break;
  default:
   std::cout << "Unknown vm status" << std::endl;
   break;
 }
} catch (std::exception const& e) {
 std::cerr << std::string(e.what()) << '\n';
 return 1;
} catch (...) {
 std::cerr << "Unhandled exception!\n";
 return 1;
}
