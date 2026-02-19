#include "pmt/pika/meta/grammar_from_ast.hpp"

#include "pmt/pika/meta/ast_utils.hpp"
#include "pmt/pika/meta/charset_literal.hpp"
#include "pmt/pika/meta/ids.hpp"
#include "pmt/pika/meta/number.hpp"
#include "pmt/pika/meta/repetition_range.hpp"
#include "pmt/pika/meta/rule.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/closest_strings.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <cassert>
#include <set>

namespace pmt::pika::meta {
using namespace pmt::container;
using namespace pmt::pika::rt;

namespace {
enum : size_t {
 MaxLevDistanceToReport = 3,
 MaxNotFoundNamesToReport = 3,
 MaxDuplicateNamesToReport = 3,
};

char const base_36_alphabet_lowercase[] = "0123456789abcdefghijklmnopqrstuvwxyz";

class FrameBase {
public:
 Ast* _cur_expr;
 size_t _stage = 0;
};

class PassthroughFrame : public FrameBase {
public:
};

class SequenceFrame : public FrameBase {
public:
 RuleExpression::UniqueHandle _sub_part;
 size_t _idx = 0;
};

class ChoicesFrame : public FrameBase {
public:
 RuleExpression::UniqueHandle _sub_part;
 size_t _idx = 0;
};

class RepetitionFrame : public FrameBase {
public:
 RuleExpression::UniqueHandle _sub_part;
};

class StringLiteralFrame : public FrameBase {};

class IntegerLiteralFrame : public FrameBase {};

class CharsetFrame : public FrameBase {};

class IdentifierFrame : public FrameBase {};

class NegativeLookaheadFrame : public FrameBase {};

class PositiveLookaheadFrame : public FrameBase {};

class HiddenFrame : public FrameBase {};

class EpsilonFrame : public FrameBase {};

class EofFrame : public FrameBase {};

class AnyFrame : public FrameBase {};

using Frame = std::variant<PassthroughFrame, SequenceFrame, ChoicesFrame, RepetitionFrame, StringLiteralFrame, IntegerLiteralFrame, CharsetFrame, IdentifierFrame, NegativeLookaheadFrame, PositiveLookaheadFrame, HiddenFrame, EpsilonFrame, EofFrame, AnyFrame>;

auto fetch_ast(Ast::UniqueHandle const& key_) -> Ast const& {
 return *key_;
}

auto fetch_ast(Ast const& key_) -> Ast const& {
 return key_;
}

struct AstHash {
 using is_transparent = void;  // NOLINT
 auto operator()(auto const& key_) const -> size_t {
  std::vector<Ast const*> pending{&fetch_ast(key_)};
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

struct AstEq {
 using is_transparent = void;  // NOLINT
 auto operator()(auto const& lhs_, auto const& rhs_) const -> bool {
  std::vector<std::pair<Ast const*, Ast const*>> pending{{&fetch_ast(lhs_), &fetch_ast(rhs_)}};

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

struct PlusRuleNames {
 std::string _body_rule_name;
 std::string _plus_rule_name;
};

class Locals {
public:
 std::vector<std::string> _pending_rules;
 std::unordered_map<std::string, std::pair<RuleParameters, Ast*>> _rules;  // rule name -> <parameters, definition ast>
 std::unordered_map<Ast::UniqueHandle, std::string, AstHash, AstEq> _hidden_expression_to_rule_name;
 std::unordered_map<Ast::UniqueHandle, PlusRuleNames, AstHash, AstEq> _plus_expression_to_rule_names;
 std::set<std::string> _duplicate_rules;
 std::unordered_map<std::string, IdType> _id_string_to_id_map;  // Only non-reserved ids
 Grammar _ret;
 RuleExpression::UniqueHandle _ret_part;
 std::deque<Frame> _callstack;
 size_t _constructed_hidden_rules = 0;
 size_t _constructed_plus_rules = 0;
 Ast& _ast;
 bool _keep_current_frame : 1;
 bool _in_hidden : 1;
};

auto make_str_hidden_rule_name(size_t number_) -> std::string {
 return "__hidden_" + pmt::util::uint_to_string(number_, 0, base_36_alphabet_lowercase);
}

auto make_str_plus_rule_name(size_t number_) -> std::string {
 return "__plus_" + pmt::util::uint_to_string(number_, 0, base_36_alphabet_lowercase);
}

auto make_str_plus_body_rule_name(size_t number_) -> std::string {
 return "__plus_body_" + pmt::util::uint_to_string(number_, 0, base_36_alphabet_lowercase);
}

auto make_re_any() -> RuleExpression::UniqueHandle {
 RuleExpression::UniqueHandle ret = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 ret->get_charset_literal().push_back(CharsetLiteral::IntervalType(std::numeric_limits<SymbolType>::min(), std::numeric_limits<SymbolType>::max()));
 return ret;
}

auto make_re_eof() -> RuleExpression::UniqueHandle {
 RuleExpression::UniqueHandle ret = RuleExpression::construct(ClauseBase::Tag::NegativeLookahead);
 ret->give_child_at_back(make_re_any());
 return ret;
}

auto make_ast_identifier(std::string const& rule_name_) -> Ast::UniqueHandle {
 Ast::UniqueHandle ret = Ast::construct(Ast::Tag::String, Ids::Identifier);
 ret->set_string(rule_name_);
 return ret;
}

auto make_ast_definition(Ast const& expr_) -> Ast::UniqueHandle {
 Ast::UniqueHandle definition = Ast::construct(Ast::Tag::Parent, Ids::Definition);
 definition->give_child_at_back(Ast::clone(expr_));
 return definition;
}

void add_ast_parameters(Ast& ast_dest_production_, RuleParametersBase const& rule_parameters_, std::string const& rule_name_) {
 assert(ast_dest_production_.get_id() == Ids::Production);
 std::string const boolalpha[2] = {"false", "true"};

 if (rule_parameters_.get_display_name() != rule_name_) {
  Ast::UniqueHandle parameter_display_name = Ast::construct(Ast::Tag::String, Ids::ParameterDisplayName);
  parameter_display_name->set_string(std::string(rule_parameters_.get_display_name()));
  ast_dest_production_.give_child_at_back(std::move(parameter_display_name));
 }
 if (rule_parameters_.get_merge() != RuleParametersBase::MERGE_DEFAULT) {
  Ast::UniqueHandle parameter_merge = Ast::construct(Ast::Tag::String, Ids::ParameterMerge);
  parameter_merge->set_string(boolalpha[rule_parameters_.get_merge()]);
  ast_dest_production_.give_child_at_back(std::move(parameter_merge));
 }
 if (rule_parameters_.get_unpack() != RuleParametersBase::UNPACK_DEFAULT) {
  Ast::UniqueHandle parameter_unpack = Ast::construct(Ast::Tag::String, Ids::ParameterUnpack);
  parameter_unpack->set_string(boolalpha[rule_parameters_.get_unpack()]);
  ast_dest_production_.give_child_at_back(std::move(parameter_unpack));
 }
 if (rule_parameters_.get_hide() != RuleParametersBase::HIDE_DEFAULT) {
  Ast::UniqueHandle parameter_hide = Ast::construct(Ast::Tag::String, Ids::ParameterHide);
  parameter_hide->set_string(boolalpha[rule_parameters_.get_hide()]);
  ast_dest_production_.give_child_at_back(std::move(parameter_hide));
 }
}

void construct_rule(Locals& locals_, std::string const& rule_name_, RuleParameters const& rule_parameters_, Ast const& expr_) {
 Ast::UniqueHandle production = Ast::construct(Ast::Tag::Parent, Ids::Production);
 production->give_child_at_back(make_ast_identifier(rule_name_));
 add_ast_parameters(*production, rule_parameters_, rule_name_);
 locals_._ast.give_child_at_back(make_ast_definition(expr_));

 locals_._rules[rule_name_] = std::make_pair(rule_parameters_, locals_._ast.get_child_at_back());
 locals_._pending_rules.push_back(rule_name_);
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

auto make_ast_sequence_with_body_clones(Ast const& body_, size_t count_) -> Ast::UniqueHandle {
 Ast::UniqueHandle sequence = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 append_body_clones(*sequence, body_, count_);
 return sequence;
}

auto make_ast_optional_tail_choices(Ast const& body_, size_t count_) -> Ast::UniqueHandle {
 Ast::UniqueHandle choices = Ast::construct(Ast::Tag::Parent, Ids::Choices);
 for (size_t i = count_; i != 0; --i) {
  choices->give_child_at_back(make_ast_sequence_with_body_clones(body_, i));
 }
 Ast::UniqueHandle seq_epsilon = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 seq_epsilon->give_child_at_back(make_epsilon());
 choices->give_child_at_back(std::move(seq_epsilon));
 return choices;
}

void construct_plus_rule(Locals& locals_, std::string const& body_rule_name_, std::string const& plus_rule_name_) {
 Ast::UniqueHandle definition = Ast::construct(Ast::Tag::Parent, Ids::Definition);
 Ast::UniqueHandle choices = Ast::construct(Ast::Tag::Parent, Ids::Choices);
 Ast::UniqueHandle seq_recursive = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 seq_recursive->give_child_at_back(make_ast_identifier(body_rule_name_));
 seq_recursive->give_child_at_back(make_ast_identifier(plus_rule_name_));
 choices->give_child_at_back(std::move(seq_recursive));

 Ast::UniqueHandle seq_base = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
 seq_base->give_child_at_back(make_ast_identifier(body_rule_name_));
 choices->give_child_at_back(std::move(seq_base));

 definition->give_child_at_back(std::move(choices));

 RuleParameters rule_parameters;
 rule_parameters._unpack = true;
 rule_parameters._display_name = plus_rule_name_;
 rule_parameters._id_string = ReservedIds::id_to_string(ReservedIds::IdDefault);

 construct_rule(locals_, plus_rule_name_, rule_parameters, *definition);
}

auto make_ast_expanded_unbounded_repetition(size_t lower_, PlusRuleNames const& rule_names_) -> Ast::UniqueHandle {
 Ast::UniqueHandle plus_identifier = make_ast_identifier(rule_names_._plus_rule_name);

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
  sequence->give_child_at_back(make_ast_identifier(rule_names_._body_rule_name));
 }
 sequence->give_child_at_back(std::move(plus_identifier));
 return sequence;
}

auto make_ast_expanded_bounded_repetition(Ast const& body_, RepetitionRange const& range_) -> Ast::UniqueHandle {
 size_t const lower = static_cast<size_t>(range_.get_lower());
 size_t const upper = static_cast<size_t>(*range_.get_upper());

 if (lower != 0) {
  Ast::UniqueHandle sequence = Ast::construct(Ast::Tag::Parent, Ids::Sequence);
  append_body_clones(*sequence, body_, lower);
  if (lower != upper) {
   sequence->give_child_at_back(make_ast_optional_tail_choices(body_, upper - lower));
  }
  return sequence;
 }

 return make_ast_optional_tail_choices(body_, upper);
}

void caching_traversal_handle_production(Locals& locals_, Ast& ast_) {
 std::string rule_name = ast_.get_child_at(0)->get_string();

 RuleParameters rp;
 rp._id_string = ReservedIds::id_to_string(ReservedIds::IdDefault);
 rp._display_name = rule_name;
 Ast* rule_definition = nullptr;

 for (size_t i = 1; i < ast_.get_children_size(); ++i) {
  Ast& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Ids::ParameterId: {
    rp._id_string = child.get_string();
   } break;
   case Ids::ParameterDisplayName: {
    rp._display_name = child.get_string();
   } break;
   case Ids::ParameterMerge: {
    rp._merge = child.get_string() == "true";
   } break;
   case Ids::ParameterUnpack: {
    rp._unpack = child.get_string() == "true";
   } break;
   case Ids::ParameterHide: {
    rp._hide = child.get_string() == "true";
   } break;
   case Ids::Definition: {
    rule_definition = child.get_child_at(0);
   } break;
  }
 }

 if (locals_._rules.contains(rule_name)) {
  locals_._duplicate_rules.insert(rule_name);
 }

 locals_._rules[rule_name] = std::make_pair(std::move(rp), rule_definition);
}

void caching_traversal(Locals& locals_) {
 for (size_t i = 0; i < locals_._ast.get_children_size(); ++i) {
  Ast const& child = *locals_._ast.get_child_at(i);
  switch (child.get_id()) {
   case Ids::Production:
    caching_traversal_handle_production(locals_, *locals_._ast.get_child_at(i));
    break;
  }
 }
}

[[noreturn]] void throw_rule_not_found_exception(pmt::util::ClosestStrings const& closest_) {
 std::string error_msg = "Rule not found: \"$" + closest_.query() + "\"";

 if (!closest_.candidates().empty()) {
  std::string delim;
  error_msg += ", did you mean: ";
  for (std::string const& rule_name : closest_.candidates()) {
   error_msg += std::exchange(delim, " OR ") + "\"$" + rule_name + "\"";
  }
  if (closest_.truncated()) {
   error_msg += ", ...";
  }
 }

 throw std::runtime_error(error_msg);
}

void report_duplicate_rules(std::set<std::string> const& duplicate_rules_) {
 if (duplicate_rules_.empty()) {
  return;
 }

 std::string msg = "Duplicate rule name";
 msg += (duplicate_rules_.size() > 1 ? "s: " : ": ");

 std::string delim;
 for (size_t i = 0; std::string const& rule_name : duplicate_rules_) {
  if (i++ == MaxDuplicateNamesToReport) {
   msg += ", ...";
   break;
  }
  msg += std::exchange(delim, ", ") + "\"$" + rule_name + "\"";
 }

 throw std::runtime_error(msg);
}

auto construct_frame(Ast* ast_cur_) -> Frame {
 FrameBase frame_base{._cur_expr = ast_cur_, ._stage = 0};

 switch (ast_cur_->get_id()) {
  case Ids::Definition: {
   return PassthroughFrame{frame_base};
  } break;
  case Ids::Choices: {
   return ChoicesFrame{frame_base};
  } break;
  case Ids::Repetition: {
   return RepetitionFrame{frame_base};
  } break;
  case Ids::Sequence: {
   return SequenceFrame{frame_base};
  } break;
  case Ids::Identifier: {
   return IdentifierFrame{frame_base};
  } break;
  case Ids::StringLiteral: {
   return StringLiteralFrame{frame_base};
  } break;
  case Ids::IntegerLiteral: {
   return IntegerLiteralFrame{frame_base};
  } break;
  case Ids::Charset: {
   return CharsetFrame{frame_base};
  } break;
  case Ids::NegativeLookahead: {
   return NegativeLookaheadFrame{frame_base};
  } break;
  case Ids::PositiveLookahead: {
   return PositiveLookaheadFrame{frame_base};
  } break;
  case Ids::Hidden: {
   return HiddenFrame{frame_base};
  } break;
  case Ids::Epsilon: {
   return EpsilonFrame{frame_base};
  } break;
  case Ids::Eof: {
   return EofFrame{frame_base};
  } break;
  case Ids::Any: {
   return AnyFrame{frame_base};
  } break;
  default:
   pmt::unreachable();
 }
}

void process_frame_00(Locals& locals_, PassthroughFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at_front()));
}

void process_frame_00(Locals& locals_, SequenceFrame& frame_) {
 ++frame_._stage;
 frame_._sub_part = RuleExpression::construct(ClauseBase::Tag::Sequence);
 locals_._keep_current_frame = true;
}

void process_frame_01(Locals& locals_, SequenceFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at(frame_._idx)));
}

void process_frame_02(Locals& locals_, SequenceFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 frame_._sub_part->give_child_at_back(std::move(locals_._ret_part));

 // If is last
 if (frame_._idx == frame_._cur_expr->get_children_size()) {
  locals_._ret_part = std::move(frame_._sub_part);
 } else {
  locals_._keep_current_frame = true;
 }
}

void process_frame_00(Locals& locals_, ChoicesFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at(frame_._idx)));
}

