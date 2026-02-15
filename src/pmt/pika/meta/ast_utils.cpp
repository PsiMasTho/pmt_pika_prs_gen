#include "pmt/pika/meta/ast_utils.hpp"

#include "pmt/hash.hpp"
#include "pmt/pika/meta/ids.hpp"
#include "pmt/pika/rt/ast.hpp"
#include "pmt/pika/rt/rule_parameters_base.hpp"

#include <cassert>
#include <utility>
#include <vector>

namespace pmt::pika::meta {
using namespace pmt::pika::rt;

namespace {

void add_identifier(pmt::pika::rt::Ast& ast_dest_production_, std::string const& rule_name_) {
 Ast::UniqueHandle identifier = Ast::construct(Ast::Tag::String, Ids::Identifier);
 identifier->set_string(rule_name_);
 ast_dest_production_.give_child_at_back(std::move(identifier));
}

void add_parameters(pmt::pika::rt::Ast& ast_dest_production_, RuleParametersBase const& rule_parameters_, std::string const& rule_name_) {
 assert(ast_dest_production_.get_id() == Ids::Production);
 std::string const boolalpha[2] = {"false", "true"};

 if (rule_parameters_.get_display_name() != rule_name_) {
  Ast::UniqueHandle parameter_display_name = Ast::construct(Ast::Tag::String, Ids::ParameterDisplayName);
  parameter_display_name->set_string(std::string(rule_parameters_.get_display_name()));
  ast_dest_production_.give_child_at_back(std::move(parameter_display_name));
 }
 if (rule_parameters_.get_merge() != pmt::pika::rt::RuleParametersBase::MERGE_DEFAULT) {
  Ast::UniqueHandle parameter_merge = Ast::construct(Ast::Tag::String, Ids::ParameterMerge);
  parameter_merge->set_string(boolalpha[rule_parameters_.get_merge()]);
  ast_dest_production_.give_child_at_back(std::move(parameter_merge));
 }
 if (rule_parameters_.get_unpack() != pmt::pika::rt::RuleParametersBase::UNPACK_DEFAULT) {
  Ast::UniqueHandle parameter_unpack = Ast::construct(Ast::Tag::String, Ids::ParameterUnpack);
  parameter_unpack->set_string(boolalpha[rule_parameters_.get_unpack()]);
  ast_dest_production_.give_child_at_back(std::move(parameter_unpack));
 }
 if (rule_parameters_.get_hide() != pmt::pika::rt::RuleParametersBase::HIDE_DEFAULT) {
  Ast::UniqueHandle parameter_hide = Ast::construct(Ast::Tag::String, Ids::ParameterHide);
  parameter_hide->set_string(boolalpha[rule_parameters_.get_hide()]);
  ast_dest_production_.give_child_at_back(std::move(parameter_hide));
 }
}

auto add_definition(pmt::pika::rt::Ast& ast_dest_production_, pmt::pika::rt::Ast const& expr_) -> Ast* {
 Ast::UniqueHandle definition = Ast::construct(Ast::Tag::Parent, Ids::Definition);
 definition->give_child_at_back(Ast::clone(expr_));
 ast_dest_production_.give_child_at_back(std::move(definition));
 return ast_dest_production_.get_child_at_back();
}

}  // namespace

auto AstNodeHash::operator()(AstNodeKey const& key_) const -> size_t {
 std::vector<Ast const*> pending{key_._node};
 size_t seed = pmt::Hash::Phi64;

 while (!pending.empty()) {
  Ast const* cur = pending.back();
  pending.pop_back();

  pmt::Hash::combine(cur->get_id(), seed);

  switch (cur->get_tag()) {
   case Ast::Tag::String: {
    pmt::Hash::combine(cur->get_string(), seed);
   } break;
   case Ast::Tag::Parent: {
    for (size_t i = 0; i < cur->get_children_size(); ++i) {
     pending.push_back(cur->get_child_at(i));
    }
   } break;
  }
 }
 return seed;
}

auto AstNodeEq::operator()(AstNodeKey const& lhs_, AstNodeKey const& rhs_) const -> bool {
 std::vector<std::pair<Ast const*, Ast const*>> pending{{lhs_._node, rhs_._node}};

 while (!pending.empty()) {
  auto [left, right] = pending.back();
  pending.pop_back();

  if (left->get_id() != right->get_id() || left->get_tag() != right->get_tag()) {
   return false;
  }

  switch (left->get_tag()) {
   case Ast::Tag::String: {
    if (left->get_string() != right->get_string()) {
     return false;
    }
   } break;
   case Ast::Tag::Parent: {
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

auto add_rule(pmt::pika::rt::Ast& ast_dest_root_, std::string const& rule_name_, RuleParametersBase const& rule_parameters_, pmt::pika::rt::Ast const& expr_) -> Ast* {
 Ast::UniqueHandle production = Ast::construct(Ast::Tag::Parent, Ids::Production);
 add_identifier(*production, rule_name_);
 add_parameters(*production, rule_parameters_, rule_name_);
 Ast* definition = add_definition(*production, expr_);
 ast_dest_root_.give_child_at_back(std::move(production));
 return definition;
}

}  // namespace pmt::pika::meta
