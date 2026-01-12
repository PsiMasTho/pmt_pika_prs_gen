#include "pmt/parser/grammar/grammar_from_ast_2.hpp"

#include "parser/grammar/charset_literal.hpp"
#include "parser/grammar/rule.hpp"
#include "pmt/asserts.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/grammar/ast_2.hpp"
#include "pmt/parser/grammar/charset.hpp"
#include "pmt/parser/grammar/number.hpp"
#include "pmt/parser/grammar/repetition_range.hpp"

namespace pmt::parser::grammar {
using namespace pmt::base;
using namespace pmt::sm;
using namespace pmt::parser::rt;

namespace {
class FrameBase {
public:
 GenericAst const* _ast_cur_path;
 size_t _stage = 0;
};

class ExpressionFrame : public FrameBase {
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

class HiddenFrame : public FrameBase {
public:
};

class EpsilonFrame : public FrameBase {
public:
};

using Frame = std::variant<ExpressionFrame, SequenceFrame, ChoicesFrame, RepetitionFrame, StringLiteralFrame, IntegerLiteralFrame, CharsetFrame, IdentifierFrame, HiddenFrame, EpsilonFrame>;

class Locals {
public:
 std::unordered_map<std::string, std::pair<RuleParameters, GenericAst const*>> _rules;  // rule name -> <parameters, definition ast>
 std::unordered_map<std::string, GenericId::IdType> _id_string_to_id_map;               // Only non-generic ids
 Grammar _ret;
 RuleExpression::UniqueHandle _ret_part;
 std::deque<Frame> _callstack;
 bool _keep_current_frame;
};

void initial_traversal_handle_grammar_property_start(Locals& locals_, GenericAst const& ast_) {
 assert(ast_.get_id() == Ast2::Identifier);
 locals_._ret.set_start_rule_name(ast_.get_string());
}

void initial_traversal_handle_grammar_property(Locals& locals_, GenericAst const& ast_) {
 GenericAst const& property_name = *ast_.get_child_at(0);
 GenericAst const& property_value = *ast_.get_child_at(1);

 switch (property_name.get_id()) {
  case Ast2::GrammarPropertyStart:
   initial_traversal_handle_grammar_property_start(locals_, property_value);
   break;
   break;
  default:
   pmt::unreachable();
 }
}

void initial_traversal_handle_production(Locals& locals_, GenericAst const& ast_) {
 std::string rule_name = ast_.get_child_at(0)->get_string();
 std::string rule_display_name = rule_name;

 std::string rule_id_string = GenericId::id_to_string(GenericId::IdDefault);
 bool rule_merge = RuleParametersBase::MERGE_DEFAULT;
 bool rule_unpack = RuleParametersBase::UNPACK_DEFAULT;
 bool rule_hide = RuleParametersBase::HIDE_DEFAULT;

 GenericAst const* rule_definition = nullptr;

 for (size_t i = 1; i < ast_.get_children_size(); ++i) {
  GenericAst const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Ast2::ParameterId: {
    rule_id_string = child.get_string();
   } break;
   case Ast2::ParameterDisplayName: {
    rule_display_name = child.get_string();
   } break;
   case Ast2::ParameterMerge: {
    rule_merge = child.get_string() == "true";
   } break;
   case Ast2::ParameterUnpack: {
    rule_unpack = child.get_string() == "true";
   } break;
   case Ast2::ParameterHide: {
    rule_hide = child.get_string() == "true";
   } break;
   case Ast2::Definition: {
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
 locals_._rules[rule_name] = std::make_pair(std::move(rule_parameters), rule_definition);
}

void initial_traversal(Locals& locals_, GenericAst const& ast_) {
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  GenericAst const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Ast2::GrammarProperty:
    initial_traversal_handle_grammar_property(locals_, *ast_.get_child_at(i));
    break;
   case Ast2::Production:
    initial_traversal_handle_production(locals_, *ast_.get_child_at(i));
    break;
  }
 }
}

auto build_epsilon() -> RuleExpression::UniqueHandle {
 return RuleExpression::construct(ClauseBase::Tag::Sequence);
}

auto construct_frame(GenericAst const* ast_cur_path_) -> Frame {
 FrameBase frame_base{._ast_cur_path = ast_cur_path_, ._stage = 0};

 switch (ast_cur_path_->get_id()) {
  case Ast2::Definition: {
   return ExpressionFrame{frame_base};
  } break;
  case Ast2::Choices: {
   return ChoicesFrame{frame_base};
  } break;
  case Ast2::Repetition: {
   return RepetitionFrame{frame_base};
  } break;
  case Ast2::Sequence: {
   return SequenceFrame{frame_base};
  } break;
  case Ast2::Identifier: {
   return IdentifierFrame{frame_base};
  } break;
  case Ast2::StringLiteral: {
   return StringLiteralFrame{frame_base};
  } break;
  case Ast2::IntegerLiteral: {
   return IntegerLiteralFrame{frame_base};
  } break;
  case Ast2::Charset: {
   return CharsetFrame{frame_base};
  }
  case Ast2::Hidden: {
   return HiddenFrame{frame_base};
  }
  case Ast2::Epsilon: {
   return EpsilonFrame{frame_base};
  }
  default:
   pmt::unreachable();
 }
}

void process_frame_00(Locals& locals_, ExpressionFrame& frame_) {
 locals_._callstack.emplace_back(construct_frame(frame_._ast_cur_path->get_child_at_front()));
}

void process_frame_00(Locals& locals_, SequenceFrame& frame_) {
 ++frame_._stage;
 frame_._sub_part = RuleExpression::construct(ClauseBase::Tag::Sequence);
 locals_._keep_current_frame = true;
}

void process_frame_01(Locals& locals_, SequenceFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._ast_cur_path->get_child_at(frame_._idx)));
}

void process_frame_02(Locals& locals_, SequenceFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 frame_._sub_part->give_child_at_back(std::move(locals_._ret_part));

 // If is last
 if (frame_._idx == frame_._ast_cur_path->get_children_size()) {
  locals_._ret_part = std::move(frame_._sub_part);
 } else {
  locals_._keep_current_frame = true;
 }
}

void process_frame_00(Locals& locals_, ChoicesFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._ast_cur_path->get_child_at(frame_._idx)));
}

