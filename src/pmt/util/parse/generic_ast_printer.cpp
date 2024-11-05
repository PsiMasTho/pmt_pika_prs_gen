#include "pmt/util/parse/generic_ast_printer.hpp"

#include "pmt/asserts.hpp"

#include <stack>

namespace pmt::util::parse {

GenericAstPrinter::GenericAstPrinter(IdNameFunctionType id_name_function_)
 : _id_name_function(std::move(id_name_function_)) {
}

void GenericAstPrinter::print(GenericAst const& ast_, std::ostream& out_) {
  std::stack<std::pair<GenericAst const*, size_t>> stack;

  auto const push = [&](GenericAst const* node_, size_t depth_) {
    stack.push({node_, depth_});
  };

  auto const take = [&]() -> std::pair<GenericAst const*, size_t> {
    auto const node = stack.top();
    stack.pop();
    return node;
  };

  push(&ast_, 0);

  while (!stack.empty()) {
    auto const [node, depth] = take();
    std::string const indent(depth * INDENT_WIDTH, ' ');
    out_ << indent << get_name(node->get_id());

    switch (node->get_tag()) {
      case GenericAst::Tag::Token:
        out_ << ": " << node->get_token();
        break;
      case GenericAst::Tag::Children:
        for (size_t i = node->get_children_size(); i--;) {
          push(node->get_child_at(i), depth + 1);
        }
        break;
      default:
        pmt_unreachable();
    }
    
    out_ << '\n';
  }
}

auto GenericAstPrinter::get_name(GenericAst::IdType id_) -> std::string {
  switch (id_) {
    case GenericAst::IdConstants::UninitializedId:
      return "Uninitialized";
    case GenericAst::IdConstants::DefaultId:
      return "Default";
    default:
      return _id_name_function(id_);
  }
}

}  // namespace pmt::util::parse