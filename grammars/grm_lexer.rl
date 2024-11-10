#include "pmt/util/parse/grm_lexer.hpp"

#include "pmt/util/parse/grm_ast.hpp"

#define MAKE_RL_CONTEXT_AVAILABLE \
  char const*  &p = _p;           \
  char const*  &pe = _pe;         \
  char const*  &eof = _eof;       \
  char const*  &ts = _ts;         \
  char const*  &te = _te;         \
  int          &act = _act;       \
  int          &cs = _cs; 

%%{
 machine GrmLexer;
  
 StringLiteral                = '"' (print - ['"])* '"';
 IntegerLiteral               = [0-9]+ '#' [0-9a-zA-Z]+;
 BooleanLiteral               = 'true' | 'false';
 TerminalIdentifier              = '$' [a-zA-Z][a-zA-Z0-9_]+;
 RuleIdentifier               = '%' [a-zA-Z][a-zA-Z0-9_]+;
 Epsilon                      = 'epsilon';
 Pipe                         = '|';
 SemiColon                    = ';';
 Equals                       = '=';
 Comma                        = ',';
 DoubleDot                    = '..';
 OpenParen                    = '(';
 CloseParen                   = ')';
 OpenBrace                    = '{';
 CloseBrace                   = '}';
 OpenSquare                   = '[';
 CloseSquare                  = ']';
 Plus                         = '+';
 Star                         = '*';
 Question                     = '?';
 KwParameterUnpack            = 'unpack';
 KwParameterHide              = 'hide';
 KwParameterMerge             = 'merge';

 SingleLineComment = "//" [^\n]*;
 MultiLineComment  = "/*" ([^*] | '*' [^/])* "*/";
  
 main :=
 |*
  StringLiteral => { 
   ++ts;
   --te;
   accepted->set_id(GrmAst::TkStringLiteral);
   fbreak;
  };

  IntegerLiteral => {
   accepted->set_id(GrmAst::TkIntegerLiteral);
   fbreak;
  };

  BooleanLiteral => {
   accepted->set_id(GrmAst::TkBooleanLiteral);
   fbreak;
  };
  
  TerminalIdentifier => {
   accepted->set_id(GrmAst::TkTerminalIdentifier);
   fbreak;
  };

  RuleIdentifier => {
   accepted->set_id(GrmAst::TkRuleIdentifier);
   fbreak;
  };

  Epsilon => {
   accepted->set_id(GrmAst::TkEpsilon);
   fbreak;
  };
  
  Pipe => {
   accepted->set_id(GrmAst::TkPipe);
   fbreak;
  };
  
  SemiColon => {
   accepted->set_id(GrmAst::TkSemiColon);
   fbreak;
  };
  
  Equals => {
   accepted->set_id(GrmAst::TkEquals);
   fbreak;
  };

  Comma => {
   accepted->set_id(GrmAst::TkComma);
   fbreak;
  };

  DoubleDot => {
   accepted->set_id(GrmAst::TkDoubleDot);
   fbreak;
  };

  OpenParen => {
   accepted->set_id(GrmAst::TkOpenParen);
   fbreak;
  };

  CloseParen => {
   accepted->set_id(GrmAst::TkCloseParen);
   fbreak;
  };

  OpenBrace => {
   accepted->set_id(GrmAst::TkOpenBrace);
   fbreak;
  };

  CloseBrace => {
   accepted->set_id(GrmAst::TkCloseBrace);
   fbreak;
  };

  OpenSquare => {
   accepted->set_id(GrmAst::TkOpenSquare);
   fbreak;
  };

  CloseSquare => {
   accepted->set_id(GrmAst::TkCloseSquare);
   fbreak;
  };

  Plus => {
   accepted->set_id(GrmAst::TkPlus);
   fbreak;
  };

  Star => {
   accepted->set_id(GrmAst::TkStar);
   fbreak;
  };

  Question => {
   accepted->set_id(GrmAst::TkQuestion);
   fbreak;
  };

  KwParameterUnpack => {
   accepted->set_id(GrmAst::TkKwParameterUnpack);
   fbreak;
  };

  KwParameterHide => {
   accepted->set_id(GrmAst::TkKwParameterHide);
   fbreak;
  };

  KwParameterMerge => {
   accepted->set_id(GrmAst::TkKwParameterMerge);
   fbreak;
  };
        
  space;
  SingleLineComment;
  MultiLineComment;

  any => 
				{ throw std::runtime_error("lexer error"); };
 *|;
}%%

namespace {

%% write data;

} // namespace

namespace pmt::util::parse {

GrmLexer::GrmLexer(std::string_view input_)
:
 _p{input_.data()},
 _pe{_p + input_.size()},
 _eof{_pe},
 _ts{nullptr},
 _te{nullptr},
 _act{0},
 _cs{0}
{
 MAKE_RL_CONTEXT_AVAILABLE;
 %% write init;
}

auto GrmLexer::next_token() -> GenericAst::UniqueHandle {
 MAKE_RL_CONTEXT_AVAILABLE;

 GenericAst::UniqueHandle accepted = GenericAst::construct(GenericAst::Tag::Token);

 %% write exec;

 if (_p == _pe) {
  return nullptr;
 }

 accepted->set_token(GenericAst::TokenType(ts, te - ts));
 return accepted;
}

auto GrmLexer::is_eof() const -> bool {
 return _p == _pe;
}

} // namespace pmt::util::parse

#undef MAKE_RL_CONTEXT_AVAILABLE