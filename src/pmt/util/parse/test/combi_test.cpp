#include "pmt/util/parse/combi/test/combi_test.hpp"

#include "pmt/util/parse/combi/combi.hpp"
#include "pmt/util/parse/generic_ast.hpp"
#include "pmt/util/text_encoding.hpp"

#include <iostream>

namespace pmt::util::parse::test {

enum id : generic_ast_base::id_type {
  NUMBER = 0,
  RESULT
};

using combi = pmt::util::parse::combi::combi<ENCODING_UTF8>;
using generic_ast = combi::generic_ast;

using number = combi::merge<combi::plus<NUMBER, combi::ch_range<'0', '9'>>>;
using dot = combi::hide<combi::ch<'.'>>;

using result = combi::seq<RESULT, number, dot, number>;

auto id_to_string(generic_ast_base::id_type id) -> std::string {
  switch (id) {
    case NUMBER:
      return "NUMBER";
    case RESULT:
      return "RESULT";
    case generic_ast_base::DEFAULT_ID:
      return "DEFAULT_ID";
    default:
      return "UNKNOWN";
  }
}

void print_result_ast(generic_ast* ast, std::size_t depth = 0) {
  if (ast == nullptr) {
    return;
  }

  for (std::size_t i = 0; i < depth; ++i) {
    std::cout << "  ";
  }

  std::cout << id_to_string(ast->get_id()) << " ";

  if (ast->get_tag() == generic_ast_base::TAG_TOKEN) {
    std::cout << ast->get_token() << std::endl;
  } else {
    std::cout << std::endl;
    for (std::size_t i = 0; i < ast->get_children_size(); ++i) {
      print_result_ast(ast->get_child_at(i), depth + 1);
    }
  }
}

void combi_test::run() {
  std::string const sample = "123111313211.456123213132121";

  combi::context ctx;
  ctx._begin = sample.c_str();
  ctx._cursor = ctx._begin;
  ctx._end = ctx._begin + sample.size();

  generic_ast::unique_handle ast(result::exec(ctx), generic_ast::destruct);
  assert(ast != nullptr);
  assert(ast->get_id() == RESULT);

  print_result_ast(ast.get());
}
}  // namespace pmt::util::parse::test