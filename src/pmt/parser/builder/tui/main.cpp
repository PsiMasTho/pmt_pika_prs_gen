#include "parser/builder/pika_program_printer.hpp"
#include "pmt/parser/builder/id_emitter.hpp"
#include "pmt/parser/builder/pika_program.hpp"
#include "pmt/parser/builder/pika_program_emitter.hpp"
#include "pmt/parser/builder/tui/args.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/generic_ast_printer.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/ast_2.hpp"
#include "pmt/parser/grammar/grammar_from_ast.hpp"
#include "pmt/parser/grammar/grammar_printer.hpp"
#include "pmt/parser/grammar/grammar_simplifier.hpp"
#include "pmt/parser/grammar/lexer_tables.hpp"
#include "pmt/parser/grammar/parser_tables.hpp"
#include "pmt/parser/grammar/pika_program.hpp"
#include "pmt/parser/grammar/post_parse.hpp"
#include "pmt/parser/rt/generic_lexer.hpp"
#include "pmt/parser/rt/generic_parser.hpp"
#include "pmt/parser/rt/pika_parser.hpp"

#include <chrono>
#include <iostream>

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
 auto const now = std::chrono::high_resolution_clock::now();
 GenericAst::UniqueHandle ast = GenericParser::parse(GenericParser::Args(lexer, parser_tables));
 auto const later = std::chrono::high_resolution_clock::now();
 std::chrono::duration<double, std::milli> diff = later - now;
 std::cout << "Top down parsing took " << diff.count() << " ms\n";
 PostParse::transform(PostParse::Args{._ast_root = *ast});

 // GenericAstPrinter::Args ast_printer_args{._id_to_string_fn = [](GenericId::IdType id_) { return Ast::id_to_string(id_); }, ._out = std::cout, ._ast = *ast, ._indent_width = 2};
 // GenericAstPrinter::print(ast_printer_args);

 return ast;
}

auto get_grammar_ast_2(std::string const& input_grammar_) -> GenericAst::UniqueHandle {
 pmt::parser::grammar::PikaProgram const pika_program;

 auto now = std::chrono::high_resolution_clock::now();
 GenericAst::UniqueHandle ast = PikaParser::parse(pika_program, input_grammar_);
 auto later = std::chrono::high_resolution_clock::now();
 std::chrono::duration<double, std::milli> diff = later - now;
 std::cout << "Pika parsing took " << diff.count() << " ms\n";

 GenericAstPrinter::Args ast_printer_args{._id_to_string_fn = [&](GenericId::IdType id_) { return Ast2::id_to_string(id_); }, ._out = std::cout, ._ast = *ast, ._indent_width = 2};
 GenericAstPrinter::print(ast_printer_args);

 return ast;
}

void write_typed_grammar(Grammar const& grammar_) {
 std::ofstream of("output.grm");
 GrammarPrinter::print_as_tree(GrammarPrinter::Args{._out = of, ._grammar = grammar_});
}

void write_program_to_file(pmt::parser::builder::PikaProgram const& program_) {
 std::ofstream of("output.asm");
 pmt::parser::builder::PikaProgramPrinter::print(pmt::parser::builder::PikaProgramPrinter::Args{._out = of, ._program = program_});
}

}  // namespace

auto main(int argc, char const* const* argv) -> int try {
 Args args(argc, argv);

 std::string const input_grammar((std::istreambuf_iterator<char>(args._input_grammar_file)), std::istreambuf_iterator<char>());

 GenericAst::UniqueHandle ast_grammar = get_grammar_ast(input_grammar);
 // GenericAst::UniqueHandle ast_grammar_2 = get_grammar_ast_2(input_grammar);

 Grammar grammar = GrammarFromAst::make(GrammarFromAst::Args{._ast = *ast_grammar});
 GrammarSimplifier::simplify(GrammarSimplifier::Args{._grammar = grammar});
 // write_typed_grammar(grammar);

 pmt::parser::builder::PikaProgram const program(grammar);
 write_program_to_file(program);

 if (args._input_test_file.has_value()) {
  std::string const input_test((std::istreambuf_iterator<char>(*args._input_test_file)), std::istreambuf_iterator<char>());
  GenericAst::UniqueHandle ast_testfile = PikaParser::parse(program, input_test);
  GenericAstPrinter::Args ast_printer_args{._id_to_string_fn = [&](GenericId::IdType id_) { return program.get_id_table().id_to_string(id_); }, ._out = std::cout, ._ast = *ast_testfile, ._indent_width = 2};
  GenericAstPrinter::print(ast_printer_args);
 }

 // emit generated program
 pmt::parser::builder::PikaProgramEmitter emitter(pmt::parser::builder::PikaProgramEmitter::Args{
  ._program = program,
  ._header_include_path = args._pika_program_header_include_filename,
  ._output_header = args._pika_program_output_header_file,
  ._output_source = args._pika_program_output_source_file,
  ._header_skel_file = args._pika_program_header_skel_file,
  ._source_skel_file = args._pika_program_source_skel_file,
  ._class_name = args._pika_program_class_name,
  ._namespace_name = args._pika_program_namespace_name,
 });
 emitter.emit();

 // emit ID tables
 pmt::parser::builder::IdEmitter id_emitter(pmt::parser::builder::IdEmitter::Args{
  ._id_table = program.get_id_table(),
  ._strings_output_file = args._id_strings_output_file,
  ._strings_skel_file = args._id_strings_skel_file,
  ._constants_output_file = args._id_constants_output_file,
  ._constants_skel_file = args._id_constants_skel_file,
 });
 id_emitter.emit();

} catch (std::exception const& e) {
 std::cerr << std::string(e.what()) << '\n';
 return 1;
} catch (...) {
 std::cerr << "Unhandled exception!\n";
 return 1;
}
