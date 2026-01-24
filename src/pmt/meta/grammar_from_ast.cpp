#include "pmt/meta/grammar_from_ast.hpp"

#include "pmt/meta/charset.hpp"
#include "pmt/meta/charset_literal.hpp"
#include "pmt/meta/language.hpp"
#include "pmt/meta/number.hpp"
#include "pmt/meta/repetition_range.hpp"
#include "pmt/meta/rule.hpp"
#include "pmt/unreachable.hpp"
#include "pmt/util/overloaded.hpp"
#include "pmt/util/uint_to_str.hpp"

#include <sstream>

namespace pmt::meta {
using namespace pmt::container;
using namespace pmt::rt;

namespace {
class FrameBase {
public:
 Ast const* _cur_expr;
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

class NegativeLookaheadFrame : public FrameBase {
public:
};

class PositiveLookaheadFrame : public FrameBase {
public:
};

class EpsilonFrame : public FrameBase {
public:
};

using Frame = std::variant<ExpressionFrame, SequenceFrame, ChoicesFrame, RepetitionFrame, StringLiteralFrame, IntegerLiteralFrame, CharsetFrame, IdentifierFrame, NegativeLookaheadFrame, PositiveLookaheadFrame, EpsilonFrame>;

class Locals {
public:
 std::unordered_map<std::string, std::pair<RuleParameters, Ast const*>> _rules;  // rule name -> <parameters, definition ast>
 std::unordered_map<std::string, AstId::IdType> _id_string_to_id_map;            // Only non-generic ids
 Grammar _ret;
 RuleExpression::UniqueHandle _ret_part;
 std::deque<Frame> _callstack;
 bool _keep_current_frame;
};

void caching_traversal_handle_grammar_property_start(Locals& locals_, Ast const& ast_) {
 assert(ast_.get_id() == Language::Identifier);
 locals_._ret.set_start_rule_name(ast_.get_string());
}

void caching_traversal_handle_grammar_property(Locals& locals_, Ast const& ast_) {
 Ast const& property_name = *ast_.get_child_at(0);
 Ast const& property_value = *ast_.get_child_at(1);

 switch (property_name.get_id()) {
  case Language::GrammarPropertyStart:
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

 std::string rule_id_string = AstId::id_to_string(AstId::IdDefault);
 bool rule_merge = RuleParametersBase::MERGE_DEFAULT;
 bool rule_unpack = RuleParametersBase::UNPACK_DEFAULT;
 bool rule_hide = RuleParametersBase::HIDE_DEFAULT;

 Ast const* rule_definition = nullptr;

 for (size_t i = 1; i < ast_.get_children_size(); ++i) {
  Ast const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Language::ParameterId: {
    rule_id_string = child.get_string();
   } break;
   case Language::ParameterDisplayName: {
    rule_display_name = child.get_string();
   } break;
   case Language::ParameterMerge: {
    rule_merge = child.get_string() == "true";
   } break;
   case Language::ParameterUnpack: {
    rule_unpack = child.get_string() == "true";
   } break;
   case Language::ParameterHide: {
    rule_hide = child.get_string() == "true";
   } break;
   case Language::Definition: {
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

void caching_traversal(Locals& locals_, Ast const& ast_) {
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  Ast const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Language::GrammarProperty:
    caching_traversal_handle_grammar_property(locals_, *ast_.get_child_at(i));
    break;
   case Language::Production:
    caching_traversal_handle_production(locals_, *ast_.get_child_at(i));
    break;
  }
 }
}

auto count_hidden_productions_needed(Ast& ast_) -> size_t {
 std::vector<Ast const*> pending{&ast_};
 size_t ret = 0;
 while (!pending.empty()) {
  Ast const* cur = pending.back();
  pending.pop_back();

  switch (cur->get_id()) {
   case Language::Production: {
    pending.push_back(cur->get_child_at(cur->get_children_size() - 1));
   } break;
   case pmt::rt::AstId::IdRoot:
   case Language::Definition:
   case Language::Choices:
   case Language::Sequence:
   case Language::NegativeLookahead:
   case Language::PositiveLookahead: {
    for (size_t i = 0; i < cur->get_children_size(); ++i) {
     pending.push_back(cur->get_child_at(i));
    }
   } break;
   case Language::Repetition: {
    pending.push_back(cur->get_child_at_front());
   } break;
   case Language::Identifier: {
   } break;
   case Language::Hidden: {
    pending.push_back(cur->get_child_at_front());
    ++ret;
   } break;
   case Language::StringLiteral:
   case Language::GrammarProperty:
   case Language::IntegerLiteral:
   case Language::Charset:
   case Language::Epsilon:
    break;
   default:
    pmt::unreachable();
  }
 }

 return ret;
}

void construct_hidden_productions(Locals& locals_, Ast& ast_) {
 class AstPosition {
 public:
  Ast* _parent;
  size_t _child_idx;
 };

 std::vector<AstPosition> pending;
 std::unordered_map<std::string, std::pair<AstPosition, bool>> production_positions;  // <rule_name, <position, visited>>

 // clang-format off
 auto const push_and_visit = pmt::util::Overloaded{
  [&](Ast* parent_, size_t child_idx_){
   pending.emplace_back(parent_, child_idx_);
  },
  [&](std::string const& rule_name_) {
   auto const itr = production_positions.find(rule_name_);
   if (itr == production_positions.end() || std::exchange(itr->second.second, true)) { // not found or is visited
    return;
   }

   pending.emplace_back(itr->second.first._parent, itr->second.first._child_idx);
  }
 };
 // clang-format on

 auto const take = [&]() {
  auto tmp = pending.back();
  pending.pop_back();
  return tmp;
 };

 auto construct_hidden_prouction = [&, hidden_production_counter = 0ull, padding = pmt::util::digits_needed(count_hidden_productions_needed(ast_), 16)](Ast::UniqueHandle expr_) mutable -> std::string {
  std::stringstream rule_name;
  rule_name << "_hidden_" << pmt::util::uint_to_string(hidden_production_counter++, padding, pmt::util::hex_alphabet_uppercase);
  Ast::UniqueHandle production = Ast::construct(Ast::Tag::Parent, Language::Production);
  Ast::UniqueHandle identifier = Ast::construct(Ast::Tag::String, Language::Identifier);
  identifier->set_string(rule_name.str());
  Ast::UniqueHandle parameter_hide = Ast::construct(Ast::Tag::String, Language::ParameterHide);
  parameter_hide->set_string("true");
  Ast::UniqueHandle definition = Ast::construct(Ast::Tag::Parent, Language::Definition);
  definition->give_child_at_back(std::move(expr_));

  production->give_child_at_back(std::move(identifier));
  production->give_child_at_back(std::move(parameter_hide));
  production->give_child_at_back(std::move(definition));

  size_t const index = ast_.get_children_size();
  ast_.give_child_at_back(std::move(production));
  production_positions[rule_name.str()] = std::make_pair(AstPosition{._parent = &ast_, ._child_idx = index}, false);

  return rule_name.str();
 };

 // Push all topmost productions, index their positions
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  if (ast_.get_child_at(i)->get_id() != Language::Production) {
   continue;
  }

  push_and_visit(&ast_, i);

  production_positions[ast_.get_child_at(i)->get_child_at_front()->get_string()] = std::make_pair(AstPosition{._parent = &ast_, ._child_idx = i}, false);
 }

 while (!pending.empty()) {
  auto const [parent, child_idx] = take();
  if (parent == nullptr) {
   continue;
  }
  Ast* child = parent->get_child_at(child_idx);
  if (child == nullptr) {
   continue;
  }

  switch (child->get_id()) {
   case Language::Production: {
    push_and_visit(child, child->get_children_size() - 1);
   } break;
   case Language::Definition:
   case Language::Choices:
   case Language::Sequence:
   case Language::NegativeLookahead:
   case Language::PositiveLookahead: {
    for (size_t i = 0; i < child->get_children_size(); ++i) {
     push_and_visit(child, i);
    }
   } break;
   case Language::Repetition: {
    push_and_visit(child, 0);
   } break;
   case Language::Identifier: {
    push_and_visit(child->get_string());
   } break;
   case Language::Hidden: {
    std::string const constructed_name = construct_hidden_prouction(child->take_child_at_front());
    parent->take_child_at(child_idx);
    parent->give_child_at(child_idx, Ast::construct(Ast::Tag::String, Language::Identifier));
    parent->get_child_at(child_idx)->set_string(constructed_name);
    push_and_visit(constructed_name);
   } break;
   case Language::StringLiteral:
   case Language::IntegerLiteral:
   case Language::Charset:
   case Language::Epsilon:
    break;
   default:
    pmt::unreachable();
  }
 }
}

auto build_epsilon() -> RuleExpression::UniqueHandle {
 return RuleExpression::construct(ClauseBase::Tag::Sequence);
}

auto construct_frame(Ast const* ast_cur_) -> Frame {
 FrameBase frame_base{._cur_expr = ast_cur_, ._stage = 0};

 switch (ast_cur_->get_id()) {
  case Language::Definition: {
   return ExpressionFrame{frame_base};
  } break;
  case Language::Choices: {
   return ChoicesFrame{frame_base};
  } break;
  case Language::Repetition: {
   return RepetitionFrame{frame_base};
  } break;
  case Language::Sequence: {
   return SequenceFrame{frame_base};
  } break;
  case Language::Identifier: {
   return IdentifierFrame{frame_base};
  } break;
  case Language::StringLiteral: {
   return StringLiteralFrame{frame_base};
  } break;
  case Language::IntegerLiteral: {
   return IntegerLiteralFrame{frame_base};
  } break;
  case Language::Charset: {
   return CharsetFrame{frame_base};
  } break;
  case Language::NegativeLookahead: {
   return NegativeLookaheadFrame{frame_base};
  } break;
  case Language::PositiveLookahead: {
   return PositiveLookaheadFrame{frame_base};
  } break;
  case Language::Epsilon: {
   return EpsilonFrame{frame_base};
  } break;
  default:
   pmt::unreachable();
 }
}

void process_frame_00(Locals& locals_, ExpressionFrame& frame_) {
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
 literal.push_back(Interval<SymbolType>(Number(*frame_._cur_expr).get_value()));
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
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Identifier);
 locals_._ret_part->set_identifier(frame_._cur_expr->get_string());
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

auto GrammarFromAst::make(Ast::UniqueHandle ast_) -> Grammar {
 Locals locals;
 construct_hidden_productions(locals, *ast_);
 caching_traversal(locals, *ast_);
 construct_definitions(locals);
 return std::move(locals._ret);
}

}  // namespace pmt::meta