void process_frame_01(Locals& locals_, ChoicesFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 if (frame_._idx == 1) {
  frame_._sub_part = RuleExpression::construct(ClauseBase::Tag::Choice);
 }

 frame_._sub_part->give_child_at_back(std::move(locals_._ret_part));

 if (frame_._idx < frame_._cur_expr->get_children_size()) {
  locals_._keep_current_frame = true;
  return;
 }

 locals_._ret_part = std::move(frame_._sub_part);
}

void process_frame_00(Locals& locals_, RepetitionFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at_front()));
}

void process_frame_01(Locals& locals_, RepetitionFrame& frame_) {
 RepetitionRange const range(*frame_._cur_expr->get_child_at(1));
 Ast const& body = *frame_._cur_expr->get_child_at_front();

 Ast::UniqueHandle replacement;
 if (!range.get_upper().has_value()) {
  auto itr = locals_._plus_expression_to_rule_names.find(body);
  if (itr == locals_._plus_expression_to_rule_names.end()) {
   size_t const plus_rule_number = locals_._constructed_plus_rules++;
   PlusRuleNames names{make_str_plus_body_rule_name(plus_rule_number), make_str_plus_rule_name(plus_rule_number)};

   RuleParameters rp_body;
   rp_body._unpack = true;
   rp_body._display_name = names._body_rule_name;
   rp_body._id_string = ReservedIds::id_to_string(ReservedIds::IdDefault);
   construct_rule(locals_, names._body_rule_name, rp_body, body);
   construct_plus_rule(locals_, names._body_rule_name, names._plus_rule_name);
   itr = locals_._plus_expression_to_rule_names.emplace(Ast::clone(body), std::move(names)).first;
  }
  replacement = make_ast_expanded_unbounded_repetition(static_cast<size_t>(range.get_lower()), itr->second);
 } else {
  replacement = make_ast_expanded_bounded_repetition(body, range);
 }

 Ast::swap(*frame_._cur_expr, *replacement);
 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr));
}

