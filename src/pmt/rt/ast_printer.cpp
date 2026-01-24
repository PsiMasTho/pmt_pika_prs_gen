#include "pmt/rt/ast_printer.hpp"

#include "pmt/rt/ast.hpp"
#include "pmt/unreachable.hpp"

#include <ostream>
#include <vector>

namespace pmt::rt {

namespace {

class Item {
public:
 Ast const* _node;
 size_t _depth;
};

}  // namespace

AstPrinter::AstPrinter(IndentWidthType indent_width_)
 : _indent_width(indent_width_) {
}

void AstPrinter::print(Ast const& ast_, std::ostream& out_) const {
 std::vector<Item> pending;
 std::string indent_str;

 auto const push = [&](Item item_) {
  pending.push_back(item_);
 };

 auto const take = [&]() -> Item {
  Item const item = pending.back();
  pending.pop_back();
  return item;
 };

 auto const get_indent = [&](size_t depth_) -> std::string_view {
  if (indent_str.size() < depth_ * _indent_width) {
   indent_str.resize(depth_ * _indent_width, ' ');
  }
  return std::string_view{indent_str.data(), depth_ * _indent_width};
 };

 push(Item{._node = &ast_, ._depth = 0});

 while (!pending.empty()) {
  auto const [node, depth] = take();
  std::string_view const indent = get_indent(depth);
  out_ << indent << id_to_string_internal(node->get_id());

  switch (node->get_tag()) {
   case Ast::Tag::String:
    out_ << ": " << node->get_string();
    break;
   case Ast::Tag::Parent:
    for (size_t i = node->get_children_size(); i--;) {
     push(Item{._node = node->get_child_at(i), ._depth = depth + 1});
    }
    break;
   default:
    pmt::unreachable();
  }

  out_ << '\n';
 }
}

void AstPrinter::set_indent_width(IndentWidthType indent_width_) {
 _indent_width = indent_width_;
}

auto AstPrinter::get_indent_width() const -> IndentWidthType {
 return _indent_width;
}

auto AstPrinter::id_to_string_internal(AstId::IdType id_) const -> std::string {
 return AstId::is_generic_id(id_) ? AstId::id_to_string(id_) : id_to_string(id_);
}

}  // namespace pmt::rt