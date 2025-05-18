
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


#line 242 "grm_lexer.rl"


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
	39, 1, 40, 1, 41
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
	96, 97, 98, 133, 138, 143, 150, 155, 
	162, 163, 165, 166, 169, 175, 179, 180, 
	181, 182, 183, 184, 185, 186
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
	126, 9, 13, 48, 57, 34, 32, 38, 
	40, 126, 95, 65, 90, 97, 122, 95, 
	48, 57, 65, 90, 97, 122, 95, 65, 
	90, 97, 122, 95, 48, 57, 65, 90, 
	97, 122, 46, 42, 47, 10, 35, 48, 
	57, 48, 57, 65, 90, 97, 122, 99, 
	110, 115, 119, 97, 112, 97, 105, 100, 
	101, 114, 110, 0
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
	1, 1, 31, 1, 1, 1, 1, 1, 
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
	156, 158, 160, 194, 198, 202, 207, 211, 
	216, 218, 221, 223, 226, 230, 235, 237, 
	239, 241, 243, 245, 247, 249
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
	91, 92, 93, 74, 74, 74, 74, 74, 
	83, 74, 74, 0, 0, 74, 1, 1, 
	1, 74, 77, 77, 77, 77, 74, 2, 
	2, 2, 74, 79, 79, 79, 79, 74, 
	74, 74, 3, 82, 74, 74, 82, 5, 
	6, 74, 84, 84, 84, 74, 7, 25, 
	31, 35, 74, 44, 74, 56, 74, 61, 
	74, 64, 74, 74, 74, 66, 74, 69, 
	74, 70, 74, 74, 74, 74, 74, 74, 
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
	74, 74, 74, 74, 74, 74, 74, 74, 
	0
};

static const char _GrmLexer_trans_actions[] = {
	7, 0, 0, 83, 0, 0, 0, 0, 
	83, 0, 0, 0, 0, 83, 0, 0, 
	69, 0, 0, 0, 0, 83, 0, 0, 
	83, 0, 0, 83, 0, 83, 0, 83, 
	0, 83, 0, 83, 0, 83, 0, 83, 
	0, 83, 0, 83, 0, 83, 0, 83, 
	0, 83, 65, 83, 0, 83, 0, 83, 
	0, 83, 0, 83, 61, 83, 0, 83, 
	0, 83, 0, 83, 0, 83, 0, 83, 
	63, 83, 0, 83, 0, 83, 0, 83, 
	57, 83, 0, 83, 0, 83, 0, 83, 
	0, 83, 0, 83, 0, 83, 0, 83, 
	0, 83, 59, 83, 0, 83, 0, 83, 
	0, 83, 0, 83, 0, 83, 0, 83, 
	0, 83, 0, 83, 0, 83, 0, 83, 
	0, 83, 55, 83, 0, 83, 0, 83, 
	0, 83, 0, 83, 11, 83, 0, 83, 
	0, 83, 9, 83, 0, 83, 49, 83, 
	0, 83, 0, 83, 51, 83, 0, 83, 
	0, 83, 0, 83, 0, 83, 47, 83, 
	67, 5, 5, 5, 23, 25, 41, 39, 
	19, 0, 5, 15, 35, 17, 37, 43, 
	5, 31, 33, 5, 5, 5, 5, 0, 
	5, 5, 5, 27, 13, 29, 45, 67, 
	5, 71, 7, 0, 0, 81, 0, 0, 
	0, 81, 0, 0, 0, 0, 75, 0, 
	0, 0, 81, 0, 0, 0, 0, 77, 
	21, 81, 0, 0, 81, 79, 0, 0, 
	0, 81, 0, 0, 0, 73, 0, 0, 
	0, 0, 81, 0, 81, 0, 81, 0, 
	81, 0, 81, 53, 81, 0, 81, 0, 
	81, 0, 81, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 83, 83, 83, 
	83, 83, 83, 83, 83, 81, 81, 75, 
	81, 77, 81, 81, 79, 81, 73, 81, 
	81, 81, 81, 81, 81, 81, 81, 81, 
	0
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
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 325, 325, 325, 325, 325, 325, 
	325, 325, 0, 344, 344, 328, 344, 330, 
	344, 344, 333, 344, 335, 344, 344, 344, 
	344, 344, 344, 344, 344, 344
};

static const int GrmLexer_start = 74;
static const int GrmLexer_first_final = 74;
static const int GrmLexer_error = -1;

static const int GrmLexer_en_main = 74;


#line 247 "grm_lexer.rl"

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
 
#line 287 "grm_lexer.cpp"
	{
	cs = GrmLexer_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 265 "grm_lexer.rl"
}