void process_frame_00(Locals& locals_, StringLiteralFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(std::move(CharsetLiteral(*frame_._cur_expr)));
}

void process_frame_00(Locals& locals_, IntegerLiteralFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(CharsetLiteral(*frame_._cur_expr));
}

void process_frame_00(Locals& locals_, CharsetFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(CharsetLiteral(*frame_._cur_expr));
}

void process_frame_00(Locals& locals_, IdentifierFrame& frame_) {
 std::string const& identifier = frame_._cur_expr->get_string();

 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Identifier);
 locals_._ret_part->set_identifier(identifier);

 // Check against rule names to see if it exists
 if (!locals_._rules.contains(identifier)) {
  pmt::util::ClosestStrings closest(identifier, MaxLevDistanceToReport, MaxNotFoundNamesToReport);
  for (auto const& [rule_name, _] : locals_._rules) {
   closest.push(rule_name);
  }
  throw_rule_not_found_exception(closest);
 }
}

void process_frame_00(Locals& locals_, NegativeLookaheadFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at(0)));
}

void process_frame_01(Locals& locals_, NegativeLookaheadFrame& frame_) {
 RuleExpression::UniqueHandle negative_lookahead = RuleExpression::construct(ClauseBase::Tag::NegativeLookahead);
 negative_lookahead->give_child_at_back(std::move(locals_._ret_part));
 locals_._ret_part = std::move(negative_lookahead);
}

