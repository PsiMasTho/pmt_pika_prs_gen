#include "pmt/parser/grammar/grammar_from_ast.hpp"

#include "parser/grammar/charset_literal.hpp"
#include "parser/grammar/rule.hpp"
#include "pmt/asserts.hpp"
#include "pmt/parser/generic_ast.hpp"
#include "pmt/parser/grammar/ast.hpp"
#include "pmt/parser/grammar/charset.hpp"
#include "pmt/parser/grammar/index_permutation_generator.hpp"
#include "pmt/parser/grammar/number.hpp"
#include "pmt/parser/grammar/repetition_range.hpp"
#include "pmt/parser/grammar/string_literal.hpp"

namespace pmt::parser::grammar {
using namespace pmt::base;
using namespace pmt::util::sm;

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
 std::optional<GenericAst const*> _delim_cur_path;
 std::vector<size_t> _index_permutation;
 size_t _idx = 0;
 bool _is_permuted : 1 = false;
 bool _is_delimiting : 1 = false;
};

class PermuteFrame : public FrameBase {
public:
 RuleExpression::UniqueHandle _sub_part;
 IndexPermutationGenerator _index_permutation_generator;
 size_t _idx = 0;
};

class PermuteDelimitedFrame : public FrameBase {
public:
 RuleExpression::UniqueHandle _sub_part;
 IndexPermutationGenerator _index_permutation_generator;
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

using Frame = std::variant<ExpressionFrame, SequenceFrame, PermuteFrame, PermuteDelimitedFrame, ChoicesFrame, RepetitionFrame, StringLiteralFrame, IntegerLiteralFrame, CharsetFrame, IdentifierFrame, HiddenFrame, EpsilonFrame>;

class Locals {
public:
 std::unordered_map<std::string, GenericAst const*> _definitions;
 Grammar _ret;
 RuleExpression::UniqueHandle _ret_part;
 std::deque<Frame> _callstack;
 size_t _extra_rules_created = 0;
 bool _keep_current_frame;
};

void initial_traversal_handle_grammar_property_start(Locals& locals_, GenericAst const& ast_) {
 assert(ast_.get_id() == Ast::TkNonterminalIdentifier || ast_.get_id() == Ast::TkTerminalIdentifier);
 locals_._ret.set_start_rule_name(ast_.get_string());
}

void initial_traversal_handle_grammar_property(Locals& locals_, GenericAst const& ast_) {
 GenericAst const& property_name = *ast_.get_child_at(0);
 GenericAst const& property_value = *ast_.get_child_at(1);

 switch (property_name.get_id()) {
  case Ast::TkGrammarPropertyStart:
   initial_traversal_handle_grammar_property_start(locals_, property_value);
   break;
  case Ast::TkGrammarPropertyWhitespace:
  case Ast::TkGrammarPropertyComment:
  case Ast::TkGrammarPropertyNewline:
   break;
  default:
   pmt::unreachable();
 }
}

void initial_traversal_handle_rule_production(Locals& locals_, GenericAst const& ast_) {
 std::string rule_name = ast_.get_child_at(0)->get_string();
 std::string rule_display_name = rule_name;

 std::string rule_id_string = GenericId::id_to_string(GenericId::IdDefault);
 bool rule_merge = RuleParameters::MERGE_DEFAULT;
 bool rule_unpack = RuleParameters::UNPACK_DEFAULT;

 GenericAst const* rule_definition = nullptr;

 for (size_t i = 1; i < ast_.get_children_size(); ++i) {
  GenericAst const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Ast::NtParameterId: {
    rule_id_string = StringLiteral(child).get_value();
   } break;
   case Ast::NtParameterDisplayName: {
    rule_display_name = StringLiteral(child).get_value();
   } break;
   case Ast::NtParameterMerge: {
    rule_merge = child.get_string() == "true";
   } break;
   case Ast::NtParameterUnpack: {
    rule_unpack = child.get_string() == "true";
   } break;
   case Ast::NtNonterminalDefinition:
   case Ast::NtTerminalDefinition: {
    rule_definition = child.get_child_at(0);
   } break;
  }
 }

 Rule& rule = locals_._ret.get_or_create_rule(rule_name);
 rule._parameters._display_name = std::move(rule_display_name);
 rule._parameters._id_string = std::move(rule_id_string);
 rule._parameters._merge = rule_merge;
 rule._parameters._unpack = rule_unpack;

 assert(rule_definition != nullptr);
 locals_._definitions[rule_name] = rule_definition;
}

