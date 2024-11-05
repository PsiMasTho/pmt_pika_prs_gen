
#line 1 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
#include "pmt/util/parse/grm_lexer.hpp"

#include "pmt/util/parse/grm_id.hpp"

#define MAKE_RL_CONTEXT_AVAILABLE \
  char const *&p = _p;            \
  char const *&pe = _pe;          \
  char const *&eof = _eof;        \
  char const *&ts = _ts;          \
  char const *&te = _te;          \
  int &act = _act;                \
  int &cs = _cs;

#line 54 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

namespace {

#line 19 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
static const int GrmLexer_start = 4;
static const int GrmLexer_first_final = 4;
static const int GrmLexer_error = 0;

static const int GrmLexer_en_main = 4;

#line 59 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

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

#line 76 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
}

auto GrmLexer::next_token() -> GenericAst::UniqueHandle {
  MAKE_RL_CONTEXT_AVAILABLE;

  GenericAst::UniqueHandle accepted = GenericAst::construct(GenericAst::Tag::Token);

#line 54 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
  {
    if (p == pe)
      goto _test_eof;
    switch (cs) {
    tr1 :
#line 25 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        ++ts;
        --te;
        accepted->set_id(GrmId::TkStringLiteral);
        {
          p++;
          cs = 4;
          goto _out;
        }
      }
    }
      goto st4;
    tr5 :
#line 52 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
    }
      goto st4;
    tr6 :
#line 42 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmId::TkSemiColon);
        {
          p++;
          cs = 4;
          goto _out;
        }
      }
    }
      goto st4;
    tr7 :
#line 47 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmId::TkEquals);
        {
          p++;
          cs = 4;
          goto _out;
        }
      }
    }
      goto st4;
    tr9 :
#line 37 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p + 1;
      {
        accepted->set_id(GrmId::TkPipe);
        {
          p++;
          cs = 4;
          goto _out;
        }
      }
    }
      goto st4;
    tr10 :
#line 32 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"
    {
      te = p;
      p--;
      {
        accepted->set_id(GrmId::TkIdentifier);
        {
          p++;
          cs = 4;
          goto _out;
        }
      }
    }
      goto st4;
    st4 :
#line 1 "NONE"
    {
      ts = 0;
    }
      if (++p == pe)
        goto _test_eof4;
      case 4:
#line 1 "NONE"
      {
        ts = p;
      }
#line 100 "/home/pmt/repos/pmt/src/pmt/util/parse/grm_lexer.cpp"
        switch ((*p)) {
          case 32:
            goto tr5;
          case 34:
            goto st1;
          case 59:
            goto tr6;
          case 61:
            goto tr7;
          case 124:
            goto tr9;
        }
        if ((*p) < 65) {
          if (9 <= (*p) && (*p) <= 13)
            goto tr5;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st3;
        } else
          goto st3;
        goto st0;
      st0:
        cs = 0;
        goto _out;
      st1:
        if (++p == pe)
          goto _test_eof1;
      case 1:
        switch ((*p)) {
          case 34:
            goto tr1;
          case 92:
            goto st2;
        }
        goto st1;
      st2:
        if (++p == pe)
          goto _test_eof2;
      case 2:
        goto st1;
      st3:
        if (++p == pe)
          goto _test_eof3;
      case 3:
        if ((*p) == 95)
          goto st5;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st5;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st5;
        } else
          goto st5;
        goto st0;
      st5:
        if (++p == pe)
          goto _test_eof5;
      case 5:
        if ((*p) == 95)
          goto st5;
        if ((*p) < 65) {
          if (48 <= (*p) && (*p) <= 57)
            goto st5;
        } else if ((*p) > 90) {
          if (97 <= (*p) && (*p) <= 122)
            goto st5;
        } else
          goto st5;
        goto tr10;
    }
  _test_eof4:
    cs = 4;
    goto _test_eof;
  _test_eof1:
    cs = 1;
    goto _test_eof;
  _test_eof2:
    cs = 2;
    goto _test_eof;
  _test_eof3:
    cs = 3;
    goto _test_eof;
  _test_eof5:
    cs = 5;
    goto _test_eof;

  _test_eof : {}
    if (p == eof) {
      switch (cs) {
        case 5:
          goto tr10;
      }
    }

  _out : {}
  }

#line 84 "/home/pmt/repos/pmt/grammars/grm_lexer.rl"

  accepted->set_token(GenericAst::TokenType(ts, te - ts));
  return accepted;
}

auto GrmLexer::is_eof() const -> bool {
  return _p == _pe;
}

}  // namespace pmt::util::parse

#undef MAKE_RL_CONTEXT_AVAILABLE