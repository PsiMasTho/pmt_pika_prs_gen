#include "pmt/meta/grammar_from_ast.hpp"

#include "pmt/meta/charset.hpp"
#include "pmt/meta/charset_literal.hpp"
#include "pmt/meta/ids.hpp"
#include "pmt/meta/number.hpp"
#include "pmt/meta/repetition_range.hpp"
#include "pmt/meta/rule.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/levenshtein.hpp"

#include <map>
#include <set>

namespace pmt::meta {
using namespace pmt::container;
using namespace pmt::ast;
using namespace pmt::rt;

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

class RepetitionFrame : public FrameBase {
public:
 RepetitionRange _range;
};

class StringLiteralFrame : public FrameBase {
public:
};

class IntegerLiteralFrame : public FrameBase {
public:
};

class CharsetFrame : public FrameBase {
public:
};

class IdentifierFrame : public FrameBase {
public:
};

class NegativeLookaheadFrame : public FrameBase {
public:
};

class PositiveLookaheadFrame : public FrameBase {
public:
};

class EofFrame : public FrameBase {
public:
};

class EpsilonFrame : public FrameBase {
public:
};

using Frame = std::variant<PassthroughFrame, SequenceFrame, ChoicesFrame, RepetitionFrame, StringLiteralFrame, IntegerLiteralFrame, CharsetFrame, IdentifierFrame, NegativeLookaheadFrame, PositiveLookaheadFrame, EofFrame, EpsilonFrame>;

class Locals {
public:
 std::unordered_map<std::string, std::pair<RuleParameters, Ast const*>> _rules;  // rule name -> <parameters, definition ast>
 std::set<std::string> _duplicate_rules;
 std::unordered_map<std::string, pmt::ast::IdType> _id_string_to_id_map;  // Only non-generic ids
 Grammar _ret;
 RuleExpression::UniqueHandle _ret_part;
 std::deque<Frame> _callstack;
 bool _keep_current_frame;
};

void caching_traversal_handle_grammar_property_start(Locals& locals_, Ast const& ast_) {
 assert(ast_.get_id() == Ids::Identifier);
 locals_._ret.set_start_rule_name(ast_.get_string());
}

void caching_traversal_handle_grammar_property(Locals& locals_, Ast const& ast_) {
 Ast const& property_name = *ast_.get_child_at(0);
 Ast const& property_value = *ast_.get_child_at(1);

 switch (property_name.get_id()) {
  case Ids::GrammarPropertyStart:
   caching_traversal_handle_grammar_property_start(locals_, property_value);
   break;
   break;
  default:
   pmt::unreachable();
 }
}

void caching_traversal_handle_production(Locals& locals_, Ast const& ast_) {
 std::string rule_name = ast_.get_child_at(0)->get_string();
 std::string rule_display_name = rule_name;

 std::string rule_id_string = pmt::ast::ReservedIds::id_to_string(pmt::ast::ReservedIds::IdDefault);
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
   case Ids::GrammarProperty:
    caching_traversal_handle_grammar_property(locals_, *ast_.get_child_at(i));
    break;
   case Ids::Production:
    caching_traversal_handle_production(locals_, *ast_.get_child_at(i));
    break;
  }
 }
}

void report_duplicate_rules(std::set<std::string> const& duplicate_rules_) {
 if (duplicate_rules_.empty()) {
  return;
 }

 static constexpr size_t const max_names_to_report = 8;

 std::string msg = "Duplicate rule name";
 msg += (duplicate_rules_.size() > 1 ? "s: " : ": ");

 std::string delim;
 for (size_t i = 0; std::string const& rule_name : duplicate_rules_) {
  if (i++ == max_names_to_report) {
   msg += ", ...";
   break;
  }
  msg += std::exchange(delim, ", ") + "\"$" + rule_name + "\"";
 }

 throw std::runtime_error(msg);
}

