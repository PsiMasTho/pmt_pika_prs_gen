%include {#include "pmt/parserbuilder/grm_ast.hpp"}
%include {#include "pmt/util/smrt/generic_ast.hpp"}

%token_type {pmt::util::smrt::GenericAst::UniqueHandle}

%extra_argument {pmt::util::smrt::GenericAst::UniqueHandle* ast_}

%syntax_error {
  throw std::runtime_error("Syntax error");
}

%token TOKEN_STRING_LITERAL.
%token TOKEN_INTEGER_LITERAL.
%token TOKEN_BOOLEAN_LITERAL.
%token TOKEN_TERMINAL_IDENTIFIER.
%token TOKEN_RULE_IDENTIFIER.
%token TOKEN_EPSILON.
%token TOKEN_PIPE.
%token TOKEN_SEMICOLON.
%token TOKEN_EQUALS.
%token TOKEN_COMMA.
%token TOKEN_DOUBLE_DOT.
%token TOKEN_OPEN_PAREN.
%token TOKEN_CLOSE_PAREN.
%token TOKEN_OPEN_BRACE.
%token TOKEN_CLOSE_BRACE.
%token TOKEN_OPEN_SQUARE.
%token TOKEN_CLOSE_SQUARE.
%token TOKEN_OPEN_ANGLE.
%token TOKEN_CLOSE_ANGLE.
%token TOKEN_PLUS.
%token TOKEN_STAR.
%token TOKEN_QUESTION.
%token TOKEN_KW_PARAMETER_UNPACK.
%token TOKEN_KW_PARAMETER_HIDE.
%token TOKEN_KW_PARAMETER_MERGE.
%token TOKEN_KW_PARAMETER_ID.
%token TOKEN_KW_PARAMETER_CASE_SENSITIVE.
%token TOKEN_GRAMMAR_PROPERTY_START.
%token TOKEN_GRAMMAR_PROPERTY_WHITESPACE.
%token TOKEN_GRAMMAR_PROPERTY_COMMENT.
%token TOKEN_GRAMMAR_PROPERTY_NEWLINE.
%token TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE.

// - GRAMMAR -
grammar ::= statement_list(A). {
 *ast_ = std::move(A);
 (*ast_)->set_id(pmt::parserbuilder::GrmAst::NtGrammar);
}

// - STATEMENT_LIST -
statement_list(A) ::= statement_list(B) statement(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

statement_list(A) ::= statement(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children);
 A->give_child_at_back(std::move(B));
}

// - STATEMENT -
statement(A) ::= terminal_production(B). {
 A = std::move(B);
}

statement(A) ::= rule_production(B). {
 A = std::move(B);
}

statement(A) ::= grammar_property(B). {
 A = std::move(B);
}

// - TERMINAL_PRODUCTION -
terminal_production(A) ::= TOKEN_TERMINAL_IDENTIFIER(B) TOKEN_EQUALS terminal_definition(C) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

terminal_production(A) ::= TOKEN_TERMINAL_IDENTIFIER(B) TOKEN_OPEN_ANGLE terminal_parameter_list(C) TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_definition(D) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->unpack(1);
 A->give_child_at_back(std::move(D));
}

// - TERMINAL_PARAMETER_LIST -
terminal_parameter_list(A) ::= terminal_parameter_list(B) TOKEN_COMMA terminal_parameter(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

terminal_parameter_list(A) ::= terminal_parameter(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children);
 A->give_child_at_back(std::move(B));
}

// - TERMINAL_PARAMETER -
terminal_parameter(A) ::= TOKEN_KW_PARAMETER_ID(B) TOKEN_EQUALS TOKEN_STRING_LITERAL(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

terminal_parameter(A) ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE(B) TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

// - TERMINAL_DEFINITION -
terminal_definition(A) ::= terminal_choices(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinition);
 A->give_child_at_back(std::move(B));
}

// - TERMINAL_CHOICES -
terminal_choices(A) ::= terminal_sequence(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalChoices);
 A->give_child_at_back(std::move(B));
}

