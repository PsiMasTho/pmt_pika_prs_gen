#include "pmt/meta/extract_repetition_expressions.hpp"

#include "pmt/meta/ast_utils.hpp"
#include "pmt/meta/ids.hpp"
#include "pmt/meta/repetition_range.hpp"
#include "pmt/meta/rule_parameters.hpp"
#include "pmt/rt/ast.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace pmt::meta {
using namespace pmt::rt;
namespace {
auto gather_repetition_expressions(Ast& ast_) -> std::vector<AstPosition> {
 std::vector<AstPosition> ret;
 std::vector<GatheringFrame> pending;
 pending.reserve(ast_.get_children_size());

 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  pending.push_back({AstPosition{&ast_, i}, false});
 }

 while (!pending.empty()) {
  auto const [pos, expanded] = pending.back();
  Ast* const cur = pos._parent->get_child_at(pos._child_idx);
  pending.pop_back();

  if (!expanded) {
   pending.push_back({pos, true});
   if (cur->get_tag() == Ast::Tag::Parent) {
    for (size_t i = cur->get_children_size(); i-- > 0;) {
     pending.push_back({AstPosition{cur, i}, false});
    }
   }
   continue;
  }

  if (cur->get_id() == Ids::Repetition) {
   ret.push_back(pos);
  }
 }

 return ret;
}

auto make_plus_rule_name(size_t number_, size_t digits_needed_) -> std::string {
 return "__plus_" + pmt::util::uint_to_string(number_, digits_needed_, pmt::util::hex_alphabet_uppercase);
}

auto make_plus_body_rule_name(size_t number_, size_t digits_needed_) -> std::string {
 return "__plus_body_" + pmt::util::uint_to_string(number_, digits_needed_, pmt::util::hex_alphabet_uppercase);
}

struct PlusRuleNames {
 std::string _body_rule_name;
 std::string _plus_rule_name;
};

auto make_identifier(std::string const& rule_name_) -> Ast::UniqueHandle {
 Ast::UniqueHandle identifier = Ast::construct(Ast::Tag::String, Ids::Identifier);
 identifier->set_string(rule_name_);
 return identifier;
}

auto make_epsilon() -> Ast::UniqueHandle {
 Ast::UniqueHandle epsilon = Ast::construct(Ast::Tag::String, Ids::Epsilon);
 epsilon->set_string("epsilon");
 return epsilon;
}

void append_body_clones(Ast& sequence_, Ast const& body_, size_t count_) {
 for (size_t i = 0; i < count_; ++i) {
  sequence_.give_child_at_back(Ast::clone(body_));
 }
}

auto make_sequence_with_body_clones(Ast const& body_, size_t count_) -> Ast::UniqueHandle {
 Ast::UniqueHandle sequence = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 append_body_clones(*sequence, body_, count_);
 return sequence;
}

auto make_optional_tail_choices(Ast const& body_, size_t count_) -> Ast::UniqueHandle {
 Ast::UniqueHandle choices = Ast::construct(Ast::Tag::Parent, Ids::Choices);
 for (size_t i = count_; i != 0; --i) {
  choices->give_child_at_back(make_sequence_with_body_clones(body_, i));
 }
 Ast::UniqueHandle seq_epsilon = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 seq_epsilon->give_child_at_back(make_epsilon());
 choices->give_child_at_back(std::move(seq_epsilon));
 return choices;
}

void add_plus_rule(Ast& ast_dest_root_, std::string const& body_rule_name_, std::string const& plus_rule_name_) {
 Ast::UniqueHandle definition = Ast::construct(Ast::Tag::Parent, Ids::Definition);
 Ast::UniqueHandle choices = Ast::construct(Ast::Tag::Parent, Ids::Choices);
 Ast::UniqueHandle seq_recursive = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 seq_recursive->give_child_at_back(make_identifier(body_rule_name_));
 seq_recursive->give_child_at_back(make_identifier(plus_rule_name_));
 choices->give_child_at_back(std::move(seq_recursive));

 Ast::UniqueHandle seq_base = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 seq_base->give_child_at_back(make_identifier(body_rule_name_));
 choices->give_child_at_back(std::move(seq_base));

 definition->give_child_at_back(std::move(choices));

 RuleParameters rule_parameters;
 rule_parameters._unpack = true;
 rule_parameters._display_name = plus_rule_name_;

 add_rule(ast_dest_root_, plus_rule_name_, rule_parameters, *definition);
}

