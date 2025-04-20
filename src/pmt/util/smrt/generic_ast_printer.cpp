#include "pmt/util/smrt/generic_ast_printer.hpp"

#include "pmt/asserts.hpp"

#include <ostream>
#include <stack>

namespace pmt::util::smrt {

GenericAstPrinter::GenericAstPrinter(IdToStringFnType id_to_string_fn_)
 : _id_to_string_fn(std::move(id_to_string_fn_)) {
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
    out_ << indent << id_to_string(node->get_id());

    switch (node->get_tag()) {
      case GenericAst::Tag::String:
        out_ << ": " << node->get_string();
        break;
      case GenericAst::Tag::Children:
        for (size_t i = node->get_children_size(); i--;) {
          push(node->get_child_at(i), depth + 1);
        }
        break;
      default:
        pmt::unreachable();
    }

    out_ << '\n';
  }
}

auto GenericAstPrinter::id_to_string(GenericId::IdType id_) -> std::string {
  if (GenericId::is_generic_id(id_)) {
    return GenericId::id_to_string(id_);
  }
  return _id_to_string_fn(id_);
}

}  // namespace pmt::util::smrt