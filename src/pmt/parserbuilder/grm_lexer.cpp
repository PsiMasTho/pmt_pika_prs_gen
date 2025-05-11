
#line 1 "grm_lexer.rl"
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


#line 236 "grm_lexer.rl"


namespace {


#line 24 "grm_lexer.cpp"
static const char _GrmLexer_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 16, 1, 17, 1, 18, 1, 
	19, 1, 20, 1, 21, 1, 22, 1, 
	23, 1, 24, 1, 25, 1, 26, 1, 
	27, 1, 28, 1, 29, 1, 30, 1, 
	31, 1, 32, 1, 33, 1, 34, 1, 
	35, 1, 36, 1, 37, 1, 38, 1, 
	39, 1, 40
};

static const unsigned char _GrmLexer_key_offsets[] = {
	0, 5, 12, 19, 20, 21, 27, 30, 
	32, 33, 34, 35, 36, 37, 38, 39, 
	40, 41, 42, 43, 44, 45, 46, 47, 
	48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 58, 59, 60, 61, 62, 63, 
	64, 65, 66, 67, 68, 69, 70, 71, 
	72, 73, 74, 75, 76, 77, 78, 79, 
	80, 81, 82, 83, 84, 85, 86, 87, 
	88, 89, 90, 91, 92, 93, 94, 95, 
	96, 97, 98, 132, 137, 142, 149, 154, 
	161, 162, 164, 165, 168, 174, 178, 179, 
	180, 181, 182, 183, 184, 185
};

static const char _GrmLexer_trans_keys[] = {
	34, 32, 38, 40, 126, 95, 48, 57, 
	65, 90, 97, 122, 95, 48, 57, 65, 
	90, 97, 122, 42, 47, 48, 57, 65, 
	90, 97, 122, 35, 48, 57, 97, 111, 
	115, 101, 95, 115, 101, 110, 115, 105, 
	116, 105, 118, 101, 109, 109, 101, 110, 
	116, 101, 119, 108, 105, 110, 101, 116, 
	97, 114, 116, 104, 105, 116, 101, 115, 
	112, 97, 99, 101, 115, 101, 95, 115, 
	101, 110, 115, 105, 116, 105, 118, 101, 
	115, 105, 108, 111, 110, 108, 115, 101, 
	100, 101, 114, 103, 101, 117, 112, 97, 
	99, 107, 32, 34, 36, 37, 40, 41, 
	42, 43, 44, 46, 47, 59, 60, 61, 
	62, 63, 64, 91, 93, 99, 101, 102, 
	104, 105, 109, 116, 117, 123, 124, 125, 
	9, 13, 48, 57, 34, 32, 38, 40, 
	126, 95, 65, 90, 97, 122, 95, 48, 
	57, 65, 90, 97, 122, 95, 65, 90, 
	97, 122, 95, 48, 57, 65, 90, 97, 
	122, 46, 42, 47, 10, 35, 48, 57, 
	48, 57, 65, 90, 97, 122, 99, 110, 
	115, 119, 97, 112, 97, 105, 100, 101, 
	114, 110, 0
};

static const char _GrmLexer_single_lengths[] = {
	1, 1, 1, 1, 1, 0, 1, 2, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 30, 1, 1, 1, 1, 1, 
	1, 2, 1, 1, 0, 4, 1, 1, 
	1, 1, 1, 1, 1, 1
};

static const char _GrmLexer_range_lengths[] = {
	2, 3, 3, 0, 0, 3, 1, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 2, 2, 2, 3, 2, 3, 
	0, 0, 0, 1, 3, 0, 0, 0, 
	0, 0, 0, 0, 0, 0
};

static const short _GrmLexer_index_offsets[] = {
	0, 4, 9, 14, 16, 18, 22, 25, 
	28, 30, 32, 34, 36, 38, 40, 42, 
	44, 46, 48, 50, 52, 54, 56, 58, 
	60, 62, 64, 66, 68, 70, 72, 74, 
	76, 78, 80, 82, 84, 86, 88, 90, 
	92, 94, 96, 98, 100, 102, 104, 106, 
	108, 110, 112, 114, 116, 118, 120, 122, 
	124, 126, 128, 130, 132, 134, 136, 138, 
	140, 142, 144, 146, 148, 150, 152, 154, 
	156, 158, 160, 193, 197, 201, 206, 210, 
	215, 217, 220, 222, 225, 229, 234, 236, 
	238, 240, 242, 244, 246, 248
};