void check_start_rule_exists(Locals& locals_) {  // -$ Todo $- need to factor out the lev distance stuff
 if (!locals_._rules.contains(locals_._ret.get_start_rule_name())) {
  static constexpr size_t const max_lev_distance_to_report = 3;
  std::map<size_t, std::set<std::string>> lev_distances;
  pmt::util::Levenshtein lev;
  for (auto const& [rule_name, _] : locals_._rules) {
   lev_distances[lev.distance(locals_._ret.get_start_rule_name(), rule_name)].insert(rule_name);
  }

  std::string error_msg = "Start rule not found: \"$" + locals_._ret.get_start_rule_name() + "\"";
  if (!lev_distances.empty() && lev_distances.begin()->first <= max_lev_distance_to_report) {
   static constexpr size_t const max_names_to_report = 3;

   std::string delim;
   error_msg += ", did you mean: ";
   for (size_t i = 0; std::string const& rule_name : lev_distances.begin()->second) {
    if (i >= max_names_to_report) {
     error_msg += ", ...";
     break;
    } else {
     error_msg += std::exchange(delim, " OR ") + "\"$" + rule_name + "\"";
    }
    ++i;
   }
  }

  throw std::runtime_error(error_msg);
 }
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
  case Ids::Eof: {
   return EofFrame{frame_base};
  } break;
  case Ids::Epsilon: {
   return EpsilonFrame{frame_base};
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

 frame_._range = RepetitionRange(*frame_._cur_expr->get_child_at(1));
 locals_._callstack.emplace_back(construct_frame(frame_._cur_expr->get_child_at(0)));
}

void process_frame_01(Locals& locals_, RepetitionFrame& frame_) {
 RuleExpression::UniqueHandle body = std::move(locals_._ret_part);

 if /* x{,} */ (frame_._range.get_lower() == 0 && !frame_._range.get_upper().has_value()) {
  locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Choice);
  locals_._ret_part->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::OneOrMore));
  locals_._ret_part->get_child_at_back()->give_child_at_back(std::move(body));
  locals_._ret_part->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Epsilon));
 } else if /* x{a,} */ (frame_._range.get_lower() != 0 && !frame_._range.get_upper().has_value()) {
  locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Sequence);
  for (size_t i = 1; i < frame_._range.get_lower(); ++i) {
   locals_._ret_part->give_child_at_back(RuleExpression::clone(*body));
  }
  locals_._ret_part->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::OneOrMore));
  locals_._ret_part->get_child_at_back()->give_child_at_back(std::move(body));
 } else if /* x{a,b} */ (frame_._range.get_lower() != 0 && frame_._range.get_upper().has_value()) {
  locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Sequence);
  for (size_t i = 0; i < frame_._range.get_lower(); ++i) {
   locals_._ret_part->give_child_at_back(RuleExpression::clone(*body));
  }
  if (frame_._range.get_lower() != *frame_._range.get_upper()) {
   locals_._ret_part->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Choice));
   RuleExpression* const choice = locals_._ret_part->get_child_at_back();
   for (size_t i = *frame_._range.get_upper() - frame_._range.get_lower(); i != 0; --i) {
    choice->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Sequence));
    RuleExpression* const sequence = choice->get_child_at_back();
    for (size_t j = 0; j < i; ++j) {
     sequence->give_child_at_back(RuleExpression::clone(*body));
    }
   }
   choice->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Epsilon));
  }
 } else /* x{,b} */ {
  locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Choice);
  RuleExpression* const choice = locals_._ret_part.get();
  for (size_t i = *frame_._range.get_upper() - frame_._range.get_lower(); i != 0; --i) {
   choice->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Sequence));
   RuleExpression* const sequence = choice->get_child_at_back();
   for (size_t j = 0; j < i; ++j) {
    sequence->give_child_at_back(RuleExpression::clone(*body));
   }
  }
  choice->give_child_at_back(RuleExpression::construct(ClauseBase::Tag::Epsilon));
 }
}

void process_frame_00(Locals& locals_, StringLiteralFrame& frame_) {
 CharsetLiteral literal;
 std::string const& str_literal = frame_._cur_expr->get_string();
 for (char const ch : str_literal) {
  literal.push_back(Interval<SymbolType>(ch));
 }
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, IntegerLiteralFrame& frame_) {
 CharsetLiteral literal;
 literal.push_back(Interval<SymbolType>(Number(*frame_._cur_expr, std::numeric_limits<SymbolType>::max()).get_value()));
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, CharsetFrame& frame_) {
 CharsetLiteral literal;
 literal.push_back(Charset(*frame_._cur_expr).get_values());
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, IdentifierFrame& frame_) {
 std::string const& identifier = frame_._cur_expr->get_string();

 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Identifier);
 locals_._ret_part->set_identifier(identifier);

 // Check against rule names to see if it exists
 if (!locals_._rules.contains(identifier)) {
  static constexpr size_t const max_lev_distance_to_report = 3;
  std::map<size_t, std::set<std::string>> lev_distances;
  pmt::util::Levenshtein lev;
  for (auto const& [rule_name, _] : locals_._rules) {
   lev_distances[lev.distance(identifier, rule_name)].insert(rule_name);
  }

  std::string error_msg = "Rule not found: \"$" + identifier + "\"";
  if (!lev_distances.empty() && lev_distances.begin()->first <= max_lev_distance_to_report) {
   static constexpr size_t const max_closest_to_report = 3;

   std::string delim;
   error_msg += ", did you mean: ";
   for (size_t i = 0; std::string const& rule_name : lev_distances.begin()->second) {
    if (i >= max_closest_to_report) {
     error_msg += ", ...";
     break;
    } else {
     error_msg += std::exchange(delim, " OR ") + "\"$" + rule_name + "\"";
    }
    ++i;
   }
  }

  throw std::runtime_error(error_msg);
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

void process_frame_00(Locals& locals_, EofFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Eof);
}

void process_frame_00(Locals& locals_, EpsilonFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Epsilon);
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
 check_start_rule_exists(locals);
 construct_definitions(locals);
 return std::move(locals._ret);
}

}  // namespace pmt::meta