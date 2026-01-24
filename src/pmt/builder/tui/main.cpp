#include "pmt/builder/id_emitter.hpp"
#include "pmt/builder/pika_program.hpp"
#include "pmt/builder/pika_program_emitter.hpp"
#include "pmt/builder/pika_program_printer.hpp"
#include "pmt/builder/terminal_dotfile_writer.hpp"
#include "pmt/builder/tui/args.hpp"
#include "pmt/meta/grammar_from_ast.hpp"
#include "pmt/meta/grammar_printer.hpp"
#include "pmt/meta/grammar_simplifier.hpp"
#include "pmt/meta/language.hpp"
#include "pmt/meta/pika_program.hpp"
#include "pmt/rt/ast.hpp"
#include "pmt/rt/ast_printer.hpp"
#include "pmt/rt/pika_parser.hpp"

#include <chrono>
#include <iostream>

using namespace pmt::rt;

namespace {

class MetaAstPrinter : public AstPrinter {
public:
 auto id_to_string(AstId::IdType id_) const -> std::string override {
  return pmt::meta::Language::id_to_string(id_);
 }
};

class TestAstPrinter : public AstPrinter {
public:
 using IdToStringFnType = std::function<std::string(AstId::IdType)>;

private:
 IdToStringFnType _id_to_string_fn;

public:
 TestAstPrinter(IdToStringFnType id_to_string_fn_)
  : AstPrinter()
  , _id_to_string_fn(std::move(id_to_string_fn_)) {
 }

 auto id_to_string(AstId::IdType id_) const -> std::string override {
  return _id_to_string_fn(id_);
 }
};

auto get_grammar_ast(std::string const& input_grammar_) -> Ast::UniqueHandle {
 pmt::meta::PikaProgram const pika_program;

 auto now = std::chrono::high_resolution_clock::now();
 Ast::UniqueHandle ast = PikaParser::parse(pika_program, input_grammar_);
 if (!ast) {
  throw std::runtime_error("Failed to parse grammar input.");
 }

 return ast;
}

}  // namespace

auto main(int argc_, char const* const* argv_) -> int try {
 pmt::builder::tui::Args args(argc_, argv_);

 std::string const input_grammar((std::istreambuf_iterator<char>(args._input_grammar_file)), std::istreambuf_iterator<char>());

 pmt::meta::Grammar grammar = pmt::meta::GrammarFromAst::make(get_grammar_ast(input_grammar));
 pmt::meta::GrammarSimplifier::simplify(grammar);
 if (args._output_grammar_file.has_value()) {
  pmt::meta::GrammarPrinter::print_as_tree(grammar, *args._output_grammar_file);
 }

 pmt::builder::PikaProgram const program(grammar);
 if (args._output_clauses_file.has_value()) {
  pmt::builder::PikaProgramPrinter::print(program, *args._output_clauses_file);
 }

 if (args._input_test_file.has_value()) {
  std::string const input_test((std::istreambuf_iterator<char>(*args._input_test_file)), std::istreambuf_iterator<char>());
  Ast::UniqueHandle ast_testfile = PikaParser::parse(program, input_test);
  if (ast_testfile != nullptr) {
   TestAstPrinter const printer([&](AstId::IdType id_) { return program.get_id_table().id_to_string(id_); });
   printer.print(*ast_testfile, std::cout);
  }
 }

 // emit generated program
 pmt::builder::PikaProgramEmitter emitter(pmt::builder::PikaProgramEmitter::Args{
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
 pmt::builder::IdEmitter id_emitter(pmt::builder::IdEmitter::Args{
  ._id_table = program.get_id_table(),
  ._strings_output_file = args._id_strings_output_file,
  ._strings_skel_file = args._id_strings_skel_file,
  ._constants_output_file = args._id_constants_output_file,
  ._constants_skel_file = args._id_constants_skel_file,
 });
 id_emitter.emit();

 if (args._terminal_graph_output_file.has_value() && args._terminal_graph_skel_file.has_value()) {
  pmt::builder::TerminalDotfileWriter dot_writer(program.get_literal_state_machine_tables().get_state_machine(), *args._terminal_graph_output_file, *args._terminal_graph_skel_file, [&](FinalIdType idx_) { return std::to_string(idx_); });
  dot_writer.write_dot();
 }

} catch (std::exception const& e) {
 std::cerr << std::string(e.what()) << '\n';
 return 1;
} catch (...) {
 std::cerr << "Unhandled exception!\n";
 return 1;
}
