#include "example_tables.hpp"  // <- generated

#include <pmt/rt/ast_to_string.hpp>  // <- pmt_ast, should be installed
#include <pmt/rt/pika_parser.hpp>    // <- pmt_pika_rt, should be installed

#include <iostream>
#include <string>
#include <string_view>

namespace example {

struct Ids {
 enum : pmt::rt::IdType {
#include "id_constants-inl.hpp"  // <- generated
 };

 static auto id_to_string(pmt::rt::IdType id_) -> std::string {
  static char const* const IdStrings[] = {
#include "id_strings-inl.hpp"  // <- generated
  };

  if (id_ < std::size(IdStrings)) {
   return IdStrings[id_];
  }
  return "<?>";
 }
};

}  // namespace example

auto main(int argc, char** argv) -> int {
 char const* input = "aAaabBbcCccC";

 switch (argc) {
  case 0:
  case 1:
   break;
  case 2:
   input = argv[1];
   break;
  default:
   std::cerr << "Expected 0 or 1 argument" << std::endl;
   return EXIT_FAILURE;
 }

 example::ExampleTables const tables;

 // Actual parsing happens here
 pmt::rt::MemoTable const memo = pmt::rt::PikaParser::populate_memo_table(input, tables);

 // Construct the AST
 pmt::rt::Ast::UniqueHandle const ast = pmt::rt::PikaParser::memo_table_to_ast(memo, input, tables);

 if (!ast) {
  std::cerr << "Parse failed." << std::endl;
  return EXIT_FAILURE;
 }

 // Lets walk the ast and count how many As, Bs and Cs we have.
 size_t a_tally = 0;
 size_t b_tally = 0;
 size_t c_tally = 0;

 std::vector<pmt::rt::Ast const*> pending{ast.get()};
 while (!pending.empty()) {
  pmt::rt::Ast const& cur = *pending.back();
  pending.pop_back();

  switch (cur.get_id()) {
   case example::Ids::A:  // <- generated
    ++a_tally;
    break;
   case example::Ids::B:  // <- generated
    ++b_tally;
    break;
   case example::Ids::C:  // <- generated
    ++c_tally;
    break;
   default:
    break;
  };

  if (cur.get_tag() == pmt::rt::Ast::Tag::Parent) {
   for (size_t i = 0; i < cur.get_children_size(); ++i) {
    pending.push_back(cur.get_child_at(i));
   }
  }
 }

 // Report the counts:
 std::cout << "As: " << std::to_string(a_tally) << std::endl;
 std::cout << "Bs: " << std::to_string(b_tally) << std::endl;
 std::cout << "Cs: " << std::to_string(c_tally) << std::endl;
 std::cout << "-------------------" << std::endl;

 // Print the whole ast
 pmt::rt::AstToString const ast_to_string(example::Ids::id_to_string, 2);
 std::cout << ast_to_string.to_string(*ast);
}

// Notes:
//  - The lifetime of pmt::rt::Ast objects should only ever be managed through a UniqueHandle.
//    That means don't manually heap allocate any pmt::rt::Ast or even put them on the stack. Instead, use the pmt::rt::Ast::construct function to get a UniqueHandle
//    and treat it as you would a std::unique_ptr.