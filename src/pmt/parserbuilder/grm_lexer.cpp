
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

#define ACCEPT_TOKEN(id)                                       \
  do {                                                         \
    accepted = GenericAst::construct(GenericAst::Tag::String); \
    accepted->set_id(id);                                      \
  } while (0)

#line 189 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

namespace {

#line 25 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
static const int GrmLexer_start = 24;
static const int GrmLexer_first_final = 24;
static const int GrmLexer_error = -1;

static const int GrmLexer_en_main = 24;

#line 194 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

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

#line 48 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
  {
    cs = GrmLexer_start;
    ts = 0;
    te = 0;
    act = 0;
  }

#line 212 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
}

auto GrmLexer::next_token() -> GenericAst::UniqueHandle {
  MAKE_RL_CONTEXT_AVAILABLE;

  GenericAst::UniqueHandle accepted;

#line 61 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
  {
    if (p == pe)
      goto _test_eof;
    switch (cs) {
    tr0:
#line 187 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      { p = ((te)) - 1; }
      { throw std::runtime_error("lexer error"); }
    }
      goto st24;
    tr2:
#line 55 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ++ts;
        --te;
        ACCEPT_TOKEN(GrmAst::TkStringLiteral);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr6:
#line 184 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
    }
      goto st24;
    tr14:
#line 77 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkEpsilon);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr17:
#line 67 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkBooleanLiteral);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr19:
#line 167 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterHide);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr22:
#line 172 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterMerge);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr26:
#line 162 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterUnpack);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr27:
#line 187 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      { throw std::runtime_error("lexer error"); }
    }
      goto st24;
    tr28:
#line 182 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
    }
      goto st24;
    tr31:
#line 107 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenParen);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr32:
#line 112 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseParen);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr33:
#line 152 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkStar);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr34:
#line 147 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkPlus);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr35:
#line 97 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkComma);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr39:
#line 87 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkSemiColon);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr40:
#line 137 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenAngle);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr41:
#line 92 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkEquals);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr42:
#line 142 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseAngle);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr43:
#line 157 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkQuestion);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr44:
#line 127 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenSquare);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr45:
#line 132 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseSquare);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr53:
#line 117 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkOpenBrace);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr54:
#line 82 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkPipe);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr55:
#line 122 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkCloseBrace);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr56:
#line 187 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      { throw std::runtime_error("lexer error"); }
    }
      goto st24;
    tr58:
#line 72 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        ACCEPT_TOKEN(GrmAst::TkTerminalIdentifier);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr59:
#line 102 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkDoubleDot);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr61:
#line 183 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
    }
      goto st24;
    tr62:
#line 62 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        ACCEPT_TOKEN(GrmAst::TkIntegerLiteral);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    tr66:
#line 177 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ACCEPT_TOKEN(GrmAst::TkKwParameterId);
        {
          p++;
          cs = 24;
          goto _out;
        }
      }
    }
      goto st24;
    st24:
#line 1 "NONE"
    {
      ts = 0;
    }
      if (++p == pe)
        goto _test_eof24;
      case 24:
#line 1 "NONE"
      {
        ts = p;
      }
#line 242 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        switch ((*p)) {
          case 32:
            goto tr28;
          case 34:
            goto tr29;
          case 36:
            goto tr30;
          case 40:
            goto tr31;
          case 41:
            goto tr32;
          case 42:
            goto tr33;
          case 43:
            goto tr34;
          case 44:
            goto tr35;
          case 46:
            goto st28;
          case 47:
            goto tr37;
          case 59:
            goto tr39;
          case 60:
            goto tr40;
          case 61:
            goto tr41;
          case 62:
            goto tr42;
          case 63:
            goto tr43;
          case 91:
            goto tr44;
          case 93:
            goto tr45;
          case 101:
            goto tr46;
          case 102:
            goto tr47;
          case 104:
            goto tr48;
          case 105:
            goto st36;
          case 109:
            goto tr50;
          case 116:
            goto tr51;
          case 117:
            goto tr52;
          case 123:
            goto tr53;
          case 124:
            goto tr54;
          case 125:
            goto tr55;
        }
        if ((*p) > 13) {
          if (48 <= (*p) && (*p) <= 57)
            goto tr38;
        } else if ((*p) >= 9)
          goto tr28;
        goto tr27;
      tr29:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st25;
      st25:
        if (++p == pe)
          goto _test_eof25;
      case 25:
#line 284 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 34)
          goto tr2;
        if ((*p) > 38) {
          if (40 <= (*p) && (*p) <= 126)
            goto st0;
        } else if ((*p) >= 32)
          goto st0;
        goto tr56;
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
      tr30:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st26;
      st26:
        if (++p == pe)
          goto _test_eof26;
      case 26:
#line 311 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 95)
          goto st1;
        if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st1;
        } else if ((*p) >= 65)
          goto st1;
        goto tr56;
      st1:
        if (++p == pe)
          goto _test_eof1;
      case 1:
        if ((*p) == 95)
          goto st27;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st27;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st27;
        } else
          goto st27;
        goto tr0;
      st27:
        if (++p == pe)
          goto _test_eof27;
      case 27:
        if ((*p) == 95)
          goto st27;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st27;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st27;
        } else
          goto st27;
        goto tr58;
      st28:
        if (++p == pe)
          goto _test_eof28;
      case 28:
        if ((*p) == 46)
          goto tr59;
        goto tr56;
      tr37:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st29;
      st29:
        if (++p == pe)
          goto _test_eof29;
      case 29:
#line 363 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        switch ((*p)) {
          case 42:
            goto st2;
          case 47:
            goto st30;
        }
        goto tr56;
      st2:
        if (++p == pe)
          goto _test_eof2;
      case 2:
        if ((*p) == 42)
          goto st3;
        goto st2;
      st3:
        if (++p == pe)
          goto _test_eof3;
      case 3:
        if ((*p) == 47)
          goto tr6;
        goto st2;
      st30:
        if (++p == pe)
          goto _test_eof30;
      case 30:
        if ((*p) == 10)
          goto tr61;
        goto st30;
      tr38:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st31;
      st31:
        if (++p == pe)
          goto _test_eof31;
      case 31:
#line 396 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 35)
          goto st4;
        if (48 <= (*p) && (*p) <= 57)
          goto st5;
        goto tr56;
      st4:
        if (++p == pe)
          goto _test_eof4;
      case 4:
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st32;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st32;
        } else
          goto st32;
        goto tr0;
      st32:
        if (++p == pe)
          goto _test_eof32;
      case 32:
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st32;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st32;
        } else
          goto st32;
        goto tr62;
      st5:
        if (++p == pe)
          goto _test_eof5;
      case 5:
        if ((*p) == 35)
          goto st4;
        if (48 <= (*p) && (*p) <= 57)
          goto st5;
        goto tr0;
      tr46:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st33;
      st33:
        if (++p == pe)
          goto _test_eof33;
      case 33:
#line 443 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 112)
          goto st6;
        goto tr56;
      st6:
        if (++p == pe)
          goto _test_eof6;
      case 6:
        if ((*p) == 115)
          goto st7;
        goto tr0;
      st7:
        if (++p == pe)
          goto _test_eof7;
      case 7:
        if ((*p) == 105)
          goto st8;
        goto tr0;
      st8:
        if (++p == pe)
          goto _test_eof8;
      case 8:
        if ((*p) == 108)
          goto st9;
        goto tr0;
      st9:
        if (++p == pe)
          goto _test_eof9;
      case 9:
        if ((*p) == 111)
          goto st10;
        goto tr0;
      st10:
        if (++p == pe)
          goto _test_eof10;
      case 10:
        if ((*p) == 110)
          goto tr14;
        goto tr0;
      tr47:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st34;
      st34:
        if (++p == pe)
          goto _test_eof34;
      case 34:
#line 488 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 97)
          goto st11;
        goto tr56;
      st11:
        if (++p == pe)
          goto _test_eof11;
      case 11:
        if ((*p) == 108)
          goto st12;
        goto tr0;
      st12:
        if (++p == pe)
          goto _test_eof12;
      case 12:
        if ((*p) == 115)
          goto st13;
        goto tr0;
      st13:
        if (++p == pe)
          goto _test_eof13;
      case 13:
        if ((*p) == 101)
          goto tr17;
        goto tr0;
      tr48:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st35;
      st35:
        if (++p == pe)
          goto _test_eof35;
      case 35:
#line 519 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 105)
          goto st14;
        goto tr56;
      st14:
        if (++p == pe)
          goto _test_eof14;
      case 14:
        if ((*p) == 100)
          goto st15;
        goto tr0;
      st15:
        if (++p == pe)
          goto _test_eof15;
      case 15:
        if ((*p) == 101)
          goto tr19;
        goto tr0;
      st36:
        if (++p == pe)
          goto _test_eof36;
      case 36:
        if ((*p) == 100)
          goto tr66;
        goto tr56;
      tr50:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st37;
      st37:
        if (++p == pe)
          goto _test_eof37;
      case 37:
#line 550 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 101)
          goto st16;
        goto tr56;
      st16:
        if (++p == pe)
          goto _test_eof16;
      case 16:
        if ((*p) == 114)
          goto st17;
        goto tr0;
      st17:
        if (++p == pe)
          goto _test_eof17;
      case 17:
        if ((*p) == 103)
          goto st18;
        goto tr0;
      st18:
        if (++p == pe)
          goto _test_eof18;
      case 18:
        if ((*p) == 101)
          goto tr22;
        goto tr0;
      tr51:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st38;
      st38:
        if (++p == pe)
          goto _test_eof38;
      case 38:
#line 581 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 114)
          goto st19;
        goto tr56;
      st19:
        if (++p == pe)
          goto _test_eof19;
      case 19:
        if ((*p) == 117)
          goto st13;
        goto tr0;
      tr52:
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st39;
      st39:
        if (++p == pe)
          goto _test_eof39;
      case 39:
#line 598 "/home/pmt/repos/pmt/src/pmt/parserbuilder/grm_lexer.cpp"
        if ((*p) == 110)
          goto st20;
        goto tr56;
      st20:
        if (++p == pe)
          goto _test_eof20;
      case 20:
        if ((*p) == 112)
          goto st21;
        goto tr0;
      st21:
        if (++p == pe)
          goto _test_eof21;
      case 21:
        if ((*p) == 97)
          goto st22;
        goto tr0;
      st22:
        if (++p == pe)
          goto _test_eof22;
      case 22:
        if ((*p) == 99)
          goto st23;
        goto tr0;
      st23:
        if (++p == pe)
          goto _test_eof23;
      case 23:
        if ((*p) == 107)
          goto tr26;
        goto tr0;
    }
  _test_eof24:
    cs = 24;
    goto _test_eof;
  _test_eof25:
    cs = 25;
    goto _test_eof;
  _test_eof0:
    cs = 0;
    goto _test_eof;
  _test_eof26:
    cs = 26;
    goto _test_eof;
  _test_eof1:
    cs = 1;
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
  _test_eof2:
    cs = 2;
    goto _test_eof;
  _test_eof3:
    cs = 3;
    goto _test_eof;
  _test_eof30:
    cs = 30;
    goto _test_eof;
  _test_eof31:
    cs = 31;
    goto _test_eof;
  _test_eof4:
    cs = 4;
    goto _test_eof;
  _test_eof32:
    cs = 32;
    goto _test_eof;
  _test_eof5:
    cs = 5;
    goto _test_eof;
  _test_eof33:
    cs = 33;
    goto _test_eof;
  _test_eof6:
    cs = 6;
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
  _test_eof34:
    cs = 34;
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
  _test_eof35:
    cs = 35;
    goto _test_eof;
  _test_eof14:
    cs = 14;
    goto _test_eof;
  _test_eof15:
    cs = 15;
    goto _test_eof;
  _test_eof36:
    cs = 36;
    goto _test_eof;
  _test_eof37:
    cs = 37;
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
  _test_eof38:
    cs = 38;
    goto _test_eof;
  _test_eof19:
    cs = 19;
    goto _test_eof;
  _test_eof39:
    cs = 39;
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

  _test_eof: {}
    if (p == eof) {
      switch (cs) {
        case 25:
          goto tr56;
        case 0:
          goto tr0;
        case 26:
          goto tr56;
        case 1:
          goto tr0;
        case 27:
          goto tr58;
        case 28:
          goto tr56;
        case 29:
          goto tr56;
        case 2:
          goto tr0;
        case 3:
          goto tr0;
        case 30:
          goto tr61;
        case 31:
          goto tr56;
        case 4:
          goto tr0;
        case 32:
          goto tr62;
        case 5:
          goto tr0;
        case 33:
          goto tr56;
        case 6:
          goto tr0;
        case 7:
          goto tr0;
        case 8:
          goto tr0;
        case 9:
          goto tr0;
        case 10:
          goto tr0;
        case 34:
          goto tr56;
        case 11:
          goto tr0;
        case 12:
          goto tr0;
        case 13:
          goto tr0;
        case 35:
          goto tr56;
        case 14:
          goto tr0;
        case 15:
          goto tr0;
        case 36:
          goto tr56;
        case 37:
          goto tr56;
        case 16:
          goto tr0;
        case 17:
          goto tr0;
        case 18:
          goto tr0;
        case 38:
          goto tr56;
        case 19:
          goto tr0;
        case 39:
          goto tr56;
        case 20:
          goto tr0;
        case 21:
          goto tr0;
        case 22:
          goto tr0;
        case 23:
          goto tr0;
      }
    }

  _out: {}
  }

#line 220 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

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