auto GrmLexer::next_token() -> GenericAst::UniqueHandle {
 MAKE_RL_CONTEXT_AVAILABLE;

 GenericAst::UniqueHandle accepted;

 
#line 300 "grm_lexer.cpp"
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
#line 317 "grm_lexer.cpp"
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
#line 61 "grm_lexer.rl"
	{te = p+1;{ 
   ++ts;
   --te;
   ACCEPT_TOKEN(GrmAst::TkStringLiteral);
   {p++; goto _out; }
  }}
	break;
	case 4:
#line 73 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkBooleanLiteral);
   {p++; goto _out; }
  }}
	break;
	case 5:
#line 90 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkEpsilon);
   {p++; goto _out; }
  }}
	break;
	case 6:
#line 95 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkPipe);
   {p++; goto _out; }
  }}
	break;
	case 7:
#line 100 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkSemiColon);
   {p++; goto _out; }
  }}
	break;
	case 8:
#line 105 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkEquals);
   {p++; goto _out; }
  }}
	break;
	case 9:
#line 110 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkComma);
   {p++; goto _out; }
  }}
	break;
	case 10:
#line 115 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkDoubleDot);
   {p++; goto _out; }
  }}
	break;
	case 11:
#line 120 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenParen);
   {p++; goto _out; }
  }}
	break;
	case 12:
#line 125 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseParen);
   {p++; goto _out; }
  }}
	break;
	case 13:
#line 130 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenBrace);
   {p++; goto _out; }
  }}
	break;
	case 14:
#line 135 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseBrace);
   {p++; goto _out; }
  }}
	break;
	case 15:
#line 140 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenSquare);
   {p++; goto _out; }
  }}
	break;
	case 16:
#line 145 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseSquare);
   {p++; goto _out; }
  }}
	break;
	case 17:
#line 150 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkOpenAngle);
   {p++; goto _out; }
  }}
	break;
	case 18:
#line 155 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkCloseAngle);
   {p++; goto _out; }
  }}
	break;
	case 19:
#line 160 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkPlus);
   {p++; goto _out; }
  }}
	break;
	case 20:
#line 165 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkStar);
   {p++; goto _out; }
  }}
	break;
	case 21:
#line 170 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkQuestion);
   {p++; goto _out; }
  }}
	break;
	case 22:
#line 175 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkTilde);
   {p++; goto _out; }
  }}
	break;
	case 23:
#line 180 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterUnpack);
   {p++; goto _out; }
  }}
	break;
	case 24:
#line 185 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterHide);
   {p++; goto _out; }
  }}
	break;
	case 25:
#line 190 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterMerge);
   {p++; goto _out; }
  }}
	break;
	case 26:
#line 195 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterId);
   {p++; goto _out; }
  }}
	break;
	case 27:
#line 200 "grm_lexer.rl"
	{te = p+1;{
   ACCEPT_TOKEN(GrmAst::TkKwParameterCaseSensitive);
   {p++; goto _out; }
  }}
	break;
	case 28:
#line 205 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyStart);
   {p++; goto _out; }
  }}
	break;
	case 29:
#line 211 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyWhitespace);
   {p++; goto _out; }
  }}
	break;
	case 30:
#line 217 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyComment);
   {p++; goto _out; }
  }}
	break;
	case 31:
#line 223 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyNewline);
   {p++; goto _out; }
  }}
	break;
	case 32:
#line 229 "grm_lexer.rl"
	{te = p+1;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkGrammarPropertyCaseSensitive);
   {p++; goto _out; }
  }}
	break;
	case 33:
#line 235 "grm_lexer.rl"
	{te = p+1;}
	break;
	case 34:
#line 237 "grm_lexer.rl"
	{te = p+1;}
	break;
	case 35:
#line 240 "grm_lexer.rl"
	{te = p+1;{ throw std::runtime_error("lexer error"); }}
	break;
	case 36:
#line 68 "grm_lexer.rl"
	{te = p;p--;{
   ACCEPT_TOKEN(GrmAst::TkIntegerLiteral);
   {p++; goto _out; }
  }}
	break;
	case 37:
#line 78 "grm_lexer.rl"
	{te = p;p--;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkTerminalIdentifier);
   {p++; goto _out; }
  }}
	break;
	case 38:
#line 84 "grm_lexer.rl"
	{te = p;p--;{
   ++ts;
   ACCEPT_TOKEN(GrmAst::TkNonterminalIdentifier);
   {p++; goto _out; }
  }}
	break;
	case 39:
#line 236 "grm_lexer.rl"
	{te = p;p--;}
	break;
	case 40:
#line 240 "grm_lexer.rl"
	{te = p;p--;{ throw std::runtime_error("lexer error"); }}
	break;
	case 41:
#line 240 "grm_lexer.rl"
	{{p = ((te))-1;}{ throw std::runtime_error("lexer error"); }}
	break;
#line 609 "grm_lexer.cpp"
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
#line 620 "grm_lexer.cpp"
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

#line 273 "grm_lexer.rl"

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