terminal_choices(A) ::= terminal_choices(B) TOKEN_PIPE terminal_sequence(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

// - TERMINAL_SEQUENCE -
terminal_sequence(A) ::= terminal_expression(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalSequence);
 A->give_child_at_back(std::move(B));
}

terminal_sequence(A) ::= terminal_sequence(B) terminal_expression(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

// - TERMINAL_EXPRESSION -
terminal_expression(A) ::= TOKEN_TERMINAL_IDENTIFIER(B). {
 A = std::move(B);
}

terminal_expression(A) ::= TOKEN_STRING_LITERAL(B). {
 A = std::move(B);
}

terminal_expression(A) ::= TOKEN_INTEGER_LITERAL(B). {
 A = std::move(B);
}

terminal_expression(A) ::= TOKEN_EPSILON(B). {
 A = std::move(B);
}

terminal_expression(A) ::= TOKEN_OPEN_PAREN terminal_choices(B) TOKEN_CLOSE_PAREN. {
 A = std::move(B);
}

terminal_expression(A) ::= terminal_expression(B) TOKEN_QUESTION. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "," "1"
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));
 
 auto rhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 rhs->set_string("10#1");
 repetition_info->give_child_at_back(std::move(rhs));

 A->give_child_at_back(std::move(repetition_info));
}

terminal_expression(A) ::= terminal_expression(B) TOKEN_STAR. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: ","
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");

 repetition_info->give_child_at_back(std::move(comma));
 A->give_child_at_back(std::move(repetition_info));
}

terminal_expression(A) ::= terminal_expression(B) TOKEN_PLUS. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "1" ","
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);

 auto lhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 lhs->set_string("10#1");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(lhs));

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

 A->give_child_at_back(std::move(repetition_info));
}

terminal_expression(A) ::= terminal_expression(B) terminal_repetition_range(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

terminal_expression(A) ::= range_expression(B). {
 A = std::move(B);
}

// - RANGE_EXPRESSION -
range_expression(A) ::= TOKEN_OPEN_SQUARE range_literal(B) TOKEN_DOUBLE_DOT range_literal(C) TOKEN_CLOSE_SQUARE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

// - RANGE_LITERAL -
range_literal(A) ::= TOKEN_STRING_LITERAL(B). {
 A = std::move(B);
}

range_literal(A) ::= TOKEN_INTEGER_LITERAL(B). {
 A = std::move(B);
}

// - TERMINAL_REPETITION_RANGE -
terminal_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_COMMA(B) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
}

terminal_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_COMMA(C) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

terminal_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_COMMA(B) TOKEN_INTEGER_LITERAL(C) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

terminal_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_COMMA(C) TOKEN_INTEGER_LITERAL(D) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->give_child_at_back(std::move(D));
}

terminal_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
}

// - RULE_PRODUCTION -
rule_production(A) ::= TOKEN_RULE_IDENTIFIER(B) TOKEN_EQUALS rule_definition(C) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

rule_production(A) ::= TOKEN_RULE_IDENTIFIER(B) TOKEN_OPEN_ANGLE rule_parameter_list(C) TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_definition(D) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->unpack(1);
 A->give_child_at_back(std::move(D));
}

// - RULE_PARAMETER_LIST -
rule_parameter_list(A) ::= rule_parameter_list(B) TOKEN_COMMA rule_parameter(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

rule_parameter_list(A) ::= rule_parameter(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children);
 A->give_child_at_back(std::move(B));
}

// - RULE_PARAMETER -
rule_parameter(A) ::= TOKEN_KW_PARAMETER_ID(B) TOKEN_EQUALS TOKEN_STRING_LITERAL(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

rule_parameter(A) ::= TOKEN_KW_PARAMETER_UNPACK(B) TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

rule_parameter(A) ::= TOKEN_KW_PARAMETER_HIDE(B) TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

rule_parameter(A) ::= TOKEN_KW_PARAMETER_MERGE(B) TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

// - RULE_DEFINITION -
rule_definition(A) ::= rule_choices(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalDefinition);
 A->give_child_at_back(std::move(B));
}