void initial_traversal(Locals& locals_, GenericAst const& ast_) {
 for (size_t i = 0; i < ast_.get_children_size(); ++i) {
  GenericAst const& child = *ast_.get_child_at(i);
  switch (child.get_id()) {
   case Ast::NtGrammarProperty:
    initial_traversal_handle_grammar_property(locals_, *ast_.get_child_at(i));
    break;
   case Ast::NtTerminalProduction:
   case Ast::NtNonterminalProduction:
    initial_traversal_handle_rule_production(locals_, *ast_.get_child_at(i));
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
  case Ast::NtNonterminalDefinition:
  case Ast::NtTerminalDefinition:
  case Ast::NtNonterminalExpression:
  case Ast::NtTerminalExpression: {
   return ExpressionFrame{frame_base};
  } break;
  case Ast::NtNonterminalChoices:
  case Ast::NtTerminalChoices: {
   return ChoicesFrame{frame_base};
  } break;
  case Ast::NtRepetitionExpression: {
   return RepetitionFrame{frame_base};
  } break;
  case Ast::NtNonterminalSequence:
  case Ast::NtTerminalSequence: {
   return SequenceFrame{frame_base};
  } break;
  case Ast::TkNonterminalIdentifier:
  case Ast::TkTerminalIdentifier: {
   return IdentifierFrame{frame_base};
  } break;
  case Ast::TkStringLiteral: {
   return StringLiteralFrame{frame_base};
  } break;
  case Ast::TkIntegerLiteral: {
   return IntegerLiteralFrame{frame_base};
  } break;
  case Ast::NtTerminalCharset: {
   return CharsetFrame{frame_base};
  }
  case Ast::NtHidden: {
   return HiddenFrame{frame_base};
  }
  case Ast::TkEpsilon: {
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

 if (!frame_._is_permuted) {
  std::generate_n(std::back_inserter(frame_._index_permutation), frame_._ast_cur_path->get_children_size(), [n = 0]() mutable { return n++; });
  frame_._is_permuted = true;
 }

 if (frame_._index_permutation.empty()) {
  locals_._ret_part = build_epsilon();
  return;
 }

 frame_._sub_part = RuleExpression::construct(ClauseBase::Tag::Sequence);

 locals_._keep_current_frame = true;
}

void process_frame_01(Locals& locals_, SequenceFrame& frame_) {
 locals_._keep_current_frame = true;
 ++frame_._stage;

 if (frame_._is_delimiting) {
  locals_._callstack.emplace_back(construct_frame(*frame_._delim_cur_path));
 } else {
  locals_._callstack.emplace_back(construct_frame(frame_._ast_cur_path->get_child_at(frame_._index_permutation[frame_._idx])));
 }
}

void process_frame_02(Locals& locals_, SequenceFrame& frame_) {
 --frame_._stage;

 if (frame_._is_delimiting) {
  frame_._is_delimiting = false;
  frame_._sub_part->give_child_at_back(std::move(locals_._ret_part));
  locals_._keep_current_frame = true;
  return;
 }

 ++frame_._idx;

 frame_._sub_part->give_child_at_back(std::move(locals_._ret_part));

 // If is last
 if (frame_._idx == frame_._index_permutation.size()) {
  locals_._ret_part = std::move(frame_._sub_part);
 } else {
  frame_._is_delimiting = frame_._delim_cur_path.has_value();
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
 std::string const& str_literal = StringLiteral(*frame_._ast_cur_path).get_value();
 for (char const ch : str_literal) {
  literal.push_back<CharsetLiteral::IsHidden::No>(Interval<SymbolValueType>(ch));
 }
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Literal);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, IntegerLiteralFrame& frame_) {
 CharsetLiteral literal;
 literal.push_back<CharsetLiteral::IsHidden::No>(Interval<SymbolValueType>(Number(*frame_._ast_cur_path).get_value()));
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Literal);
 locals_._ret_part->set_charset_literal(std::move(literal));
}

void process_frame_00(Locals& locals_, CharsetFrame& frame_) {
 CharsetLiteral literal;
 literal.push_back<CharsetLiteral::IsHidden::No>(Charset(*frame_._ast_cur_path).get_values());
 locals_._ret_part = RuleExpression::construct(ClauseBase::Tag::Literal);
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
 for (auto& [rule_name, rule_definition] : locals_._definitions) {
  locals_._ret.get_or_create_rule(rule_name)._definition = construct_definition(locals_, rule_definition);
 }
}

}  // namespace

auto GrammarFromAst::make(Args args_) -> Grammar {
 Locals locals;
 initial_traversal(locals, args_._ast);
 construct_definitions(locals);

 return std::move(locals._ret);
}

}  // namespace pmt::parser::grammar