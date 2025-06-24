#include "pmt/parser/generic_ast_printer.hpp"

#include "pmt/asserts.hpp"
#include "pmt/parser/generic_ast.hpp"

#include <ostream>
#include <vector>

namespace pmt::parser {

namespace {

class StackItem {
public:
 GenericAst const* _node;
 size_t _depth;
};

class Locals {
public:
 std::vector<StackItem> _pending;
 std::string _indent_str;
};

auto take(Locals& locals_) -> StackItem {
 StackItem const node = locals_._pending.back();
 locals_._pending.pop_back();
 return node;
}

auto push(Locals& locals_, GenericAst const& node_, size_t depth_) {
 locals_._pending.push_back({._node = &node_, ._depth = depth_});
}

auto get_indent(GenericAstPrinter::Args const& args_, Locals& locals_, size_t depth_) -> std::string_view {
 if (locals_._indent_str.size() < depth_ * args_._indent_width) {
  locals_._indent_str.resize(depth_ * args_._indent_width, ' ');
 }
 return std::string_view{locals_._indent_str.data(), depth_ * args_._indent_width};
}

auto id_to_string(GenericAstPrinter::Args const& args_, GenericId::IdType id_) -> std::string {
 if (GenericId::is_generic_id(id_)) {
  return GenericId::id_to_string(id_);
 }
 return args_._id_to_string_fn(id_);
}

}  // namespace

void GenericAstPrinter::print(Args args_) {
 Locals locals;

 push(locals, args_._ast, 0);

 while (!locals._pending.empty()) {
  StackItem const cur = take(locals);
  std::string_view const indent = get_indent(args_, locals, cur._depth);
  args_._out << indent << id_to_string(args_, cur._node->get_id());

  switch (cur._node->get_tag()) {
   case GenericAst::Tag::String:
    args_._out << ": " << cur._node->get_string();
    break;
   case GenericAst::Tag::Children:
    for (size_t i = cur._node->get_children_size(); i--;) {
     push(locals, *cur._node->get_child_at(i), cur._depth + 1);
    }
    break;
   default:
    pmt::unreachable();
  }

  args_._out << '\n';
 }
}

}  // namespace pmt::parser