// - RULE_CHOICES -
rule_choices(A) ::= rule_sequence(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalChoices);
 A->give_child_at_back(std::move(B));
}

rule_choices(A) ::= rule_choices(B) TOKEN_PIPE rule_sequence(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

// - RULE_SEQUENCE -
rule_sequence(A) ::= rule_expression(B). {
 A = std::move(B);
}

rule_sequence(A) ::= rule_sequence(B) rule_expression(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalSequence);
 A->give_child_at_back(std::move(B));
 if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtNonterminalSequence) {
  A->unpack(A->get_children_size() - 1);
 }
 A->give_child_at_back(std::move(C));
 if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtNonterminalSequence) {
  A->unpack(A->get_children_size() - 1);
 }
}

// - RULE_EXPRESSION -
rule_expression(A) ::= TOKEN_RULE_IDENTIFIER(B). {
 A = std::move(B);
}

rule_expression(A) ::= TOKEN_TERMINAL_IDENTIFIER(B). {
 A = std::move(B);
}

rule_expression(A) ::= TOKEN_STRING_LITERAL(B). {
 A = std::move(B);
}

rule_expression(A) ::= TOKEN_INTEGER_LITERAL(B). {
 A = std::move(B);
}

rule_expression(A) ::= TOKEN_EPSILON(B). {
 A = std::move(B);
}

rule_expression(A) ::= TOKEN_OPEN_PAREN rule_choices(B) TOKEN_CLOSE_PAREN. {
 A = std::move(B);
}

rule_expression(A) ::= rule_expression(B) TOKEN_QUESTION. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "," "1"
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));
 
 auto rhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 rhs->set_string("10#1");
 repetition_info->give_child_at_back(std::move(rhs));

 A->give_child_at_back(std::move(repetition_info));
}

rule_expression(A) ::= rule_expression(B) TOKEN_STAR. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: ","
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");

 repetition_info->give_child_at_back(std::move(comma));
 A->give_child_at_back(std::move(repetition_info));
}

rule_expression(A) ::= rule_expression(B) TOKEN_PLUS. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "1" ","
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);

 auto lhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 lhs->set_string("10#1");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(lhs));

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

 A->give_child_at_back(std::move(repetition_info));
}

rule_expression(A) ::= rule_expression(B) rule_repetition_range(C). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

// - RULE_REPETITION_RANGE -
rule_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_COMMA(B) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
}

rule_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_COMMA(C) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

rule_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_COMMA(B) TOKEN_INTEGER_LITERAL(C) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

rule_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_COMMA(C) TOKEN_INTEGER_LITERAL(D) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->give_child_at_back(std::move(D));
}

rule_repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
}

// - GRAMMAR_PROPERTY -
grammar_property(A) ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE(B) TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL(C) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

grammar_property(A) ::= TOKEN_GRAMMAR_PROPERTY_START(B) TOKEN_EQUALS TOKEN_RULE_IDENTIFIER(C) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

grammar_property(A) ::= TOKEN_GRAMMAR_PROPERTY_COMMENT(B) TOKEN_EQUALS terminal_definition_pair_list(C) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

grammar_property(A) ::= TOKEN_GRAMMAR_PROPERTY_NEWLINE(B) TOKEN_EQUALS terminal_definition(C) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

grammar_property(A) ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE(B) TOKEN_EQUALS terminal_definition(C) TOKEN_SEMICOLON. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

// - TERMINAL_DEFINITION_PAIR_LIST -
terminal_definition_pair_list(A) ::= terminal_definition_pair(B). {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinitionPairList);
 A->give_child_at_back(std::move(B));
}

terminal_definition_pair_list(A) ::= terminal_definition_pair_list(B) TOKEN_COMMA terminal_definition_pair(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

// - TERMINAL_DEFINITION_PAIR -
terminal_definition_pair(A) ::= TOKEN_OPEN_BRACE terminal_definition(B) TOKEN_COMMA terminal_definition(C) TOKEN_CLOSE_BRACE. {
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinitionPair);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}