void process_frame_00(Locals& locals_, PositiveLookaheadFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at(0)));
}

void process_frame_01(Locals& locals_, PositiveLookaheadFrame& frame_) {
 RuleExpression::UniqueHandle outer = RuleExpression::construct(ClauseBase::Tag::NegativeLookahead);
 outer->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::NegativeLookahead));
 outer->get_child_at_back()->give_child_at_back(std::move(locals_._ret_part));
 locals_._ret_part = std::move(outer);
}

void process_frame_00(Locals& locals_, HiddenFrame& frame_) {
 if (locals_._in_hidden) {
  locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at_front()));
  return;
 }

 Ast& hidden_expr{*frame_._cur_expr->get_child_at_front()};
 if (!locals_._hidden_expression_to_rule_name.contains(hidden_expr)) {
  size_t const unique_idx = locals_._hidden_expression_to_rule_name.size();
  std::string const rule_name = make_str_hidden_rule_name(unique_idx);
  locals_._hidden_expression_to_rule_name.emplace(Ast::clone(hidden_expr), rule_name);
  RuleParameters params;
  params._hide = true;
  params._display_name = rule_name;
  params._id_string = ReservedIds::id_to_string(ReservedIds::IdDefault);
  construct_rule(locals_, rule_name, params, hidden_expr);
 }

 auto const itr = locals_._hidden_expression_to_rule_name.find(hidden_expr);
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Identifier);
 locals_._ret_part->set_identifier(itr->second);
}