static const char _GrmLexer_trans_targs[] = {
	74, 0, 0, 74, 77, 77, 77, 77, 
	74, 79, 79, 79, 79, 74, 4, 3, 
	74, 3, 84, 84, 84, 74, 5, 6, 
	74, 8, 20, 74, 9, 74, 10, 74, 
	11, 74, 12, 74, 13, 74, 14, 74, 
	15, 74, 16, 74, 17, 74, 18, 74, 
	19, 74, 74, 74, 21, 74, 22, 74, 
	23, 74, 24, 74, 74, 74, 26, 74, 
	27, 74, 28, 74, 29, 74, 30, 74, 
	74, 74, 32, 74, 33, 74, 34, 74, 
	74, 74, 36, 74, 37, 74, 38, 74, 
	39, 74, 40, 74, 41, 74, 42, 74, 
	43, 74, 74, 74, 45, 74, 46, 74, 
	47, 74, 48, 74, 49, 74, 50, 74, 
	51, 74, 52, 74, 53, 74, 54, 74, 
	55, 74, 74, 74, 57, 74, 58, 74, 
	59, 74, 60, 74, 74, 74, 62, 74, 
	63, 74, 74, 74, 65, 74, 74, 74, 
	67, 74, 68, 74, 74, 74, 63, 74, 
	71, 74, 72, 74, 73, 74, 74, 74, 
	74, 75, 76, 78, 74, 74, 74, 74, 
	74, 80, 81, 74, 74, 74, 74, 74, 
	85, 74, 74, 86, 87, 88, 89, 90, 
	91, 92, 93, 74, 74, 74, 74, 83, 
	74, 74, 0, 0, 74, 1, 1, 1, 
	74, 77, 77, 77, 77, 74, 2, 2, 
	2, 74, 79, 79, 79, 79, 74, 74, 
	74, 3, 82, 74, 74, 82, 5, 6, 
	74, 84, 84, 84, 74, 7, 25, 31, 
	35, 74, 44, 74, 56, 74, 61, 74, 
	64, 74, 74, 74, 66, 74, 69, 74, 
	70, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 74, 
	74, 74, 74, 74, 74, 74, 74, 0
};

static const char _GrmLexer_trans_actions[] = {
	7, 0, 0, 81, 0, 0, 0, 0, 
	81, 0, 0, 0, 0, 81, 0, 0, 
	67, 0, 0, 0, 0, 81, 0, 0, 
	81, 0, 0, 81, 0, 81, 0, 81, 
	0, 81, 0, 81, 0, 81, 0, 81, 
	0, 81, 0, 81, 0, 81, 0, 81, 
	0, 81, 63, 81, 0, 81, 0, 81, 
	0, 81, 0, 81, 59, 81, 0, 81, 
	0, 81, 0, 81, 0, 81, 0, 81, 
	61, 81, 0, 81, 0, 81, 0, 81, 
	55, 81, 0, 81, 0, 81, 0, 81, 
	0, 81, 0, 81, 0, 81, 0, 81, 
	0, 81, 57, 81, 0, 81, 0, 81, 
	0, 81, 0, 81, 0, 81, 0, 81, 
	0, 81, 0, 81, 0, 81, 0, 81, 
	0, 81, 53, 81, 0, 81, 0, 81, 
	0, 81, 0, 81, 11, 81, 0, 81, 
	0, 81, 9, 81, 0, 81, 47, 81, 
	0, 81, 0, 81, 49, 81, 0, 81, 
	0, 81, 0, 81, 0, 81, 45, 81, 
	65, 5, 5, 5, 23, 25, 41, 39, 
	19, 0, 5, 15, 35, 17, 37, 43, 
	5, 31, 33, 5, 5, 5, 5, 0, 
	5, 5, 5, 27, 13, 29, 65, 5, 
	69, 7, 0, 0, 79, 0, 0, 0, 
	79, 0, 0, 0, 0, 73, 0, 0, 
	0, 79, 0, 0, 0, 0, 75, 21, 
	79, 0, 0, 79, 77, 0, 0, 0, 
	79, 0, 0, 0, 71, 0, 0, 0, 
	0, 79, 0, 79, 0, 79, 0, 79, 
	0, 79, 51, 79, 0, 79, 0, 79, 
	0, 79, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	81, 81, 81, 81, 79, 79, 73, 79, 
	75, 79, 79, 77, 79, 71, 79, 79, 
	79, 79, 79, 79, 79, 79, 79, 0
};

static const char _GrmLexer_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0
};

static const char _GrmLexer_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 3, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0
};

static const short _GrmLexer_eof_trans[] = {
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 324, 324, 324, 324, 324, 324, 
	324, 324, 0, 343, 343, 327, 343, 329, 
	343, 343, 332, 343, 334, 343, 343, 343, 
	343, 343, 343, 343, 343, 343
};

static const int GrmLexer_start = 74;
static const int GrmLexer_first_final = 74;
static const int GrmLexer_error = -1;

static const int GrmLexer_en_main = 74;


#line 241 "grm_lexer.rl"

} // namespace

namespace pmt::parserbuilder {
 using namespace pmt::util::smrt;

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
 
#line 285 "grm_lexer.cpp"
	{
	cs = GrmLexer_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 259 "grm_lexer.rl"
}

auto GrmLexer::next_token() -> GenericAst::UniqueHandle {
 MAKE_RL_CONTEXT_AVAILABLE;

 GenericAst::UniqueHandle accepted;

 
#line 298 "grm_lexer.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
_resume:
	_acts = _GrmLexer_actions + _GrmLexer_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 1 "NONE"
	{ts = p;}
	break;
#line 315 "grm_lexer.cpp"
		}
	}

	_keys = _GrmLexer_trans_keys + _GrmLexer_key_offsets[cs];
	_trans = _GrmLexer_index_offsets[cs];

	_klen = _GrmLexer_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _GrmLexer_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
