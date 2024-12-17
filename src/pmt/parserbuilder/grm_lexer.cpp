
#line 1 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
#include "pmt/parserbuilder/grm_lexer.hpp"

#include "pmt/parserbuilder/grm_ast.hpp"

#define MAKE_RL_CONTEXT_AVAILABLE \
  char const *&p = _p;            \
  char const *&pe = _pe;          \
  char const *&eof = _eof;        \
  char const *&ts = _ts;          \
  char const *&te = _te;          \
  int &act = _act;                \
  int &cs = _cs;

#define ACCEPT_TOKEN(id)                                           \
  do {                                                             \
    accepted = GenericAst::construct(GenericAst::Tag::String, id); \
  } while (0)

#line 229 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

namespace {

#line 24 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
static const int GrmLexer_start = 97;
static const int GrmLexer_first_final = 97;
static const int GrmLexer_error = -1;

static const int GrmLexer_en_main = 97;

#line 234 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

}  // namespace

namespace pmt::parserbuilder {
using namespace pmt::util::parsert;

GrmLexer::GrmLexer(std::string_view input_)
 : _p{input_.data()}
 , _pe{_p + input_.size()}
 , _eof{_pe}
 , _ts{nullptr}
 , _te{nullptr}
 , _act{0}
 , _cs{0} {
  MAKE_RL_CONTEXT_AVAILABLE;

#line 47 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
  {
    cs = GrmLexer_start;
    ts = 0;
    te = 0;
    act = 0;
  }

#line 252 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
}

auto GrmLexer::next_token() -> GenericAst::UniqueHandle {
  MAKE_RL_CONTEXT_AVAILABLE;

  GenericAst::UniqueHandle accepted;

#line 60 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
  {
    if (p == pe)
      goto _test_eof;
    switch (cs) {
    tr0 :
#line 227 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      { p = ((te)) - 1; }
      { throw std::runtime_error("lexer error"); }
    }
      goto st97;
    tr2 :
#line 60 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ++ts;
        --te;
        ACCEPT_TOKEN(GrmAst::TkStringLiteral);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr7 :
#line 224 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
    }
      goto st97;
    tr23 :
#line 217 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkGrammarPropertyCaseSensitive);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr40 :
#line 212 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkGrammarPropertyMultiLineComment);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr59 :
#line 207 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkGrammarPropertySingleLineComment);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr62 :
#line 197 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkGrammarPropertyStart);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr71 :
#line 202 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkGrammarPropertyWhitespace);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr83 :
#line 192 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterCaseSensitive);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr88 :
#line 87 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkEpsilon);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr91 :
#line 72 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkBooleanLiteral);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr93 :
#line 177 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterHide);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr96 :
#line 182 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterMerge);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr100 :
#line 172 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterUnpack);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr101 :
#line 227 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      { throw std::runtime_error("lexer error"); }
    }
      goto st97;
    tr102 :
#line 222 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
    }
      goto st97;
    tr106 :
#line 117 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenParen);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr107 :
#line 122 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseParen);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr108 :
#line 162 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkStar);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr109 :
#line 157 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkPlus);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr110 :
#line 107 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkComma);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr114 :
#line 97 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkSemiColon);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr115 :
#line 147 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenAngle);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr116 :
#line 102 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkEquals);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr117 :
#line 152 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseAngle);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr118 :
#line 167 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkQuestion);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr120 :
#line 137 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenSquare);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr121 :
#line 142 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseSquare);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr130 :
#line 127 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenBrace);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr131 :
#line 92 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkPipe);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr132 :
#line 132 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseBrace);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr133 :
#line 227 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      { throw std::runtime_error("lexer error"); }
    }
      goto st97;
    tr135 :
#line 77 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        ACCEPT_TOKEN(GrmAst::TkTerminalIdentifier);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr137 :
#line 82 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        ACCEPT_TOKEN(GrmAst::TkRuleIdentifier);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr138 :
#line 112 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkDoubleDot);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr140 :
#line 223 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
    }
      goto st97;
    tr141 :
#line 67 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        ACCEPT_TOKEN(GrmAst::TkIntegerLiteral);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    tr150 :
#line 187 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterId);
        {
          p++;
          cs = 97;
          goto _out;
        }
      }
    }
      goto st97;
    st97 :
#line 1 "NONE"
    {
      ts = 0;
    }
      if (++p == pe)
        goto _test_eof97;
      case 97:
#line 1 "NONE"
      {
        ts = p;
      }
