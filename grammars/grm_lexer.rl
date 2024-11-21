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

 #define ACCEPT_TOKEN(id)                                    \
  do {                                                       \
   accepted = GenericAst::construct(GenericAst::Tag::Token); \
   accepted->set_id(id);                                     \
  } while (0)

%%{
 machine GrmLexer;
  
 StringLiteral                = '"' (print - ['"])* '"';
 IntegerLiteral               = [0-9]+ '#' [0-9a-zA-Z]+;
 BooleanLiteral               = 'true' | 'false';
 TerminalIdentifier              = '$' [_a-zA-Z][a-zA-Z0-9_]+;
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
   ACCEPT_TOKEN(GrmAst::TkStringLiteral);
   fbreak;
  };

  IntegerLiteral => {
   ACCEPT_TOKEN(GrmAst::TkIntegerLiteral);
   fbreak;
  };

  BooleanLiteral => {
   ACCEPT_TOKEN(GrmAst::TkBooleanLiteral);
   fbreak;
  };
  
  TerminalIdentifier => {
   ACCEPT_TOKEN(GrmAst::TkTerminalIdentifier);
   fbreak;
  };

  Epsilon => {
   ACCEPT_TOKEN(GrmAst::TkEpsilon);
   fbreak;
  };
  
  Pipe => {
   ACCEPT_TOKEN(GrmAst::TkPipe);
   fbreak;
  };
  
  SemiColon => {
   ACCEPT_TOKEN(GrmAst::TkSemiColon);
   fbreak;
  };
  
  Equals => {
   ACCEPT_TOKEN(GrmAst::TkEquals);
   fbreak;
  };

  Comma => {
   ACCEPT_TOKEN(GrmAst::TkComma);
   fbreak;
  };

  DoubleDot => {
   ACCEPT_TOKEN(GrmAst::TkDoubleDot);
   fbreak;
  };

  OpenParen => {
   ACCEPT_TOKEN(GrmAst::TkOpenParen);
   fbreak;
  };

  CloseParen => {
   ACCEPT_TOKEN(GrmAst::TkCloseParen);
   fbreak;
  };

  OpenBrace => {
   ACCEPT_TOKEN(GrmAst::TkOpenBrace);
   fbreak;
  };

  CloseBrace => {
   ACCEPT_TOKEN(GrmAst::TkCloseBrace);
   fbreak;
  };

  OpenSquare => {
   ACCEPT_TOKEN(GrmAst::TkOpenSquare);
   fbreak;
  };

  CloseSquare => {
   ACCEPT_TOKEN(GrmAst::TkCloseSquare);
   fbreak;
  };

  Plus => {
   ACCEPT_TOKEN(GrmAst::TkPlus);
   fbreak;
  };

  Star => {
   ACCEPT_TOKEN(GrmAst::TkStar);
   fbreak;
  };

  Question => {
   ACCEPT_TOKEN(GrmAst::TkQuestion);
   fbreak;
  };

  KwParameterUnpack => {
   ACCEPT_TOKEN(GrmAst::TkKwParameterUnpack);
   fbreak;
  };

  KwParameterHide => {
   ACCEPT_TOKEN(GrmAst::TkKwParameterHide);
   fbreak;
  };

  KwParameterMerge => {
   ACCEPT_TOKEN(GrmAst::TkKwParameterMerge);
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

 GenericAst::UniqueHandle accepted;

 %% write exec;

 if (_p == _pe && accepted.get() == nullptr) {
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