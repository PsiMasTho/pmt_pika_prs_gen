#include "pmt/rt/ast_to_str.hpp"

#include "pmt/rt/ast.hpp"
#include "pmt/unreachable.hpp"

#include <vector>

namespace pmt::rt {

namespace {

class Item {
public:
 Ast const* _node;
 size_t _depth;
};

}  // namespace

AstToString::AstToString(IdToStringFnType id_to_string_fn_, IndentWidthType indent_width_)
 : _id_to_string_fn(std::move(id_to_string_fn_))
 , _indent_width(indent_width_) {
}

auto AstToString::to_string(Ast const& ast_) const -> std::string {
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

 std::string ret;

 while (!pending.empty()) {
  auto const [node, depth] = take();
  std::string_view const indent = get_indent(depth);
  ret += indent;
  ret += id_to_string_internal(node->get_id());

  switch (node->get_tag()) {
   case Ast::Tag::String:
    ret += ": ";
    ret += node->get_string();
    break;
   case Ast::Tag::Parent:
    for (size_t i = node->get_children_size(); i--;) {
     push(Item{._node = node->get_child_at(i), ._depth = depth + 1});
    }
    break;
   default:
    pmt::unreachable();
  }

  ret += '\n';
 }
 return ret;
}

void AstToString::set_indent_width(IndentWidthType indent_width_) {
 _indent_width = indent_width_;
}

auto AstToString::get_indent_width() const -> IndentWidthType {
 return _indent_width;
}

auto AstToString::id_to_string_internal(AstId::IdType id_) const -> std::string {
 return AstId::is_generic_id(id_) ? AstId::id_to_string(id_) : _id_to_string_fn(id_);
}

}  // namespace pmt::rt