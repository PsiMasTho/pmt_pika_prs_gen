#include "round_tables.hpp"   // <- generated
#include "square_tables.hpp"  // <- generated

#include <pmt/pika/rt/ast_to_string.hpp>  // <- pmt_ast, should be installed
#include <pmt/pika/rt/pika_parser.hpp>    // <- pmt_pika_rt, should be installed

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

namespace example {

struct SharedIds {
 enum : pmt::pika::rt::IdType {
#include "shared_id_constants-inl.hpp"  // <- generated, has Ids used by RoundTables and SquareTables
 };

 static auto id_to_string(pmt::pika::rt::IdType id_) -> std::string {
  static char const* const IdStrings[] = {
#include "shared_id_strings-inl.hpp"  // <- generated
  };

  if (id_ < std::size(IdStrings)) {
   return IdStrings[id_];
  }
  return "<?>";
 }
};

auto read_file(std::string const& file_path_) -> std::string {
 std::ifstream in(file_path_);
 return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

auto parse_and_print(std::string_view input_, pmt::pika::rt::PikaTablesBase const& tables_) -> int {
 pmt::pika::rt::Ast::UniqueHandle const ast = pmt::pika::rt::PikaParser::memo_table_to_ast(pmt::pika::rt::PikaParser::populate_memo_table(input_, tables_), input_, tables_);

 if (!ast) {
  std::cerr << "Parse failed." << std::endl;
  return EXIT_FAILURE;
 }

 pmt::pika::rt::AstToString const ast_to_string(example::SharedIds::id_to_string, 2);
 std::cout << ast_to_string.to_string(*ast);
 return EXIT_SUCCESS;
}

}  // namespace example

auto main(int argc, char** argv) -> int {
 char const* const usage_msg = "Expected 2 arguments: [square/round] [input_file_path]";
 switch (argc) {
  case 3:
   break;
  default:
   std::cerr << usage_msg << std::endl;
   return EXIT_FAILURE;
 }

 std::string const input = example::read_file(argv[2]);
 if (std::strcmp(argv[1], "square") == 0) {
  example::SquareTables const square_tables;
  return example::parse_and_print(input, square_tables);
 } else if (std::strcmp(argv[1], "round") == 0) {
  example::RoundTables const round_tables;
  return example::parse_and_print(input, round_tables);
 } else {
  std::cerr << usage_msg << std::endl;
  return EXIT_FAILURE;
 }
}