void process_frame_01(Locals& locals_, ChoicesFrame& frame_) {
 --frame_._stage;
 ++frame_._idx;

 if (frame_._idx == 1) {
  frame_._sub_part = RuleExpression::construct(ClauseBase::Tag::Choice);
 }

 frame_._sub_part->give_child_at_back(std::move(locals_._ret_part));

 if (frame_._idx < frame_._ast_cur_path->get_children_size()) {
  locals_._keep_current_frame = true;
  return;
 }

 locals_._ret_part = std::move(frame_._sub_part);
}

void process_frame_00(Locals& locals_, RepetitionFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 frame_._range = RepetitionRange(*frame_._ast_cur_path->get_child_at(1));
 locals_._callstack.emplace_back(construct_frame(frame_._ast_cur_path->get_child_at(0)));
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
 std::string const& str_literal = frame_._ast_cur_path->get_string();
 for (char const ch : str_literal) {
  literal.push_back(Interval<SymbolType>(ch));
 }
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, IntegerLiteralFrame& frame_) {
 CharsetLiteral literal;
 literal.push_back(Interval<SymbolType>(Number(*frame_._ast_cur_path).get_value()));
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, CharsetFrame& frame_) {
 CharsetLiteral literal;
 literal.push_back(Charset(*frame_._ast_cur_path).get_values());
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::CharsetLiteral);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, IdentifierFrame& frame_) {
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Identifier);
 locals_._ret_part->set_identifier(frame_._ast_cur_path->get_string());
}

void process_frame_00(Locals& locals_, HiddenFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 locals_._callstack.emplace_back(construct_frame(frame_._ast_cur_path->get_child_at(0)));
}

void process_frame_01(Locals& locals_, HiddenFrame& frame_) {
 RuleExpression::UniqueHandle hidden = RuleExpression::construct(ClauseBase::Tag::Hidden);
 hidden->give_child_at_back(std::move(locals_._ret_part));
 locals_._ret_part = std::move(hidden);
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

auto construct_definition(Locals& locals_, GenericAst const* ast_) -> RuleExpression::UniqueHandle {
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

auto GrammarFromAst2::make(Args args_) -> Grammar {
 Locals locals;
 initial_traversal(locals, args_._ast);
 construct_definitions(locals);

 return std::move(locals._ret);
}

}  // namespace pmt::parser::grammar