auto expand_unbounded_repetition(size_t lower_, PlusRuleNames const& rule_names_) -> Ast::UniqueHandle {
 Ast::UniqueHandle plus_identifier = make_identifier(rule_names_._plus_rule_name);

 if (lower_ == 0) {
  Ast::UniqueHandle choices = Ast::construct(Ast::Tag::Parent, Ids::Choices);
  Ast::UniqueHandle seq_plus = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
  seq_plus->give_child_at_back(std::move(plus_identifier));
  choices->give_child_at_back(std::move(seq_plus));

  Ast::UniqueHandle seq_epsilon = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
  seq_epsilon->give_child_at_back(make_epsilon());
  choices->give_child_at_back(std::move(seq_epsilon));
  return choices;
 }

 Ast::UniqueHandle sequence = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 for (size_t i = 0; i < lower_ - 1; ++i) {
  sequence->give_child_at_back(make_identifier(rule_names_._body_rule_name));
 }
 sequence->give_child_at_back(std::move(plus_identifier));
 return sequence;
}

auto expand_bounded_repetition(Ast const& body_, RepetitionRange const& range_) -> Ast::UniqueHandle {
 size_t const lower = static_cast<size_t>(range_.get_lower());
 size_t const upper = static_cast<size_t>(*range_.get_upper());

 if (lower != 0) {
  Ast::UniqueHandle sequence = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
  append_body_clones(*sequence, body_, lower);
  if (lower != upper) {
   sequence->give_child_at_back(make_optional_tail_choices(body_, upper - lower));
  }
  return sequence;
 }

 return make_optional_tail_choices(body_, upper);
}

}  // namespace

void extract_repetition_expressions(pmt::rt::Ast& ast_) {
 std::vector<AstPosition> repetitions = gather_repetition_expressions(ast_);

 std::vector<Ast::UniqueHandle> repetition_bodies_orig;
 repetition_bodies_orig.reserve(repetitions.size());
 std::vector<AstNodeKey> repetition_body_keys;
 repetition_body_keys.reserve(repetitions.size());
 std::vector<bool> repetition_unbounded;
 repetition_unbounded.reserve(repetitions.size());

 for (AstPosition const& pos : repetitions) {
  Ast const* const repetition = pos._parent->get_child_at(pos._child_idx);
  repetition_bodies_orig.push_back(Ast::clone(*repetition->get_child_at(0)));
  repetition_body_keys.push_back(AstNodeKey{repetition_bodies_orig.back().get()});
  RepetitionRange range(*repetition->get_child_at(1));
  repetition_unbounded.push_back(!range.get_upper().has_value());
 }

 std::unordered_set<AstNodeKey, AstNodeHash, AstNodeEq> unique_plus_bodies;
 for (size_t i = 0; i < repetition_body_keys.size(); ++i) {
  if (repetition_unbounded[i]) {
   unique_plus_bodies.insert(repetition_body_keys[i]);
  }
 }

 size_t const digits_needed = pmt::util::digits_needed(unique_plus_bodies.size(), 16);
 size_t plus_rule_idx = 0;
 std::unordered_map<AstNodeKey, PlusRuleNames, AstNodeHash, AstNodeEq> plus_rules_by_body;

 for (size_t i = 0; i < repetitions.size(); ++i) {
  AstPosition const& pos = repetitions[i];
  Ast* const repetition = pos._parent->get_child_at(pos._child_idx);
  RepetitionRange const range(*repetition->get_child_at(1));
  Ast const& body = *repetition->get_child_at(0);

  Ast::UniqueHandle replacement;
  if (!range.get_upper().has_value()) {
   AstNodeKey const key = repetition_body_keys[i];
   auto itr = plus_rules_by_body.find(key);
   if (itr == plus_rules_by_body.end()) {
    size_t const plus_rule_number = plus_rule_idx++;
    PlusRuleNames names{make_plus_body_rule_name(plus_rule_number, digits_needed), make_plus_rule_name(plus_rule_number, digits_needed)};

    RuleParameters rp_body;
    rp_body._unpack = true;
    rp_body._display_name = names._body_rule_name;
    add_rule(ast_, names._body_rule_name, rp_body, body);
    add_plus_rule(ast_, names._body_rule_name, names._plus_rule_name);
    itr = plus_rules_by_body.emplace(key, std::move(names)).first;
   }
   replacement = expand_unbounded_repetition(static_cast<size_t>(range.get_lower()), itr->second);
  } else {
   replacement = expand_bounded_repetition(body, range);
  }

  Ast::swap(*repetition, *replacement);
 }
}

}  // namespace pmt::meta