void process_frame_00(Locals& locals_, EpsilonFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Epsilon);
}

void process_frame_00(Locals& locals_, EofFrame& frame_) {
 locals_._ret_part = make_re_eof();
}

void process_frame_00(Locals& locals_, AnyFrame& frame_) {
 locals_._ret_part = make_re_any();
}

void dispatch(Locals& locals_, Frame& frame_) {
 switch (std::visit([](auto const& f_) { return f_._stage; }, frame_)) {
  case 0:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_00(locals_, f_); }) {
      process_frame_00(locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  case 1:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_01(locals_, f_); }) {
      process_frame_01(locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  case 2:
   std::visit(
    [&](auto& f_) {
     if constexpr (requires { process_frame_02(locals_, f_); }) {
      process_frame_02(locals_, f_);
     } else {
      pmt::unreachable();
     }
    },
    frame_);
   return;
  default:
   pmt::unreachable();
 }
}

auto construct_definition(Locals& locals_, Ast* ast_, bool hidden_) -> RuleExpression::UniqueHandle {
 locals_._in_hidden = hidden_;
 locals_._callstack.emplace_back(construct_frame(ast_));

 while (!locals_._callstack.empty()) {
  locals_._keep_current_frame = false;
  size_t const frame_idx = locals_._callstack.size() - 1;
  dispatch(locals_, locals_._callstack.back());
  if (!locals_._keep_current_frame) {
   locals_._callstack.erase(locals_._callstack.begin() + frame_idx);
  }
 }

 return std::move(locals_._ret_part);
}

void construct_definitions(Locals& locals_) {
 locals_._pending_rules.clear();

 for (auto& [rule_name, rule_definition] : locals_._rules) {
  locals_._pending_rules.push_back(rule_name);
 }

 while (!locals_._pending_rules.empty()) {
  std::string rule_name = locals_._pending_rules.back();
  locals_._pending_rules.pop_back();
  auto const itr = locals_._rules.find(rule_name);
  auto& [params, definition] = itr->second;
  locals_._ret.insert_or_assign_rule(rule_name, Rule{
                                                 ._parameters = params,
                                                 ._definition = construct_definition(locals_, definition, params.get_hide()),
                                                });
 }
}

}  // namespace

auto grammar_from_ast(Ast::UniqueHandle ast_) -> Grammar {
 Locals locals{._ast = *ast_};
 caching_traversal(locals);
 report_duplicate_rules(locals._duplicate_rules);
 construct_definitions(locals);
 return std::move(locals._ret);
}

}  // namespace pmt::pika::meta