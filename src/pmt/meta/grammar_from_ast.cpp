#include "pmt/meta/grammar_from_ast.hpp"

#include "pmt/meta/charset_literal.hpp"
#include "pmt/meta/ids.hpp"
#include "pmt/meta/number.hpp"
#include "pmt/meta/repetition_range.hpp"
#include "pmt/meta/rule.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/closest_strings.hpp"

#include <map>
#include <set>

namespace pmt::meta {
using namespace pmt::container;
using namespace pmt::rt;

enum : size_t {
 MaxLevDistanceToReport = 3,
 MaxNotFoundNamesToReport = 3,
 MaxDuplicateNamesToReport = 3,
};

namespace {
class FrameBase {
public:
 Ast const* _cur_expr;
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

class StringLiteralFrame : public FrameBase {};

class IntegerLiteralFrame : public FrameBase {};

class CharsetFrame : public FrameBase {};

class IdentifierFrame : public FrameBase {};

class NegativeLookaheadFrame : public FrameBase {};

class PositiveLookaheadFrame : public FrameBase {};

class EpsilonFrame : public FrameBase {};

class EofFrame : public FrameBase {};

using Frame = std::variant<PassthroughFrame, SequenceFrame, ChoicesFrame, StringLiteralFrame, IntegerLiteralFrame, CharsetFrame, IdentifierFrame, NegativeLookaheadFrame, PositiveLookaheadFrame, EpsilonFrame, EofFrame>;

class Locals {
public:
 std::unordered_map<std::string, std::pair<RuleParameters, Ast const*>> _rules;  // rule name -> <parameters, definition ast>
 std::set<std::string> _duplicate_rules;
 std::unordered_map<std::string, pmt::rt::IdType> _id_string_to_id_map;  // Only non-generic ids
 Grammar _ret;
 RuleExpression::UniqueHandle _ret_part;
 std::deque<Frame> _callstack;
 size_t _generated_plus_rules = 0;
 bool _keep_current_frame;
};

void caching_traversal_handle_production(Locals& locals_, Ast const& ast_) {
 std::string rule_name = ast_.get_child_at(0)->get_string();
 std::string rule_display_name = rule_name;

 std::string rule_id_string = pmt::rt::ReservedIds::id_to_string(pmt::rt::ReservedIds::IdDefault);
 bool rule_merge = RuleParametersBase::MERGE_DEFAULT;
 bool rule_unpack = RuleParametersBase::UNPACK_DEFAULT;
 bool rule_hide = RuleParametersBase::HIDE_DEFAULT;

 Ast const* rule_definition = nullptr;

 for (size_t i = 1; i < ast_.get_children_size(); ++i) {
  Ast const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Ids::ParameterId: {
    rule_id_string = child.get_string();
   } break;
   case Ids::ParameterDisplayName: {
    rule_display_name = child.get_string();
   } break;
   case Ids::ParameterMerge: {
    rule_merge = child.get_string() == "true";
   } break;
   case Ids::ParameterUnpack: {
    rule_unpack = child.get_string() == "true";
   } break;
   case Ids::ParameterHide: {
    rule_hide = child.get_string() == "true";
   } break;
   case Ids::Definition: {
    rule_definition = child.get_child_at(0);
   } break;
  }
 }

 RuleParameters rule_parameters;
 rule_parameters._display_name = rule_display_name;
 rule_parameters._id_string = rule_id_string;
 rule_parameters._merge = rule_merge;
 rule_parameters._unpack = rule_unpack;
 rule_parameters._hide = rule_hide;

 if (locals_._rules.contains(rule_name)) {
  locals_._duplicate_rules.insert(rule_name);
 }

 locals_._rules[rule_name] = std::make_pair(std::move(rule_parameters), rule_definition);
}

void caching_traversal(Locals& locals_, Ast const& ast_) {
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  Ast const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Ids::Production:
    caching_traversal_handle_production(locals_, *ast_.get_child_at(i));
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

auto construct_frame(Ast const* ast_cur_) -> Frame {
 FrameBase frame_base{._cur_expr = ast_cur_, ._stage = 0};

 switch (ast_cur_->get_id()) {
  case Ids::Definition:
  case Ids::Hidden: {
   return PassthroughFrame{frame_base};
  } break;
  case Ids::Choices: {
   return ChoicesFrame{frame_base};
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
  case Ids::Epsilon: {
   return EpsilonFrame{frame_base};
  } break;
  case Ids::Eof: {
   return EofFrame{frame_base};
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

void process_frame_00(Locals& locals_, EpsilonFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Epsilon);
}

void process_frame_00(Locals& locals_, EofFrame& frame_) {
 RuleExpression::UniqueHandle outer = RuleExpression::construct(ClauseBase::Tag::NegativeLookahead);
 outer->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::CharsetLiteral));
 outer->get_child_at_back()->get_charset_literal().push_back(CharsetLiteral::IntervalType(0, std::numeric_limits<SymbolType>::max()));
 locals_._ret_part = std::move(outer);
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

auto construct_definition(Locals& locals_, Ast const* ast_) -> RuleExpression::UniqueHandle {
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
 for (auto& [rule_name, rule_definition] : locals_._rules) {
  locals_._ret.insert_or_assign_rule(rule_name, Rule{
                                                 ._parameters = std::move(rule_definition.first),
                                                 ._definition = construct_definition(locals_, rule_definition.second),
                                                });
 }
}

}  // namespace

auto grammar_from_ast(Ast::UniqueHandle const& ast_) -> Grammar {
 Locals locals;
 caching_traversal(locals, *ast_);
 report_duplicate_rules(locals._duplicate_rules);
 construct_definitions(locals);
 return std::move(locals._ret);
}

}  // namespace pmt::meta