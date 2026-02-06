#include "pmt/meta/ast_utils.hpp"

#include "pmt/hash.hpp"
#include "pmt/rt/ast.hpp"

#include <utility>
#include <vector>

namespace pmt::meta {

auto AstNodeHash::operator()(AstNodeKey const& key_) const -> size_t {
 std::vector<pmt::rt::Ast const*> pending{key_._node};
 size_t seed = pmt::Hash::Phi64;

 while (!pending.empty()) {
  pmt::rt::Ast const* cur = pending.back();
  pending.pop_back();

  pmt::Hash::combine(cur->get_id(), seed);

  switch (cur->get_tag()) {
   case pmt::rt::Ast::Tag::String: {
    pmt::Hash::combine(cur->get_string(), seed);
   } break;
   case pmt::rt::Ast::Tag::Parent: {
    for (size_t i = 0; i < cur->get_children_size(); ++i) {
     pending.push_back(cur->get_child_at(i));
    }
   } break;
  }
 }
 return seed;
}

auto AstNodeEq::operator()(AstNodeKey const& lhs_, AstNodeKey const& rhs_) const -> bool {
 std::vector<std::pair<pmt::rt::Ast const*, pmt::rt::Ast const*>> pending{{lhs_._node, rhs_._node}};

 while (!pending.empty()) {
  auto [left, right] = pending.back();
  pending.pop_back();

  if (left->get_id() != right->get_id() || left->get_tag() != right->get_tag()) {
   return false;
  }

  switch (left->get_tag()) {
   case pmt::rt::Ast::Tag::String: {
    if (left->get_string() != right->get_string()) {
     return false;
    }
   } break;
   case pmt::rt::Ast::Tag::Parent: {
    if (left->get_children_size() != right->get_children_size()) {
     return false;
    }
    for (size_t i = 0; i < left->get_children_size(); ++i) {
     pending.emplace_back(left->get_child_at(i), right->get_child_at(i));
    }
   } break;
  }
 }

 return true;
}

}  // namespace pmt::meta