#line 283 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        switch ((*p)) {
          case 32:
            goto tr102;
          case 34:
            goto tr103;
          case 36:
            goto tr104;
          case 37:
            goto tr105;
          case 40:
            goto tr106;
          case 41:
            goto tr107;
          case 42:
            goto tr108;
          case 43:
            goto tr109;
          case 44:
            goto tr110;
          case 46:
            goto st103;
          case 47:
            goto tr112;
          case 59:
            goto tr114;
          case 60:
            goto tr115;
          case 61:
            goto tr116;
          case 62:
            goto tr117;
          case 63:
            goto tr118;
          case 64:
            goto tr119;
          case 91:
            goto tr120;
          case 93:
            goto tr121;
          case 99:
            goto tr122;
          case 101:
            goto tr123;
          case 102:
            goto tr124;
          case 104:
            goto tr125;
          case 105:
            goto st113;
          case 109:
            goto tr127;
          case 116:
            goto tr128;
          case 117:
            goto tr129;
          case 123:
            goto tr130;
          case 124:
            goto tr131;
          case 125:
            goto tr132;
        }
        if ((*p) > 13) {
          if (48 <= (*p) && (*p) <= 57)
            goto tr113;
        } else if ((*p) >= 9)
          goto tr102;
        goto tr101;
      tr103 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st98;
      st98:
        if (++p == pe)
          goto _test_eof98;
      case 98:
#line 328 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 34)
          goto tr2;
        if ((*p) > 38) {
          if (40 <= (*p) && (*p) <= 126)
            goto st0;
        } else if ((*p) >= 32)
          goto st0;
        goto tr133;
      st0:
        if (++p == pe)
          goto _test_eof0;
      case 0:
        if ((*p) == 34)
          goto tr2;
        if ((*p) > 38) {
          if (40 <= (*p) && (*p) <= 126)
            goto st0;
        } else if ((*p) >= 32)
          goto st0;
        goto tr0;
      tr104 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st99;
      st99:
        if (++p == pe)
          goto _test_eof99;
      case 99:
#line 355 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 95)
          goto st1;
        if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st1;
        } else if ((*p) >= 65)
          goto st1;
        goto tr133;
      st1:
        if (++p == pe)
          goto _test_eof1;
      case 1:
        if ((*p) == 95)
          goto st100;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st100;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st100;
        } else
          goto st100;
        goto tr0;
      st100:
        if (++p == pe)
          goto _test_eof100;
      case 100:
        if ((*p) == 95)
          goto st100;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st100;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st100;
        } else
          goto st100;
        goto tr135;
      tr105 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st101;
      st101:
        if (++p == pe)
          goto _test_eof101;
      case 101:
#line 400 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 95)
          goto st2;
        if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st2;
        } else if ((*p) >= 65)
          goto st2;
        goto tr133;
      st2:
        if (++p == pe)
          goto _test_eof2;
      case 2:
        if ((*p) == 95)
          goto st102;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st102;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st102;
        } else
          goto st102;
        goto tr0;
      st102:
        if (++p == pe)
          goto _test_eof102;
      case 102:
        if ((*p) == 95)
          goto st102;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st102;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st102;
        } else
          goto st102;
        goto tr137;
      st103:
        if (++p == pe)
          goto _test_eof103;
      case 103:
        if ((*p) == 46)
          goto tr138;
        goto tr133;
      tr112 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st104;
      st104:
        if (++p == pe)
          goto _test_eof104;
      case 104:
#line 452 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        switch ((*p)) {
          case 42:
            goto st3;
          case 47:
            goto st105;
        }
        goto tr133;
      st3:
        if (++p == pe)
          goto _test_eof3;
      case 3:
        if ((*p) == 42)
          goto st4;
        goto st3;
      st4:
        if (++p == pe)
          goto _test_eof4;
      case 4:
        if ((*p) == 47)
          goto tr7;
        goto st3;
      st105:
        if (++p == pe)
          goto _test_eof105;
      case 105:
        if ((*p) == 10)
          goto tr140;
        goto st105;
      tr113 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st106;
      st106:
        if (++p == pe)
          goto _test_eof106;
      case 106:
#line 485 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 35)
          goto st5;
        if (48 <= (*p) && (*p) <= 57)
          goto st6;
        goto tr133;
      st5:
        if (++p == pe)
          goto _test_eof5;
      case 5:
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st107;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st107;
        } else
          goto st107;
        goto tr0;
      st107:
        if (++p == pe)
          goto _test_eof107;
      case 107:
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st107;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st107;
        } else
          goto st107;
        goto tr141;
      st6:
        if (++p == pe)
          goto _test_eof6;
      case 6:
        if ((*p) == 35)
          goto st5;
        if (48 <= (*p) && (*p) <= 57)
          goto st6;
        goto tr0;
      tr119 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st108;
      st108:
        if (++p == pe)
          goto _test_eof108;
      case 108:
#line 532 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        switch ((*p)) {
          case 99:
            goto st7;
          case 109:
            goto st20;
          case 115:
            goto st37;
          case 119:
            goto st58;
        }
        goto tr133;
      st7:
        if (++p == pe)
          goto _test_eof7;
      case 7:
        if ((*p) == 97)
          goto st8;
        goto tr0;
      st8:
        if (++p == pe)
          goto _test_eof8;
      case 8:
        if ((*p) == 115)
          goto st9;
        goto tr0;
      st9:
        if (++p == pe)
          goto _test_eof9;
      case 9:
        if ((*p) == 101)
          goto st10;
        goto tr0;
      st10:
        if (++p == pe)
          goto _test_eof10;
      case 10:
        if ((*p) == 95)
          goto st11;
        goto tr0;
      st11:
        if (++p == pe)
          goto _test_eof11;
      case 11:
        if ((*p) == 115)
          goto st12;
        goto tr0;
      st12:
        if (++p == pe)
          goto _test_eof12;
      case 12:
        if ((*p) == 101)
          goto st13;
        goto tr0;
      st13:
        if (++p == pe)
          goto _test_eof13;
      case 13:
        if ((*p) == 110)
          goto st14;
        goto tr0;
      st14:
        if (++p == pe)
          goto _test_eof14;
      case 14:
        if ((*p) == 115)
          goto st15;
        goto tr0;
      st15:
        if (++p == pe)
          goto _test_eof15;
      case 15:
        if ((*p) == 105)
          goto st16;
        goto tr0;
      st16:
        if (++p == pe)
          goto _test_eof16;
      case 16:
        if ((*p) == 116)
          goto st17;
        goto tr0;
      st17:
        if (++p == pe)
          goto _test_eof17;
      case 17:
        if ((*p) == 105)
          goto st18;
        goto tr0;
      st18:
        if (++p == pe)
          goto _test_eof18;
      case 18:
        if ((*p) == 118)
          goto st19;
        goto tr0;
      st19:
        if (++p == pe)
          goto _test_eof19;
      case 19:
        if ((*p) == 101)
          goto tr23;
        goto tr0;
      st20:
        if (++p == pe)
          goto _test_eof20;
      case 20:
        if ((*p) == 117)
          goto st21;
        goto tr0;
      st21:
        if (++p == pe)
          goto _test_eof21;
      case 21:
        if ((*p) == 108)
          goto st22;
        goto tr0;
      st22:
        if (++p == pe)
          goto _test_eof22;
      case 22:
        if ((*p) == 116)
          goto st23;
        goto tr0;
      st23:
        if (++p == pe)
          goto _test_eof23;
      case 23:
        if ((*p) == 105)
          goto st24;
        goto tr0;
      st24:
        if (++p == pe)
          goto _test_eof24;
      case 24:
        if ((*p) == 95)
          goto st25;
        goto tr0;
      st25:
        if (++p == pe)
          goto _test_eof25;
      case 25:
        if ((*p) == 108)
          goto st26;
        goto tr0;
      st26:
        if (++p == pe)
          goto _test_eof26;
      case 26:
        if ((*p) == 105)
          goto st27;
        goto tr0;
      st27:
        if (++p == pe)
          goto _test_eof27;
      case 27:
        if ((*p) == 110)
          goto st28;
        goto tr0;
      st28:
        if (++p == pe)
          goto _test_eof28;
      case 28:
        if ((*p) == 101)
          goto st29;
        goto tr0;
      st29:
        if (++p == pe)
          goto _test_eof29;
      case 29:
        if ((*p) == 95)
          goto st30;
        goto tr0;
      st30:
        if (++p == pe)
          goto _test_eof30;
      case 30:
        if ((*p) == 99)
          goto st31;
        goto tr0;
      st31:
        if (++p == pe)
          goto _test_eof31;
      case 31:
        if ((*p) == 111)
          goto st32;
        goto tr0;
      st32:
        if (++p == pe)
          goto _test_eof32;
      case 32:
        if ((*p) == 109)
          goto st33;
        goto tr0;
      st33:
        if (++p == pe)
          goto _test_eof33;
      case 33:
        if ((*p) == 109)
          goto st34;
        goto tr0;
      st34:
        if (++p == pe)
          goto _test_eof34;
      case 34:
        if ((*p) == 101)
          goto st35;
        goto tr0;
      st35:
        if (++p == pe)
          goto _test_eof35;
      case 35:
        if ((*p) == 110)
          goto st36;
        goto tr0;
      st36:
        if (++p == pe)
          goto _test_eof36;
      case 36:
        if ((*p) == 116)
          goto tr40;
        goto tr0;
      st37:
        if (++p == pe)
          goto _test_eof37;
      case 37:
        switch ((*p)) {
          case 105:
            goto st38;
          case 116:
            goto st55;
        }
        goto tr0;
      st38:
        if (++p == pe)
          goto _test_eof38;
      case 38:
        if ((*p) == 110)
          goto st39;
        goto tr0;
      st39:
        if (++p == pe)
          goto _test_eof39;
      case 39:
        if ((*p) == 103)
          goto st40;
        goto tr0;
      st40:
        if (++p == pe)
          goto _test_eof40;
      case 40:
        if ((*p) == 108)
          goto st41;
        goto tr0;
      st41:
        if (++p == pe)
          goto _test_eof41;
      case 41:
        if ((*p) == 101)
          goto st42;
        goto tr0;
      st42:
        if (++p == pe)
          goto _test_eof42;
      case 42:
        if ((*p) == 95)
          goto st43;
        goto tr0;
      st43:
        if (++p == pe)
          goto _test_eof43;
      case 43:
        if ((*p) == 108)
          goto st44;
        goto tr0;
      st44:
        if (++p == pe)
          goto _test_eof44;
      case 44:
        if ((*p) == 105)
          goto st45;
        goto tr0;
      st45:
        if (++p == pe)
          goto _test_eof45;
      case 45:
        if ((*p) == 110)
          goto st46;
        goto tr0;
      st46:
        if (++p == pe)
          goto _test_eof46;
      case 46:
        if ((*p) == 101)
          goto st47;
        goto tr0;
      st47:
        if (++p == pe)
          goto _test_eof47;
      case 47:
        if ((*p) == 95)
          goto st48;
        goto tr0;
      st48:
        if (++p == pe)
          goto _test_eof48;
      case 48:
        if ((*p) == 99)
          goto st49;
        goto tr0;
      st49:
        if (++p == pe)
          goto _test_eof49;
      case 49:
        if ((*p) == 111)
          goto st50;
        goto tr0;
      st50:
        if (++p == pe)
          goto _test_eof50;
      case 50:
        if ((*p) == 109)
          goto st51;
        goto tr0;
      st51:
        if (++p == pe)
          goto _test_eof51;
      case 51:
        if ((*p) == 109)
          goto st52;
        goto tr0;
      st52:
        if (++p == pe)
          goto _test_eof52;
      case 52:
        if ((*p) == 101)
          goto st53;
        goto tr0;
      st53:
        if (++p == pe)
          goto _test_eof53;
      case 53:
        if ((*p) == 110)
          goto st54;
        goto tr0;
      st54:
        if (++p == pe)
          goto _test_eof54;
      case 54:
        if ((*p) == 116)
          goto tr59;
        goto tr0;
      st55:
        if (++p == pe)
          goto _test_eof55;
      case 55:
        if ((*p) == 97)
          goto st56;
        goto tr0;
      st56:
        if (++p == pe)
          goto _test_eof56;
      case 56:
        if ((*p) == 114)
          goto st57;
        goto tr0;
      st57:
        if (++p == pe)
          goto _test_eof57;
      case 57:
        if ((*p) == 116)
          goto tr62;
        goto tr0;
      st58:
        if (++p == pe)
          goto _test_eof58;
      case 58:
        if ((*p) == 104)
          goto st59;
        goto tr0;
      st59:
        if (++p == pe)
          goto _test_eof59;
      case 59:
        if ((*p) == 105)
          goto st60;
        goto tr0;
      st60:
        if (++p == pe)
          goto _test_eof60;
      case 60:
        if ((*p) == 116)
          goto st61;
        goto tr0;
      st61:
        if (++p == pe)
          goto _test_eof61;
      case 61:
        if ((*p) == 101)
          goto st62;
        goto tr0;
      st62:
        if (++p == pe)
          goto _test_eof62;
      case 62:
        if ((*p) == 115)
          goto st63;
        goto tr0;
      st63:
        if (++p == pe)
          goto _test_eof63;
      case 63:
        if ((*p) == 112)
          goto st64;
        goto tr0;
      st64:
        if (++p == pe)
          goto _test_eof64;
      case 64:
        if ((*p) == 97)
          goto st65;
        goto tr0;
      st65:
        if (++p == pe)
          goto _test_eof65;
      case 65:
        if ((*p) == 99)
          goto st66;
        goto tr0;
      st66:
        if (++p == pe)
          goto _test_eof66;
      case 66:
        if ((*p) == 101)
          goto tr71;
        goto tr0;
      tr122 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st109;
      st109:
        if (++p == pe)
          goto _test_eof109;
      case 109:
#line 968 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 97)
          goto st67;
        goto tr133;
      st67:
        if (++p == pe)
          goto _test_eof67;
      case 67:
        if ((*p) == 115)
          goto st68;
        goto tr0;
      st68:
        if (++p == pe)
          goto _test_eof68;
      case 68:
        if ((*p) == 101)
          goto st69;
        goto tr0;
      st69:
        if (++p == pe)
          goto _test_eof69;
      case 69:
        if ((*p) == 95)
          goto st70;
        goto tr0;
      st70:
        if (++p == pe)
          goto _test_eof70;
      case 70:
        if ((*p) == 115)
          goto st71;
        goto tr0;
      st71:
        if (++p == pe)
          goto _test_eof71;
      case 71:
        if ((*p) == 101)
          goto st72;
        goto tr0;
      st72:
        if (++p == pe)
          goto _test_eof72;
      case 72:
        if ((*p) == 110)
          goto st73;
        goto tr0;
      st73:
        if (++p == pe)
          goto _test_eof73;
      case 73:
        if ((*p) == 115)
          goto st74;
        goto tr0;
      st74:
        if (++p == pe)
          goto _test_eof74;
      case 74:
        if ((*p) == 105)
          goto st75;
        goto tr0;
      st75:
        if (++p == pe)
          goto _test_eof75;
      case 75:
        if ((*p) == 116)
          goto st76;
        goto tr0;
      st76:
        if (++p == pe)
          goto _test_eof76;
      case 76:
        if ((*p) == 105)
          goto st77;
        goto tr0;
      st77:
        if (++p == pe)
          goto _test_eof77;
      case 77:
        if ((*p) == 118)
          goto st78;
        goto tr0;
      st78:
        if (++p == pe)
          goto _test_eof78;
      case 78:
        if ((*p) == 101)
          goto tr83;
        goto tr0;
      tr123 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st110;
      st110:
        if (++p == pe)
          goto _test_eof110;
      case 110:
#line 1062 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 112)
          goto st79;
        goto tr133;
      st79:
        if (++p == pe)
          goto _test_eof79;
      case 79:
        if ((*p) == 115)
          goto st80;
        goto tr0;
      st80:
        if (++p == pe)
          goto _test_eof80;
      case 80:
        if ((*p) == 105)
          goto st81;
        goto tr0;
      st81:
        if (++p == pe)
          goto _test_eof81;
      case 81:
        if ((*p) == 108)
          goto st82;
        goto tr0;
      st82:
        if (++p == pe)
          goto _test_eof82;
      case 82:
        if ((*p) == 111)
          goto st83;
        goto tr0;
      st83:
        if (++p == pe)
          goto _test_eof83;
      case 83:
        if ((*p) == 110)
          goto tr88;
        goto tr0;
      tr124 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st111;
      st111:
        if (++p == pe)
          goto _test_eof111;
      case 111:
#line 1107 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 97)
          goto st84;
        goto tr133;
      st84:
        if (++p == pe)
          goto _test_eof84;
      case 84:
        if ((*p) == 108)
          goto st85;
        goto tr0;
      st85:
        if (++p == pe)
          goto _test_eof85;
      case 85:
        if ((*p) == 115)
          goto st86;
        goto tr0;
      st86:
        if (++p == pe)
          goto _test_eof86;
      case 86:
        if ((*p) == 101)
          goto tr91;
        goto tr0;
      tr125 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st112;
      st112:
        if (++p == pe)
          goto _test_eof112;
      case 112:
#line 1138 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 105)
          goto st87;
        goto tr133;
      st87:
        if (++p == pe)
          goto _test_eof87;
      case 87:
        if ((*p) == 100)
          goto st88;
        goto tr0;
      st88:
        if (++p == pe)
          goto _test_eof88;
      case 88:
        if ((*p) == 101)
          goto tr93;
        goto tr0;
      st113:
        if (++p == pe)
          goto _test_eof113;
      case 113:
        if ((*p) == 100)
          goto tr150;
        goto tr133;
      tr127 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st114;
      st114:
        if (++p == pe)
          goto _test_eof114;
      case 114:
#line 1169 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 101)
          goto st89;
        goto tr133;
      st89:
        if (++p == pe)
          goto _test_eof89;
      case 89:
        if ((*p) == 114)
          goto st90;
        goto tr0;
      st90:
        if (++p == pe)
          goto _test_eof90;
      case 90:
        if ((*p) == 103)
          goto st91;
        goto tr0;
      st91:
        if (++p == pe)
          goto _test_eof91;
      case 91:
        if ((*p) == 101)
          goto tr96;
        goto tr0;
      tr128 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st115;
      st115:
        if (++p == pe)
          goto _test_eof115;
      case 115:
#line 1200 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 114)
          goto st92;
        goto tr133;
      st92:
        if (++p == pe)
          goto _test_eof92;
      case 92:
        if ((*p) == 117)
          goto st86;
        goto tr0;
      tr129 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st116;
      st116:
        if (++p == pe)
          goto _test_eof116;
      case 116:
#line 1217 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 110)
          goto st93;
        goto tr133;
      st93:
        if (++p == pe)
          goto _test_eof93;
      case 93:
        if ((*p) == 112)
          goto st94;
        goto tr0;
      st94:
        if (++p == pe)
          goto _test_eof94;
      case 94:
        if ((*p) == 97)
          goto st95;
        goto tr0;
      st95:
        if (++p == pe)
          goto _test_eof95;
      case 95:
        if ((*p) == 99)
          goto st96;
        goto tr0;
      st96:
        if (++p == pe)
          goto _test_eof96;
      case 96:
        if ((*p) == 107)
          goto tr100;
        goto tr0;
    }
  _test_eof97:
    cs = 97;
    goto _test_eof;
  _test_eof98:
    cs = 98;
    goto _test_eof;
  _test_eof0:
    cs = 0;
    goto _test_eof;
  _test_eof99:
    cs = 99;
    goto _test_eof;
  _test_eof1:
    cs = 1;
    goto _test_eof;
  _test_eof100:
    cs = 100;
    goto _test_eof;
  _test_eof101:
    cs = 101;
    goto _test_eof;
  _test_eof2:
    cs = 2;
    goto _test_eof;
  _test_eof102:
    cs = 102;
    goto _test_eof;
  _test_eof103:
    cs = 103;
    goto _test_eof;
  _test_eof104:
    cs = 104;
    goto _test_eof;
  _test_eof3:
    cs = 3;
    goto _test_eof;
  _test_eof4:
    cs = 4;
    goto _test_eof;
  _test_eof105:
    cs = 105;
    goto _test_eof;
  _test_eof106:
    cs = 106;
    goto _test_eof;
  _test_eof5:
    cs = 5;
    goto _test_eof;
  _test_eof107:
    cs = 107;
    goto _test_eof;
  _test_eof6:
    cs = 6;
    goto _test_eof;
  _test_eof108:
    cs = 108;
    goto _test_eof;
  _test_eof7:
    cs = 7;
    goto _test_eof;
  _test_eof8:
    cs = 8;
    goto _test_eof;
  _test_eof9:
    cs = 9;
    goto _test_eof;
  _test_eof10:
    cs = 10;
    goto _test_eof;
  _test_eof11:
    cs = 11;
    goto _test_eof;
  _test_eof12:
    cs = 12;
    goto _test_eof;
  _test_eof13:
    cs = 13;
    goto _test_eof;
  _test_eof14:
    cs = 14;
    goto _test_eof;
  _test_eof15:
    cs = 15;
    goto _test_eof;
  _test_eof16:
    cs = 16;
    goto _test_eof;
  _test_eof17:
    cs = 17;
    goto _test_eof;
  _test_eof18:
    cs = 18;
    goto _test_eof;
  _test_eof19:
    cs = 19;
    goto _test_eof;
  _test_eof20:
    cs = 20;
    goto _test_eof;
  _test_eof21:
    cs = 21;
    goto _test_eof;
  _test_eof22:
    cs = 22;
    goto _test_eof;
  _test_eof23:
    cs = 23;
    goto _test_eof;
  _test_eof24:
    cs = 24;
    goto _test_eof;
  _test_eof25:
    cs = 25;
    goto _test_eof;
  _test_eof26:
    cs = 26;
    goto _test_eof;
  _test_eof27:
    cs = 27;
    goto _test_eof;
  _test_eof28:
    cs = 28;
    goto _test_eof;
  _test_eof29:
    cs = 29;
    goto _test_eof;
  _test_eof30:
    cs = 30;
    goto _test_eof;
  _test_eof31:
    cs = 31;
    goto _test_eof;
  _test_eof32:
    cs = 32;
    goto _test_eof;
  _test_eof33:
    cs = 33;
    goto _test_eof;
  _test_eof34:
    cs = 34;
    goto _test_eof;
  _test_eof35:
    cs = 35;
    goto _test_eof;
  _test_eof36:
    cs = 36;
    goto _test_eof;
  _test_eof37:
    cs = 37;
    goto _test_eof;
  _test_eof38:
    cs = 38;
    goto _test_eof;
  _test_eof39:
    cs = 39;
    goto _test_eof;
  _test_eof40:
    cs = 40;
    goto _test_eof;
  _test_eof41:
    cs = 41;
    goto _test_eof;
  _test_eof42:
    cs = 42;
    goto _test_eof;
  _test_eof43:
    cs = 43;
    goto _test_eof;
  _test_eof44:
    cs = 44;
    goto _test_eof;
  _test_eof45:
    cs = 45;
    goto _test_eof;
  _test_eof46:
    cs = 46;
    goto _test_eof;
  _test_eof47:
    cs = 47;
    goto _test_eof;
  _test_eof48:
    cs = 48;
    goto _test_eof;
  _test_eof49:
    cs = 49;
    goto _test_eof;
  _test_eof50:
    cs = 50;
    goto _test_eof;
  _test_eof51:
    cs = 51;
    goto _test_eof;
  _test_eof52:
    cs = 52;
    goto _test_eof;
  _test_eof53:
    cs = 53;
    goto _test_eof;
  _test_eof54:
    cs = 54;
    goto _test_eof;
  _test_eof55:
    cs = 55;
    goto _test_eof;
  _test_eof56:
    cs = 56;
    goto _test_eof;
  _test_eof57:
    cs = 57;
    goto _test_eof;
  _test_eof58:
    cs = 58;
    goto _test_eof;
  _test_eof59:
    cs = 59;
    goto _test_eof;
  _test_eof60:
    cs = 60;
    goto _test_eof;
  _test_eof61:
    cs = 61;
    goto _test_eof;
  _test_eof62:
    cs = 62;
    goto _test_eof;
  _test_eof63:
    cs = 63;
    goto _test_eof;
  _test_eof64:
    cs = 64;
    goto _test_eof;
  _test_eof65:
    cs = 65;
    goto _test_eof;
  _test_eof66:
    cs = 66;
    goto _test_eof;
  _test_eof109:
    cs = 109;
    goto _test_eof;
  _test_eof67:
    cs = 67;
    goto _test_eof;
  _test_eof68:
    cs = 68;
    goto _test_eof;
  _test_eof69:
    cs = 69;
    goto _test_eof;
  _test_eof70:
    cs = 70;
    goto _test_eof;
  _test_eof71:
    cs = 71;
    goto _test_eof;
  _test_eof72:
    cs = 72;
    goto _test_eof;
  _test_eof73:
    cs = 73;
    goto _test_eof;
  _test_eof74:
    cs = 74;
    goto _test_eof;
  _test_eof75:
    cs = 75;
    goto _test_eof;
  _test_eof76:
    cs = 76;
    goto _test_eof;
  _test_eof77:
    cs = 77;
    goto _test_eof;
  _test_eof78:
    cs = 78;
    goto _test_eof;
  _test_eof110:
    cs = 110;
    goto _test_eof;
  _test_eof79:
    cs = 79;
    goto _test_eof;
  _test_eof80:
    cs = 80;
    goto _test_eof;
  _test_eof81:
    cs = 81;
    goto _test_eof;
  _test_eof82:
    cs = 82;
    goto _test_eof;
  _test_eof83:
    cs = 83;
    goto _test_eof;
  _test_eof111:
    cs = 111;
    goto _test_eof;
  _test_eof84:
    cs = 84;
    goto _test_eof;
  _test_eof85:
    cs = 85;
    goto _test_eof;
  _test_eof86:
    cs = 86;
    goto _test_eof;
  _test_eof112:
    cs = 112;
    goto _test_eof;
  _test_eof87:
    cs = 87;
    goto _test_eof;
  _test_eof88:
    cs = 88;
    goto _test_eof;
  _test_eof113:
    cs = 113;
    goto _test_eof;
  _test_eof114:
    cs = 114;
    goto _test_eof;
  _test_eof89:
    cs = 89;
    goto _test_eof;
  _test_eof90:
    cs = 90;
    goto _test_eof;
  _test_eof91:
    cs = 91;
    goto _test_eof;
  _test_eof115:
    cs = 115;
    goto _test_eof;
  _test_eof92:
    cs = 92;
    goto _test_eof;
  _test_eof116:
    cs = 116;
    goto _test_eof;
  _test_eof93:
    cs = 93;
    goto _test_eof;
  _test_eof94:
    cs = 94;
    goto _test_eof;
  _test_eof95:
    cs = 95;
    goto _test_eof;
  _test_eof96:
    cs = 96;
    goto _test_eof;

  _test_eof : {}
    if (p == eof) {
      switch (cs) {
        case 98:
          goto tr133;
        case 0:
          goto tr0;
        case 99:
          goto tr133;
        case 1:
          goto tr0;
        case 100:
          goto tr135;
        case 101:
          goto tr133;
        case 2:
          goto tr0;
        case 102:
          goto tr137;
        case 103:
          goto tr133;
        case 104:
          goto tr133;
        case 3:
          goto tr0;
        case 4:
          goto tr0;
        case 105:
          goto tr140;
        case 106:
          goto tr133;
        case 5:
          goto tr0;
        case 107:
          goto tr141;
        case 6:
          goto tr0;
        case 108:
          goto tr133;
        case 7:
          goto tr0;
        case 8:
          goto tr0;
        case 9:
          goto tr0;
        case 10:
          goto tr0;
        case 11:
          goto tr0;
        case 12:
          goto tr0;
        case 13:
          goto tr0;
        case 14:
          goto tr0;
        case 15:
          goto tr0;
        case 16:
          goto tr0;
        case 17:
          goto tr0;
        case 18:
          goto tr0;
        case 19:
          goto tr0;
        case 20:
          goto tr0;
        case 21:
          goto tr0;
        case 22:
          goto tr0;
        case 23:
          goto tr0;
        case 24:
          goto tr0;
        case 25:
          goto tr0;
        case 26:
          goto tr0;
        case 27:
          goto tr0;
        case 28:
          goto tr0;
        case 29:
          goto tr0;
        case 30:
          goto tr0;
        case 31:
          goto tr0;
        case 32:
          goto tr0;
        case 33:
          goto tr0;
        case 34:
          goto tr0;
        case 35:
          goto tr0;
        case 36:
          goto tr0;
        case 37:
          goto tr0;
        case 38:
          goto tr0;
        case 39:
          goto tr0;
        case 40:
          goto tr0;
        case 41:
          goto tr0;
        case 42:
          goto tr0;
        case 43:
          goto tr0;
        case 44:
          goto tr0;
        case 45:
          goto tr0;
        case 46:
          goto tr0;
        case 47:
          goto tr0;
        case 48:
          goto tr0;
        case 49:
          goto tr0;
        case 50:
          goto tr0;
        case 51:
          goto tr0;
        case 52:
          goto tr0;
        case 53:
          goto tr0;
        case 54:
          goto tr0;
        case 55:
          goto tr0;
        case 56:
          goto tr0;
        case 57:
          goto tr0;
        case 58:
          goto tr0;
        case 59:
          goto tr0;
        case 60:
          goto tr0;
        case 61:
          goto tr0;
        case 62:
          goto tr0;
        case 63:
          goto tr0;
        case 64:
          goto tr0;
        case 65:
          goto tr0;
        case 66:
          goto tr0;
        case 109:
          goto tr133;
        case 67:
          goto tr0;
        case 68:
          goto tr0;
        case 69:
          goto tr0;
        case 70:
          goto tr0;
        case 71:
          goto tr0;
        case 72:
          goto tr0;
        case 73:
          goto tr0;
        case 74:
          goto tr0;
        case 75:
          goto tr0;
        case 76:
          goto tr0;
        case 77:
          goto tr0;
        case 78:
          goto tr0;
        case 110:
          goto tr133;
        case 79:
          goto tr0;
        case 80:
          goto tr0;
        case 81:
          goto tr0;
        case 82:
          goto tr0;
        case 83:
          goto tr0;
        case 111:
          goto tr133;
        case 84:
          goto tr0;
        case 85:
          goto tr0;
        case 86:
          goto tr0;
        case 112:
          goto tr133;
        case 87:
          goto tr0;
        case 88:
          goto tr0;
        case 113:
          goto tr133;
        case 114:
          goto tr133;
        case 89:
          goto tr0;
        case 90:
          goto tr0;
        case 91:
          goto tr0;
        case 115:
          goto tr133;
        case 92:
          goto tr0;
        case 116:
          goto tr133;
        case 93:
          goto tr0;
        case 94:
          goto tr0;
        case 95:
          goto tr0;
        case 96:
          goto tr0;
      }
    }

  _out : {}
  }

#line 260 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

  if (_p == _pe && accepted.get() == nullptr) {
    return nullptr;
  }

  accepted->set_string(GenericAst::StringType(ts, te - ts));
  return accepted;
}

auto GrmLexer::is_eof() const -> bool {
  return _p == _pe;
}

}  // namespace pmt::parserbuilder

#undef MAKE_RL_CONTEXT_AVAILABLE