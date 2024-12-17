#include "pmt/parserbuilder/grm_lexer.hpp"

#include "pmt/parserbuilder/grm_ast.hpp"

#define MAKE_RL_CONTEXT_AVAILABLE \
  char const*  &p = _p;           \
  char const*  &pe = _pe;         \
  char const*  &eof = _eof;       \
  char const*  &ts = _ts;         \
  char const*  &te = _te;         \
  int          &act = _act;       \
  int          &cs = _cs;

 #define ACCEPT_TOKEN(id)                                         \
  do {                                                            \
   accepted = GenericAst::construct(GenericAst::Tag::String, id); \
  } while (0)

%%{
 machine GrmLexer;
  
 StringLiteral                    = '"' (print - ['"])* '"';
 IntegerLiteral                   = [0-9]+ '#' [0-9a-zA-Z]+;
 BooleanLiteral                   = 'true' | 'false';
 TerminalIdentifier               = '$' [_a-zA-Z][_a-zA-Z0-9]+;
 RuleIdentifier                   = '%' [_a-zA-Z][_a-zA-Z0-9]+;
 Epsilon                          = 'epsilon';
 Pipe                             = '|';
 SemiColon                        = ';';
 Equals                           = '=';
 Comma                            = ',';
 DoubleDot                        = '..';
 OpenParen                        = '(';
 CloseParen                       = ')';
 OpenBrace                        = '{';
 CloseBrace                       = '}';
 OpenSquare                       = '[';
 CloseSquare                      = ']';
 OpenAngle                        = '<';
 CloseAngle                       = '>';
 Plus                             = '+';
 Star                             = '*';
 Question                         = '?';
 KwParameterUnpack                = 'unpack';
 KwParameterHide                  = 'hide';
 KwParameterMerge                 = 'merge';
 KwParameterId                    = 'id';
 KwParameterCaseSensitive         = 'case_sensitive';
 GrammarPropertyStart             = '@start';
 GrammarPropertyWhitespace        = '@whitespace';
 GrammarPropertySingleLineComment = '@single_line_comment';
 GrammarPropertyMultiLineComment  = '@multi_line_comment';
 GrammarPropertyCaseSensitive     = '@case_sensitive';

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

  RuleIdentifier => {
   ACCEPT_TOKEN(GrmAst::TkRuleIdentifier);
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

  OpenAngle => {
   ACCEPT_TOKEN(GrmAst::TkOpenAngle);
   fbreak;
  };

  CloseAngle => {
   ACCEPT_TOKEN(GrmAst::TkCloseAngle);
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

  KwParameterId => {
   ACCEPT_TOKEN(GrmAst::TkKwParameterId);
   fbreak;
  };

  KwParameterCaseSensitive => {
   ACCEPT_TOKEN(GrmAst::TkKwParameterCaseSensitive);
   fbreak;
  };

  GrammarPropertyStart => {
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyStart);
   fbreak;
  };

  GrammarPropertyWhitespace => {
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyWhitespace);
   fbreak;
  };

  GrammarPropertySingleLineComment => {
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertySingleLineComment);
   fbreak;
  };

  GrammarPropertyMultiLineComment => {
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyMultiLineComment);
   fbreak;
  };

  GrammarPropertyCaseSensitive => {
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyCaseSensitive);
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

namespace pmt::parserbuilder {
 using namespace pmt::util::parsert;

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

 accepted->set_string(GenericAst::StringType(ts, te - ts));
 return accepted;
}

auto GrmLexer::is_eof() const -> bool {
 return _p == _pe;
}

} // namespace pmt::util::parse

#undef MAKE_RL_CONTEXT_AVAILABLE