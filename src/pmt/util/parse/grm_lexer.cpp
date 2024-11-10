
#line 1 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
#include "pmt/util/parse/grm_lexer.hpp"

#include "pmt/util/parse/grm_ast.hpp"

#define MAKE_RL_CONTEXT_AVAILABLE \
  char const *&p = _p;            \
  char const *&pe = _pe;          \
  char const *&eof = _eof;        \
  char const *&ts = _ts;          \
  char const *&te = _te;          \
  int &act = _act;                \
  int &cs = _cs;

#line 170 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

namespace {

#line 19 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
static const int GrmLexer_start = 25;
static const int GrmLexer_first_final = 25;
static const int GrmLexer_error = -1;

static const int GrmLexer_en_main = 25;

#line 175 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

}  // namespace

namespace pmt::util::parse {

GrmLexer::GrmLexer(std::string_view input_)
 : _p{input_.data()}
 , _pe{_p + input_.size()}
 , _eof{_pe}
 , _ts{nullptr}
 , _te{nullptr}
 , _act{0}
 , _cs{0} {
  MAKE_RL_CONTEXT_AVAILABLE;

#line 41 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
  {
    cs = GrmLexer_start;
    ts = 0;
    te = 0;
    act = 0;
  }

#line 192 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
}

auto GrmLexer::next_token() -> GenericAst::UniqueHandle {
  MAKE_RL_CONTEXT_AVAILABLE;

  GenericAst::UniqueHandle accepted = GenericAst::construct(GenericAst::Tag::Token);

#line 54 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
  {
    if (p == pe)
      goto _test_eof;
    switch (cs) {
    tr0 :
#line 168 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      { p = ((te)) - 1; }
      { throw std::runtime_error("lexer error"); }
    }
      goto st25;
    tr2 :
#line 46 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ++ts;
        --te;
        accepted->set_id(GrmAst::TkStringLiteral);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr7 :
#line 165 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
    }
      goto st25;
    tr15 :
#line 73 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkEpsilon);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr18 :
#line 58 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkBooleanLiteral);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr20 :
#line 153 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkKwParameterHide);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr23 :
#line 158 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkKwParameterMerge);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr27 :
#line 148 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkKwParameterUnpack);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr28 :
#line 168 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      { throw std::runtime_error("lexer error"); }
    }
      goto st25;
    tr29 :
#line 163 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
    }
      goto st25;
    tr33 :
#line 103 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkOpenParen);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr34 :
#line 108 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkCloseParen);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr35 :
#line 138 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkStar);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr36 :
#line 133 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkPlus);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr37 :
#line 93 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkComma);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr41 :
#line 83 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkSemiColon);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr42 :
#line 88 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkEquals);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr43 :
#line 143 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkQuestion);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr44 :
#line 123 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkOpenSquare);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr45 :
#line 128 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkCloseSquare);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr52 :
#line 113 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkOpenBrace);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr53 :
#line 78 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkPipe);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr54 :
#line 118 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkCloseBrace);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr55 :
#line 168 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      { throw std::runtime_error("lexer error"); }
    }
      goto st25;
    tr57 :
#line 63 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        accepted->set_id(GrmAst::TkTerminalIdentifier);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr59 :
#line 68 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        accepted->set_id(GrmAst::TkRuleIdentifier);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr60 :
#line 98 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmAst::TkDoubleDot);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    tr62 :
#line 164 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
    }
      goto st25;
    tr63 :
#line 53 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        accepted->set_id(GrmAst::TkIntegerLiteral);
        {
          p++;
          cs = 25;
          goto _out;
        }
      }
    }
      goto st25;
    st25 :
#line 1 "NONE"
    {
      ts = 0;
    }
      if (++p == pe)
        goto _test_eof25;
      case 25:
#line 1 "NONE"
      {
        ts = p;
      }
#line 223 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        switch ((*p)) {
          case 32:
            goto tr29;
          case 34:
            goto tr30;
          case 36:
            goto tr31;
          case 37:
            goto tr32;
          case 40:
            goto tr33;
          case 41:
            goto tr34;
          case 42:
            goto tr35;
          case 43:
            goto tr36;
          case 44:
            goto tr37;
          case 46:
            goto st31;
          case 47:
            goto tr39;
          case 59:
            goto tr41;
          case 61:
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
          case 109:
            goto tr49;
          case 116:
            goto tr50;
          case 117:
            goto tr51;
          case 123:
            goto tr52;
          case 124:
            goto tr53;
          case 125:
            goto tr54;
        }
        if ((*p) > 13) {
          if (48 <= (*p) && (*p) <= 57)
            goto tr40;
        } else if ((*p) >= 9)
          goto tr29;
        goto tr28;
      tr30 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st26;
      st26:
        if (++p == pe)
          goto _test_eof26;
      case 26:
#line 263 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 34)
          goto tr2;
        if ((*p) > 38) {
          if (40 <= (*p) && (*p) <= 126)
            goto st0;
        } else if ((*p) >= 32)
          goto st0;
        goto tr55;
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
      tr31 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st27;
      st27:
        if (++p == pe)
          goto _test_eof27;
      case 27:
#line 290 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st1;
        } else if ((*p) >= 65)
          goto st1;
        goto tr55;
      st1:
        if (++p == pe)
          goto _test_eof1;
      case 1:
        if ((*p) == 95)
          goto st28;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st28;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st28;
        } else
          goto st28;
        goto tr0;
      st28:
        if (++p == pe)
          goto _test_eof28;
      case 28:
        if ((*p) == 95)
          goto st28;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st28;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st28;
        } else
          goto st28;
        goto tr57;
      tr32 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st29;
      st29:
        if (++p == pe)
          goto _test_eof29;
      case 29:
#line 333 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st2;
        } else if ((*p) >= 65)
          goto st2;
        goto tr55;
      st2:
        if (++p == pe)
          goto _test_eof2;
      case 2:
        if ((*p) == 95)
          goto st30;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st30;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st30;
        } else
          goto st30;
        goto tr0;
      st30:
        if (++p == pe)
          goto _test_eof30;
      case 30:
        if ((*p) == 95)
          goto st30;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st30;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st30;
        } else
          goto st30;
        goto tr59;
      st31:
        if (++p == pe)
          goto _test_eof31;
      case 31:
        if ((*p) == 46)
          goto tr60;
        goto tr55;
      tr39 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st32;
      st32:
        if (++p == pe)
          goto _test_eof32;
      case 32:
#line 383 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        switch ((*p)) {
          case 42:
            goto st3;
          case 47:
            goto st33;
        }
        goto tr55;
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
      st33:
        if (++p == pe)
          goto _test_eof33;
      case 33:
        if ((*p) == 10)
          goto tr62;
        goto st33;
      tr40 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st34;
      st34:
        if (++p == pe)
          goto _test_eof34;
      case 34:
#line 416 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 35)
          goto st5;
        if (48 <= (*p) && (*p) <= 57)
          goto st6;
        goto tr55;
      st5:
        if (++p == pe)
          goto _test_eof5;
      case 5:
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st35;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st35;
        } else
          goto st35;
        goto tr0;
      st35:
        if (++p == pe)
          goto _test_eof35;
      case 35:
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st35;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st35;
        } else
          goto st35;
        goto tr63;
      st6:
        if (++p == pe)
          goto _test_eof6;
      case 6:
        if ((*p) == 35)
          goto st5;
        if (48 <= (*p) && (*p) <= 57)
          goto st6;
        goto tr0;
      tr46 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st36;
      st36:
        if (++p == pe)
          goto _test_eof36;
      case 36:
#line 463 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 112)
          goto st7;
        goto tr55;
      st7:
        if (++p == pe)
          goto _test_eof7;
      case 7:
        if ((*p) == 115)
          goto st8;
        goto tr0;
      st8:
        if (++p == pe)
          goto _test_eof8;
      case 8:
        if ((*p) == 105)
          goto st9;
        goto tr0;
      st9:
        if (++p == pe)
          goto _test_eof9;
      case 9:
        if ((*p) == 108)
          goto st10;
        goto tr0;
      st10:
        if (++p == pe)
          goto _test_eof10;
      case 10:
        if ((*p) == 111)
          goto st11;
        goto tr0;
      st11:
        if (++p == pe)
          goto _test_eof11;
      case 11:
        if ((*p) == 110)
          goto tr15;
        goto tr0;
      tr47 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st37;
      st37:
        if (++p == pe)
          goto _test_eof37;
      case 37:
#line 508 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 97)
          goto st12;
        goto tr55;
      st12:
        if (++p == pe)
          goto _test_eof12;
      case 12:
        if ((*p) == 108)
          goto st13;
        goto tr0;
      st13:
        if (++p == pe)
          goto _test_eof13;
      case 13:
        if ((*p) == 115)
          goto st14;
        goto tr0;
      st14:
        if (++p == pe)
          goto _test_eof14;
      case 14:
        if ((*p) == 101)
          goto tr18;
        goto tr0;
      tr48 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st38;
      st38:
        if (++p == pe)
          goto _test_eof38;
      case 38:
#line 539 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 105)
          goto st15;
        goto tr55;
      st15:
        if (++p == pe)
          goto _test_eof15;
      case 15:
        if ((*p) == 100)
          goto st16;
        goto tr0;
      st16:
        if (++p == pe)
          goto _test_eof16;
      case 16:
        if ((*p) == 101)
          goto tr20;
        goto tr0;
      tr49 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st39;
      st39:
        if (++p == pe)
          goto _test_eof39;
      case 39:
#line 563 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 101)
          goto st17;
        goto tr55;
      st17:
        if (++p == pe)
          goto _test_eof17;
      case 17:
        if ((*p) == 114)
          goto st18;
        goto tr0;
      st18:
        if (++p == pe)
          goto _test_eof18;
      case 18:
        if ((*p) == 103)
          goto st19;
        goto tr0;
      st19:
        if (++p == pe)
          goto _test_eof19;
      case 19:
        if ((*p) == 101)
          goto tr23;
        goto tr0;
      tr50 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st40;
      st40:
        if (++p == pe)
          goto _test_eof40;
      case 40:
#line 594 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 114)
          goto st20;
        goto tr55;
      st20:
        if (++p == pe)
          goto _test_eof20;
      case 20:
        if ((*p) == 117)
          goto st14;
        goto tr0;
      tr51 :
#line 1 "NONE"
      {
        te = p + 1;
      }
        goto st41;
      st41:
        if (++p == pe)
          goto _test_eof41;
      case 41:
#line 611 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        if ((*p) == 110)
          goto st21;
        goto tr55;
      st21:
        if (++p == pe)
          goto _test_eof21;
      case 21:
        if ((*p) == 112)
          goto st22;
        goto tr0;
      st22:
        if (++p == pe)
          goto _test_eof22;
      case 22:
        if ((*p) == 97)
          goto st23;
        goto tr0;
      st23:
        if (++p == pe)
          goto _test_eof23;
      case 23:
        if ((*p) == 99)
          goto st24;
        goto tr0;
      st24:
        if (++p == pe)
          goto _test_eof24;
      case 24:
        if ((*p) == 107)
          goto tr27;
        goto tr0;
    }
  _test_eof25:
    cs = 25;
    goto _test_eof;
  _test_eof26:
    cs = 26;
    goto _test_eof;
  _test_eof0:
    cs = 0;
    goto _test_eof;
  _test_eof27:
    cs = 27;
    goto _test_eof;
  _test_eof1:
    cs = 1;
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
  _test_eof30:
    cs = 30;
    goto _test_eof;
  _test_eof31:
    cs = 31;
    goto _test_eof;
  _test_eof32:
    cs = 32;
    goto _test_eof;
  _test_eof3:
    cs = 3;
    goto _test_eof;
  _test_eof4:
    cs = 4;
    goto _test_eof;
  _test_eof33:
    cs = 33;
    goto _test_eof;
  _test_eof34:
    cs = 34;
    goto _test_eof;
  _test_eof5:
    cs = 5;
    goto _test_eof;
  _test_eof35:
    cs = 35;
    goto _test_eof;
  _test_eof6:
    cs = 6;
    goto _test_eof;
  _test_eof36:
    cs = 36;
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
  _test_eof37:
    cs = 37;
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
  _test_eof38:
    cs = 38;
    goto _test_eof;
  _test_eof15:
    cs = 15;
    goto _test_eof;
  _test_eof16:
    cs = 16;
    goto _test_eof;
  _test_eof39:
    cs = 39;
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
  _test_eof40:
    cs = 40;
    goto _test_eof;
  _test_eof20:
    cs = 20;
    goto _test_eof;
  _test_eof41:
    cs = 41;
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

  _test_eof : {}
    if (p == eof) {
      switch (cs) {
        case 26:
          goto tr55;
        case 0:
          goto tr0;
        case 27:
          goto tr55;
        case 1:
          goto tr0;
        case 28:
          goto tr57;
        case 29:
          goto tr55;
        case 2:
          goto tr0;
        case 30:
          goto tr59;
        case 31:
          goto tr55;
        case 32:
          goto tr55;
        case 3:
          goto tr0;
        case 4:
          goto tr0;
        case 33:
          goto tr62;
        case 34:
          goto tr55;
        case 5:
          goto tr0;
        case 35:
          goto tr63;
        case 6:
          goto tr0;
        case 36:
          goto tr55;
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
        case 37:
          goto tr55;
        case 12:
          goto tr0;
        case 13:
          goto tr0;
        case 14:
          goto tr0;
        case 38:
          goto tr55;
        case 15:
          goto tr0;
        case 16:
          goto tr0;
        case 39:
          goto tr55;
        case 17:
          goto tr0;
        case 18:
          goto tr0;
        case 19:
          goto tr0;
        case 40:
          goto tr55;
        case 20:
          goto tr0;
        case 41:
          goto tr55;
        case 21:
          goto tr0;
        case 22:
          goto tr0;
        case 23:
          goto tr0;
        case 24:
          goto tr0;
      }
    }

  _out : {}
  }

#line 200 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

  if (_p == _pe) {
    return nullptr;
  }

  accepted->set_token(GenericAst::TokenType(ts, te - ts));
  return accepted;
}

auto GrmLexer::is_eof() const -> bool {
  return _p == _pe;
}

}  // namespace pmt::util::parse

#undef MAKE_RL_CONTEXT_AVAILABLE