_eof_trans:
	cs = _GrmLexer_trans_targs[_trans];

	if ( _GrmLexer_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _GrmLexer_actions + _GrmLexer_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 2:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 3:
#line 60 "grm_lexer.rl"
	{te = p+1;{ 
   ++ts;
   --te;
   ACCEPT_TOKEN(GrmAst::TkStringLiteral);
   {p++; goto _out; }
  }}
	break;
	case 4:
#line 72 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkBooleanLiteral);
   {p++; goto _out; }
  }}
	break;
	case 5:
#line 89 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkEpsilon);
   {p++; goto _out; }
  }}
	break;
	case 6:
#line 94 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkPipe);
   {p++; goto _out; }
  }}
	break;
	case 7:
#line 99 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkSemiColon);
   {p++; goto _out; }
  }}
	break;
	case 8:
#line 104 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkEquals);
   {p++; goto _out; }
  }}
	break;
	case 9:
#line 109 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkComma);
   {p++; goto _out; }
  }}
	break;
	case 10:
#line 114 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkDoubleDot);
   {p++; goto _out; }
  }}
	break;
	case 11:
#line 119 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenParen);
   {p++; goto _out; }
  }}
	break;
	case 12:
#line 124 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseParen);
   {p++; goto _out; }
  }}
	break;
	case 13:
#line 129 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenBrace);
   {p++; goto _out; }
  }}
	break;
	case 14:
#line 134 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseBrace);
   {p++; goto _out; }
  }}
	break;
	case 15:
#line 139 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenSquare);
   {p++; goto _out; }
  }}
	break;
	case 16:
#line 144 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseSquare);
   {p++; goto _out; }
  }}
	break;
	case 17:
#line 149 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenAngle);
   {p++; goto _out; }
  }}
	break;
	case 18:
#line 154 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseAngle);
   {p++; goto _out; }
  }}
	break;
	case 19:
#line 159 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkPlus);
   {p++; goto _out; }
  }}
	break;
	case 20:
#line 164 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkStar);
   {p++; goto _out; }
  }}
	break;
	case 21:
#line 169 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkQuestion);
   {p++; goto _out; }
  }}
	break;
	case 22:
#line 174 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterUnpack);
   {p++; goto _out; }
  }}
	break;
	case 23:
#line 179 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterHide);
   {p++; goto _out; }
  }}
	break;
	case 24:
#line 184 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterMerge);
   {p++; goto _out; }
  }}
	break;
	case 25:
#line 189 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterId);
   {p++; goto _out; }
  }}
	break;
	case 26:
#line 194 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterCaseSensitive);
   {p++; goto _out; }
  }}
	break;
	case 27:
#line 199 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyStart);
   {p++; goto _out; }
  }}
	break;
	case 28:
#line 205 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyWhitespace);
   {p++; goto _out; }
  }}
	break;
	case 29:
#line 211 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyComment);
   {p++; goto _out; }
  }}
	break;
	case 30:
#line 217 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyNewline);
   {p++; goto _out; }
  }}
	break;
	case 31:
#line 223 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyCaseSensitive);
   {p++; goto _out; }
  }}
	break;
	case 32:
#line 229 "grm_lexer.rl"
	{te = p+1;}
	break;
	case 33:
#line 231 "grm_lexer.rl"
	{te = p+1;}
	break;
	case 34:
#line 234 "grm_lexer.rl"
	{te = p+1;{ throw std::runtime_error("lexer error"); }}
	break;
	case 35:
#line 67 "grm_lexer.rl"
	{te = p;p--;{
   ACCEPT_TOKEN(GrmAst::TkIntegerLiteral);
   {p++; goto _out; }
  }}
	break;
	case 36:
#line 77 "grm_lexer.rl"
	{te = p;p--;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkTerminalIdentifier);
   {p++; goto _out; }
  }}
	break;
	case 37:
#line 83 "grm_lexer.rl"
	{te = p;p--;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkNonterminalIdentifier);
   {p++; goto _out; }
  }}
	break;
	case 38:
#line 230 "grm_lexer.rl"
	{te = p;p--;}
	break;
	case 39:
#line 234 "grm_lexer.rl"
	{te = p;p--;{ throw std::runtime_error("lexer error"); }}
	break;
	case 40:
#line 234 "grm_lexer.rl"
	{{p = ((te))-1;}{ throw std::runtime_error("lexer error"); }}
	break;
#line 601 "grm_lexer.cpp"
		}
	}

_again:
	_acts = _GrmLexer_actions + _GrmLexer_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 0:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 612 "grm_lexer.cpp"
		}
	}

	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _GrmLexer_eof_trans[cs] > 0 ) {
		_trans = _GrmLexer_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}

#line 267 "grm_lexer.rl"

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