%include {#include "pmt/parserbuilder/grm_ast.hpp"}
%include {#include "pmt/util/parse/generic_ast.hpp"}

%token_type {pmt::util::parse::GenericAst::UniqueHandle}

%extra_argument {pmt::util::parse::GenericAst::UniqueHandle* ast_}

%syntax_error {
  throw std::runtime_error("Syntax error");
}

%token TOKEN_STRING_LITERAL.
%token TOKEN_INTEGER_LITERAL.
%token TOKEN_BOOLEAN_LITERAL.
%token TOKEN_TERMINAL_IDENTIFIER.
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

grammar ::= production_list(A). {
 *ast_ = std::move(A);
 (*ast_)->set_id(pmt::parserbuilder::GrmAst::NtGrammar);
}

production_list(A) ::= production_list(B) production(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

production_list(A) ::= production(B). {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->give_child_at_back(std::move(B));
}

production(A) ::= TOKEN_TERMINAL_IDENTIFIER(B) TOKEN_EQUALS expr(C) TOKEN_SEMICOLON. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtTerminalProduction);
 A->give_child_at_back(std::move(B));
 auto default_id = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Token, pmt::parserbuilder::GrmAst::TkStringLiteral);
 default_id->set_token("IdDefault");
 A->give_child_at_back(std::move(default_id));
 A->give_child_at_back(std::move(C));
}

production(A) ::= TOKEN_TERMINAL_IDENTIFIER(B) TOKEN_OPEN_ANGLE TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL(C) TOKEN_CLOSE_ANGLE TOKEN_EQUALS expr(D) TOKEN_SEMICOLON. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtTerminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->give_child_at_back(std::move(D));
}

expr(A) ::= choices(B). {
 A = std::move(B);
}

term(A) ::= TOKEN_TERMINAL_IDENTIFIER(B). {
 A = std::move(B);
}

term(A) ::= TOKEN_STRING_LITERAL(B). {
 A = std::move(B);
}

term(A) ::= TOKEN_INTEGER_LITERAL(B). {
 A = std::move(B);
}

term(A) ::= TOKEN_EPSILON(B). {
 A = std::move(B);
}

term(A) ::= TOKEN_OPEN_PAREN expr(B) TOKEN_CLOSE_PAREN. {
 A = std::move(B);
}

term(A) ::= term(B) TOKEN_QUESTION. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "," "1"
 auto repetition_info = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 repetition_info->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);

 auto comma = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Token);
 comma->set_id(pmt::parserbuilder::GrmAst::TkComma);
 comma->set_token(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));
 
 auto rhs = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Token);
 rhs->set_id(pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 rhs->set_token("10#1");
 repetition_info->give_child_at_back(std::move(rhs));

 A->give_child_at_back(std::move(repetition_info));
}

term(A) ::= term(B) TOKEN_STAR. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: ","
 auto repetition_info = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 repetition_info->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);

 auto comma = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Token);
 comma->set_id(pmt::parserbuilder::GrmAst::TkComma);
 comma->set_token(",");

 repetition_info->give_child_at_back(std::move(comma));
 A->give_child_at_back(std::move(repetition_info));
}

term(A) ::= term(B) TOKEN_PLUS. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "1" ","
 auto repetition_info = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 repetition_info->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);

 auto lhs = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Token);
 lhs->set_id(pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 lhs->set_token("10#1");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(lhs));

 auto comma = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Token);
 comma->set_id(pmt::parserbuilder::GrmAst::TkComma);
 comma->set_token(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

 A->give_child_at_back(std::move(repetition_info));
}

term(A) ::= term(B) repetition_range(C). {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetition);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

term(A) ::= TOKEN_OPEN_SQUARE range_literal(B) TOKEN_DOUBLE_DOT range_literal(C) TOKEN_CLOSE_SQUARE. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

range_literal(A) ::= TOKEN_STRING_LITERAL(B). {
 A = std::move(B);
}

range_literal(A) ::= TOKEN_INTEGER_LITERAL(B). {
 A = std::move(B);
}

repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_COMMA(B) TOKEN_CLOSE_BRACE. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);
 A->give_child_at_back(std::move(B));
}

repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_COMMA(C) TOKEN_CLOSE_BRACE. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_COMMA(B) TOKEN_INTEGER_LITERAL(C) TOKEN_CLOSE_BRACE. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}

repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_COMMA(C) TOKEN_INTEGER_LITERAL(D) TOKEN_CLOSE_BRACE. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->give_child_at_back(std::move(D));
}

repetition_range(A) ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL(B) TOKEN_CLOSE_BRACE. {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtRepetitionRange);
 A->give_child_at_back(std::move(B));
}

sequence(A) ::= term(B). {
 A = std::move(B);
}

sequence(A) ::= sequence(B) term(C). {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtSequence);
 A->give_child_at_back(std::move(B));
 if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtSequence) {
  A->unpack(A->get_children_size() - 1);
 }
 A->give_child_at_back(std::move(C));
 if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtSequence) {
  A->unpack(A->get_children_size() - 1);
 }
}

choices(A) ::= sequence(B). {
 A = pmt::util::parse::GenericAst::construct(pmt::util::parse::GenericAst::Tag::Children);
 A->set_id(pmt::parserbuilder::GrmAst::NtChoices);
 A->give_child_at_back(std::move(B));
}

choices(A) ::= choices(B) TOKEN_PIPE sequence(C). {
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}

