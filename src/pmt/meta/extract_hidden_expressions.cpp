#include "pmt/meta/extract_hidden_expressions.hpp"

#include "pmt/ast/ast.hpp"
#include "pmt/hash.hpp"
#include "pmt/meta/ids.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <span>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace pmt::meta {
using namespace pmt::ast;
namespace {
struct AstPosition {
 Ast* _parent;
 size_t _child_idx;
};

struct GatheringFrame {
 AstPosition _position;
 bool _expanded;
};

struct AstPositionHash {
 auto operator()(AstPosition const& pos_) const -> size_t {
  std::vector<Ast const*> pending{pos_._parent->get_child_at(pos_._child_idx)};
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
};

struct AstPositionEq {
 auto operator()(AstPosition const& lhs_, AstPosition const& rhs_) const -> bool {
  std::vector<std::pair<Ast const*, Ast const*>> pending{{lhs_._parent->get_child_at(lhs_._child_idx), rhs_._parent->get_child_at(rhs_._child_idx)}};

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
};

auto is_production_hidden(Ast const& production_) -> bool {
 for (size_t i = 0; i < production_.get_children_size(); ++i) {
  Ast const* const child = production_.get_child_at(i);
  if (child->get_id() == Ids::ParameterHide && child->get_string() == "true") {
   return true;
  }
 }
 return false;
}

auto gather_hidden_expressions(Ast& ast_) -> std::vector<AstPosition> {
 std::vector<AstPosition> ret;

 std::vector<GatheringFrame> pending;
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  pending.push_back({AstPosition{&ast_, i}, false});
 }

 while (!pending.empty()) {
  auto const [pos, expanded] = pending.back();
  Ast* const cur = pos._parent->get_child_at(pos._child_idx);
  pending.pop_back();

  if (!expanded) {
   pending.push_back({pos, true});
   switch (cur->get_id()) {
    case Ids::Production: {
     if (!is_production_hidden(*cur)) {
      pending.push_back({AstPosition{cur, cur->get_children_size() - 1}, false});
     }
    } break;
    case pmt::ast::ReservedIds::IdRoot:
    case Ids::Definition:
    case Ids::Choices:
    case Ids::Sequence:
    case Ids::NegativeLookahead:
    case Ids::PositiveLookahead: {
     // push in reverse so child 0 is processed first
     for (size_t i = cur->get_children_size(); i-- > 0;) {
      pending.push_back({AstPosition{cur, i}, false});
     }
    } break;
    case Ids::Repetition: {
     pending.push_back({AstPosition{cur, 0}, false});
    } break;
    case Ids::Identifier:
    case Ids::StringLiteral:
    case Ids::GrammarProperty:
    case Ids::IntegerLiteral:
    case Ids::Charset:
    case Ids::Eof:
    case Ids::Epsilon:
    case Ids::Hidden:  // We only care about the topmost Hidden, so don't expand further
     break;
    default:
     pmt::unreachable();
   }
  } else if (cur->get_id() == Ids::Hidden) {
   ret.push_back(pos);
  }
 }

 return ret;
}

auto make_hidden_rule_name(size_t number_, size_t digits_needed_) -> std::string {
 return "__hidden_" + pmt::util::uint_to_string(number_, digits_needed_, pmt::util::hex_alphabet_uppercase);
}

void construct_hidden_production(pmt::ast::Ast& ast_root_, Ast const* expr_, std::string const& rule_name_) {
 Ast::UniqueHandle production = Ast::construct(Ast::Tag::Parent, Ids::Production);
 Ast::UniqueHandle identifier = Ast::construct(Ast::Tag::String, Ids::Identifier);
 identifier->set_string(rule_name_);
 Ast::UniqueHandle parameter_hide = Ast::construct(Ast::Tag::String, Ids::ParameterHide);
 parameter_hide->set_string("true");
 Ast::UniqueHandle definition = Ast::construct(Ast::Tag::Parent, Ids::Definition);
 definition->give_child_at_back(Ast::clone(*expr_->get_child_at_front()));

 production->give_child_at_back(std::move(identifier));
 production->give_child_at_back(std::move(parameter_hide));
 production->give_child_at_back(std::move(definition));

 ast_root_.give_child_at_back(std::move(production));
}

auto count_unique_hidden_expressions(std::span<AstPosition const> hidden_expressions_) -> size_t {
 std::unordered_set<AstPosition, AstPositionHash, AstPositionEq> unique_set;
 for (AstPosition const& pos : hidden_expressions_) {
  unique_set.insert(pos);
 }

 return unique_set.size();
}

}  // namespace

void extract_hidden_expressions(pmt::ast::Ast& ast_) {
 std::vector<AstPosition> hidden_expressions = gather_hidden_expressions(ast_);
 std::vector<Ast::UniqueHandle> hidden_expressions_orig;

 for (AstPosition const& pos : hidden_expressions) {
  hidden_expressions_orig.push_back(Ast::construct(Ast::Tag::Parent, Ids::Sequence));
  hidden_expressions_orig.back()->give_child_at_back(Ast::clone(*pos._parent->get_child_at(pos._child_idx)));
 }

 std::unordered_map<AstPosition, std::string, AstPositionHash, AstPositionEq> hidden_expression_to_rule_name;

 size_t const digits_needed = pmt::util::digits_needed(count_unique_hidden_expressions(hidden_expressions), 16);

 for (size_t i = 0; AstPosition const& pos : hidden_expressions) {
  AstPosition const pos_deep = {hidden_expressions_orig[i].get(), 0};

  if (!hidden_expression_to_rule_name.contains(pos_deep)) {
   size_t const unique_idx = hidden_expression_to_rule_name.size();
   std::string const rule_name = make_hidden_rule_name(unique_idx, digits_needed);
   hidden_expression_to_rule_name.emplace(pos_deep, rule_name);
   construct_hidden_production(ast_, pos._parent->get_child_at(pos._child_idx), rule_name);
  }

  auto const itr = hidden_expression_to_rule_name.find(pos_deep);
  Ast::UniqueHandle identifier = Ast::construct(Ast::Tag::String, Ids::Identifier);
  identifier->set_string(itr->second);
  Ast::swap(*pos._parent->get_child_at(pos._child_idx), *identifier);
  ++i;
 }
}

}  // namespace pmt::meta