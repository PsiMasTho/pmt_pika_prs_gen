/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>

namespace {

// use std::allocator etc?

// this is here so you can do something like Parse(void *, int, my_token &&) or (... const my_token &)
template <class T>
struct yy_fix_type {
  typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
};

template <>
struct yy_fix_type<void> {
  typedef struct {
  } type;
};

template <class T, class... Args>
typename yy_fix_type<T>::type &yy_constructor(void *vp, Args &&...args) {
  typedef typename yy_fix_type<T>::type TT;
  TT *tmp = ::new (vp) TT(std::forward<Args>(args)...);
  return *tmp;
}

template <class T>
typename yy_fix_type<T>::type &yy_cast(void *vp) {
  typedef typename yy_fix_type<T>::type TT;
  return *(TT *)vp;
}

template <class T>
void yy_destructor(void *vp) {
  typedef typename yy_fix_type<T>::type TT;
  ((TT *)vp)->~TT();
}

template <class T>
void yy_destructor(T &t) {
  t.~T();
}

template <class T>
void yy_move(void *dest, void *src) {
  typedef typename yy_fix_type<T>::type TT;

  TT &tmp = yy_cast<TT>(src);
  yy_constructor<TT>(dest, std::move(tmp));
  yy_destructor(tmp);
}

// this is to destruct references in the event of an exception.
// only the LHS needs to be deleted -- other items remain on the
// shift/reduce stack in a valid state
// (as long as the destructor) doesn't throw!
template <class T>
struct yy_auto_deleter {
  yy_auto_deleter(T &t)
   : ref(t)
   , enaged(true) {
  }
  yy_auto_deleter(const yy_auto_deleter &) = delete;
  yy_auto_deleter(yy_auto_deleter &&) = delete;
  yy_auto_deleter &operator=(const yy_auto_deleter &) = delete;
  yy_auto_deleter &operator=(yy_auto_deleter &&) = delete;

  ~yy_auto_deleter() {
    if (enaged)
      yy_destructor(ref);
  }
  void cancel() {
    enaged = false;
  }

 private:
  T &ref;
  bool enaged = false;
};

template <class T>
class yy_storage {
 private:
  typedef typename yy_fix_type<T>::type TT;

 public:
  typedef typename std::conditional<std::is_trivial<TT>::value, TT, typename std::aligned_storage<sizeof(TT), alignof(TT)>::type>::type type;
};

}  // namespace

/************ Begin %include sections from the grammar ************************/
#line 1 "/home/pmt/repos/pmt/grammars/grm_parser.y"
#include "pmt/parserbuilder/grm_ast.hpp"
#line 2 "/home/pmt/repos/pmt/grammars/grm_parser.y"
#include "pmt/util/parsert/generic_ast.hpp"
#line 130 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    ParseTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#ifndef INTERFACE
#define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 59
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE pmt::util::parsert::GenericAst::UniqueHandle
typedef union {
  int yyinit;
  yy_storage<ParseTOKENTYPE>::type yy0;
  yy_storage<void>::type yy71;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL pmt::util::parsert::GenericAst::UniqueHandle *ast_;
#define ParseARG_PDECL , pmt::util::parsert::GenericAst::UniqueHandle *ast_
#define ParseARG_FETCH pmt::util::parsert::GenericAst::UniqueHandle *ast_ = yypParser->ast_
#define ParseARG_STORE yypParser->ast_ = ast_
#define YYNSTATE 71
#define YYNRULE 66
#define YY_MAX_SHIFT 70
#define YY_MIN_SHIFTREDUCE 130
#define YY_MAX_SHIFTREDUCE 195
#define YY_MIN_REDUCE 196
#define YY_MAX_REDUCE 261
#define YY_ERROR_ACTION 262
#define YY_ACCEPT_ACTION 263
#define YY_NO_ACTION 264
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
#define yytestcase(X)
#endif

/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The yy_shift_ofst[S]+X value is out of range, or
**    (2)  yy_lookahead[yy_shift_ofst[S]+X] is not equal to X, or
**    (3)  yy_shift_ofst[S] equal YY_SHIFT_USE_DFLT.
** (Implementation note: YY_SHIFT_USE_DFLT is chosen so that
** YY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (170)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */ 196, 17,  240, 29,  41,  32,  16,  240, 261, 41,
 /*    10 */ 32,  263, 1,   198, 199, 200, 201, 178, 179, 257,
 /*    20 */ 177, 176, 180, 197, 199, 200, 201, 241, 49,  42,
 /*    30 */ 46,  44,  52,  49,  42,  46,  44,  52,  147, 148,
 /*    40 */ 20,  146, 221, 149, 58,  56,  54,  60,  27,  3,
 /*    50 */ 254, 33,  69,  23,  9,   19,  31,  221, 35,  16,
 /*    60 */ 240, 9,   19,  40,  221, 160, 9,   19,  39,  221,
 /*    70 */ 10,  19,  261, 221, 153, 152, 151, 193, 194, 30,
 /*    80 */ 233, 26,  18,  256, 65,  63,  34,  205, 157, 158,
 /*    90 */ 7,   61,  23,  13,  185, 24,  184, 45,  183, 12,
 /*   100 */ 11,  165, 11,  164, 5,   137, 21,  5,   70,  36,
 /*   110 */ 37,  5,   136, 150, 163, 66,  4,   43,  38,  232,
 /*   120 */ 220, 159, 204, 255, 67,  14,  68,  186, 8,   192,
 /*   130 */ 15,  28,  187, 25,  47,  182, 50,  156, 171, 48,
 /*   140 */ 181, 198, 170, 51,  169, 53,  55,  168, 141, 57,
 /*   150 */ 140, 198, 59,  6,   198, 198, 62,  64,  198, 2,
 /*   160 */ 198, 198, 22,  198, 198, 198, 198, 198, 162, 161,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */ 0,  51, 52, 48, 4,  5,  51, 52, 46, 4,
 /*    10 */ 5,  34, 35, 36, 37, 38, 39, 1,  2,  57,
 /*    20 */ 4,  5,  6,  36, 37, 38, 39, 52, 28, 29,
 /*    30 */ 30, 31, 32, 28, 29, 30, 31, 32, 1,  2,
 /*    40 */ 44, 4,  46, 6,  23, 24, 25, 26, 54, 12,
 /*    50 */ 56, 40, 2,  16, 43, 44, 48, 46, 40, 51,
 /*    60 */ 52, 43, 44, 40, 46, 15, 43, 44, 14, 46,
 /*    70 */ 43, 44, 46, 46, 20, 21, 22, 1,  2,  49,
 /*    80 */ 50, 55, 10, 57, 26, 27, 41, 42, 1,  2,
 /*    90 */ 9,  19, 16, 7,  8,  7,  8,  7,  8,  18,
 /*   100 */ 7,  8,  7,  8,  7,  8,  10, 7,  2,  10,
 /*   110 */ 2,  7,  8,  13, 15, 19, 9,  1,  10, 50,
 /*   120 */ 45, 15, 42, 56, 47, 18, 47, 1,  9,  1,
 /*   130 */ 9,  53, 1,  9,  5,  8,  3,  17, 3,  9,
 /*   140 */ 8,  58, 3,  9,  3,  9,  9,  1,  3,  9,
 /*   150 */ 1,  58, 9,  9,  58, 58, 9,  9,  58, 9,
 /*   160 */ 58, 58, 11, 58, 58, 58, 58, 58, 15, 15,
};
#define YY_SHIFT_USE_DFLT (170)
#define YY_SHIFT_COUNT (70)
#define YY_SHIFT_MIN (0)
#define YY_SHIFT_MAX (154)
static const unsigned char yy_shift_ofst[] = {
 /*     0 */ 5,   0,   37,  37,  37,  37,  16,  16,  76,  37,
 /*    10 */ 37,  16,  21,  76,  58,  116, 16,  16,  21,  54,
 /*    20 */ 54,  58,  87,  87,  116, 126, 86,  88,  90,  93,
 /*    30 */ 72,  95,  81,  97,  96,  100, 50,  99,  106, 108,
 /*    40 */ 104, 107, 119, 128, 121, 131, 124, 127, 129, 130,
 /*    50 */ 132, 133, 134, 135, 136, 139, 137, 141, 140, 146,
 /*    60 */ 143, 144, 145, 147, 149, 148, 150, 120, 151, 153,
 /*    70 */ 154,
};
#define YY_REDUCE_USE_DFLT (-51)
#define YY_REDUCE_COUNT (25)
#define YY_REDUCE_MIN (-50)
#define YY_REDUCE_MAX (80)
static const signed char yy_reduce_ofst[] = {
 /*     0 */ -23, -13, 11, 18,  23, 27, -45, 8,   26, -4,
 /*    10 */ -4,  -50, 30, -38, 45, -6, -25, -25, 69, 75,
 /*    20 */ 75,  80,  77, 79,  67, 78,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */ 262, 262, 262, 262, 262, 262, 262, 262, 262, 208,
 /*    10 */ 209, 262, 262, 262, 262, 262, 238, 239, 262, 210,
 /*    20 */ 211, 262, 262, 262, 262, 262, 262, 262, 262, 262,
 /*    30 */ 262, 262, 262, 262, 262, 262, 262, 262, 262, 262,
 /*    40 */ 262, 262, 262, 262, 262, 262, 262, 262, 262, 262,
 /*    50 */ 262, 262, 262, 262, 262, 262, 262, 262, 262, 262,
 /*    60 */ 262, 262, 262, 262, 262, 262, 262, 262, 262, 262,
 /*    70 */ 262,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.
** If a construct like the following:
**
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno; /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;     /* The major token value.  This is the code
                        ** number for the token at this stack level */
  YYMINORTYPE minor;    /* The user-supplied minor token value.  This
                        ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos; /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm; /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt; /* Shifts left before out of the error */
#endif
  ParseARG_SDECL /* A place to hold %extra_argument */
#if YYSTACKDEPTH <= 0
   int yystksz;          /* Current side of the stack */
  yyStackEntry *yystack; /* The parser's stack */
  yyStackEntry yystk0;   /* First stack entry */
#else
   yyStackEntry yystack[YYSTACKDEPTH]; /* The parser's stack */
  yyStackEntry *yystackEnd;            /* Last entry in the stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/*
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt) {
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if (yyTraceFILE == 0)
    yyTracePrompt = 0;
  else if (yyTracePrompt == 0)
    yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = {
 "$",
 "TOKEN_STRING_LITERAL",
 "TOKEN_INTEGER_LITERAL",
 "TOKEN_BOOLEAN_LITERAL",
 "TOKEN_TERMINAL_IDENTIFIER",
 "TOKEN_RULE_IDENTIFIER",
 "TOKEN_EPSILON",
 "TOKEN_PIPE",
 "TOKEN_SEMICOLON",
 "TOKEN_EQUALS",
 "TOKEN_COMMA",
 "TOKEN_DOUBLE_DOT",
 "TOKEN_OPEN_PAREN",
 "TOKEN_CLOSE_PAREN",
 "TOKEN_OPEN_BRACE",
 "TOKEN_CLOSE_BRACE",
 "TOKEN_OPEN_SQUARE",
 "TOKEN_CLOSE_SQUARE",
 "TOKEN_OPEN_ANGLE",
 "TOKEN_CLOSE_ANGLE",
 "TOKEN_PLUS",
 "TOKEN_STAR",
 "TOKEN_QUESTION",
 "TOKEN_KW_PARAMETER_UNPACK",
 "TOKEN_KW_PARAMETER_HIDE",
 "TOKEN_KW_PARAMETER_MERGE",
 "TOKEN_KW_PARAMETER_ID",
 "TOKEN_KW_PARAMETER_CASE_SENSITIVE",
 "TOKEN_GRAMMAR_PROPERTY_START",
 "TOKEN_GRAMMAR_PROPERTY_WHITESPACE",
 "TOKEN_GRAMMAR_PROPERTY_SINGLE_LINE_COMMENT",
 "TOKEN_GRAMMAR_PROPERTY_MULTI_LINE_COMMENT",
 "TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE",
 "error",
 "grammar",
 "statement_list",
 "statement",
 "terminal_production",
 "rule_production",
 "grammar_property",
 "terminal_choices",
 "terminal_parameter_list",
 "terminal_parameter",
 "terminal_sequence",
 "terminal_expression",
 "terminal_repetition_range",
 "range_expression",
 "range_literal",
 "rule_choices",
 "rule_parameter_list",
 "rule_parameter",
 "rule_sequence",
 "rule_expression",
 "grammar_property_single_line_comment_choices",
 "grammar_property_multi_line_comment_choices",
 "grammar_property_whitespace_choices",
 "string_literal_pair",
 "grammar_property_whitespace_expression",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
 */
static const char *const yyRuleName[] = {
 /*   0 */ "grammar ::= statement_list",
 /*   1 */ "statement_list ::= statement_list statement",
 /*   2 */ "statement_list ::= statement",
 /*   3 */ "statement ::= terminal_production",
 /*   4 */ "statement ::= rule_production",
 /*   5 */ "statement ::= grammar_property",
 /*   6 */ "terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_EQUALS terminal_choices TOKEN_SEMICOLON",
 /*   7 */ "terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_OPEN_ANGLE terminal_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_choices TOKEN_SEMICOLON",
 /*   8 */ "terminal_parameter_list ::= terminal_parameter_list TOKEN_COMMA terminal_parameter",
 /*   9 */ "terminal_parameter_list ::= terminal_parameter",
 /*  10 */ "terminal_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL",
 /*  11 */ "terminal_parameter ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  12 */ "terminal_choices ::= terminal_sequence",
 /*  13 */ "terminal_choices ::= terminal_choices TOKEN_PIPE terminal_sequence",
 /*  14 */ "terminal_sequence ::= terminal_expression",
 /*  15 */ "terminal_sequence ::= terminal_sequence terminal_expression",
 /*  16 */ "terminal_expression ::= TOKEN_TERMINAL_IDENTIFIER",
 /*  17 */ "terminal_expression ::= TOKEN_STRING_LITERAL",
 /*  18 */ "terminal_expression ::= TOKEN_INTEGER_LITERAL",
 /*  19 */ "terminal_expression ::= TOKEN_EPSILON",
 /*  20 */ "terminal_expression ::= TOKEN_OPEN_PAREN terminal_choices TOKEN_CLOSE_PAREN",
 /*  21 */ "terminal_expression ::= terminal_expression TOKEN_QUESTION",
 /*  22 */ "terminal_expression ::= terminal_expression TOKEN_STAR",
 /*  23 */ "terminal_expression ::= terminal_expression TOKEN_PLUS",
 /*  24 */ "terminal_expression ::= terminal_expression terminal_repetition_range",
 /*  25 */ "terminal_expression ::= range_expression",
 /*  26 */ "range_expression ::= TOKEN_OPEN_SQUARE range_literal TOKEN_DOUBLE_DOT range_literal TOKEN_CLOSE_SQUARE",
 /*  27 */ "range_literal ::= TOKEN_STRING_LITERAL",
 /*  28 */ "range_literal ::= TOKEN_INTEGER_LITERAL",
 /*  29 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE",
 /*  30 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE",
 /*  31 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  32 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  33 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  34 */ "rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_EQUALS rule_choices TOKEN_SEMICOLON",
 /*  35 */ "rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_OPEN_ANGLE rule_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_choices TOKEN_SEMICOLON",
 /*  36 */ "rule_parameter_list ::= rule_parameter_list TOKEN_COMMA rule_parameter",
 /*  37 */ "rule_parameter_list ::= rule_parameter",
 /*  38 */ "rule_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL",
 /*  39 */ "rule_parameter ::= TOKEN_KW_PARAMETER_UNPACK TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  40 */ "rule_parameter ::= TOKEN_KW_PARAMETER_HIDE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  41 */ "rule_parameter ::= TOKEN_KW_PARAMETER_MERGE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  42 */ "rule_choices ::= rule_sequence",
 /*  43 */ "rule_choices ::= rule_choices TOKEN_PIPE rule_sequence",
 /*  44 */ "rule_sequence ::= rule_expression",
 /*  45 */ "rule_sequence ::= rule_sequence rule_expression",
 /*  46 */ "rule_expression ::= TOKEN_RULE_IDENTIFIER",
 /*  47 */ "rule_expression ::= TOKEN_TERMINAL_IDENTIFIER",
 /*  48 */ "rule_expression ::= TOKEN_STRING_LITERAL",
 /*  49 */ "rule_expression ::= TOKEN_INTEGER_LITERAL",
 /*  50 */ "rule_expression ::= TOKEN_EPSILON",
 /*  51 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL TOKEN_SEMICOLON",
 /*  52 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_START TOKEN_EQUALS TOKEN_RULE_IDENTIFIER TOKEN_SEMICOLON",
 /*  53 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_SINGLE_LINE_COMMENT TOKEN_EQUALS grammar_property_single_line_comment_choices TOKEN_SEMICOLON",
 /*  54 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_MULTI_LINE_COMMENT TOKEN_EQUALS grammar_property_multi_line_comment_choices TOKEN_SEMICOLON",
 /*  55 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS grammar_property_whitespace_choices TOKEN_SEMICOLON",
 /*  56 */ "grammar_property_single_line_comment_choices ::= TOKEN_STRING_LITERAL",
 /*  57 */ "grammar_property_single_line_comment_choices ::= grammar_property_single_line_comment_choices TOKEN_PIPE TOKEN_STRING_LITERAL",
 /*  58 */ "grammar_property_multi_line_comment_choices ::= string_literal_pair",
 /*  59 */ "grammar_property_multi_line_comment_choices ::= grammar_property_multi_line_comment_choices TOKEN_PIPE string_literal_pair",
 /*  60 */ "grammar_property_whitespace_choices ::= grammar_property_whitespace_expression",
 /*  61 */ "grammar_property_whitespace_choices ::= grammar_property_whitespace_choices TOKEN_PIPE grammar_property_whitespace_expression",
 /*  62 */ "string_literal_pair ::= TOKEN_STRING_LITERAL TOKEN_STRING_LITERAL",
 /*  63 */ "grammar_property_whitespace_expression ::= TOKEN_STRING_LITERAL",
 /*  64 */ "grammar_property_whitespace_expression ::= TOKEN_INTEGER_LITERAL",
 /*  65 */ "grammar_property_whitespace_expression ::= range_expression",
};
#endif /* NDEBUG */

#if YYSTACKDEPTH <= 0

static void yy_transfer(yyParser *yypParser,    /* The parser */
                        yyStackEntry *yySource, /*  */
                        yyStackEntry *yyDest    /*  */
);

/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p) {
  int newSize;
  int idx;
  yyStackEntry *pNew;
  yyStackEntry *pOld = p->yystack;
  int oldSize = p->yystksz;

  newSize = oldSize * 2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;

  pNew = (yyStackEntry *)calloc(newSize, sizeof(pNew[0]));

  if (pNew) {
    for (int i = 0; i < oldSize; ++i) {
      pNew[i].stateno = pOld[i].stateno;
      pNew[i].major = pOld[i].major;
      yy_move(pOld[i].major, &pNew[i].minor, &pOld[i].minor);
    }
    if (pOld != &p->yystk0)
      free(pOld);

    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if (yyTraceFILE) {
      fprintf(yyTraceFILE, "%sStack grows from %d to %d entries.\n", yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew == 0;
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
#define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
 */
void ParseInit(void *yypParser) {
  yyParser *pParser = (yyParser *)yypParser;
  std::memset(pParser, 0, sizeof(yyParser)); /* not safe if extra_argument is not POD */
#ifdef YYTRACKMAXSTACKDEPTH
  pParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH <= 0
  pParser->yytos = NULL;
  pParser->yystack = NULL;
  pParser->yystksz = 0;
  if (yyGrowStack(pParser)) {
    pParser->yystack = &pParser->yystk0;
    pParser->yystksz = 1;
  }
#endif
#ifndef YYNOERRORRECOVERY
  pParser->yyerrcnt = -1;
#endif
  pParser->yytos = pParser->yystack;
  pParser->yystack[0].stateno = 0;
  pParser->yystack[0].major = 0;
#if YYSTACKDEPTH > 0
  pParser->yystackEnd = &pParser->yystack[YYSTACKDEPTH - 1];
#endif
}

#ifndef Parse_ENGINEALWAYSONSTACK
/*
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)) {
  yyParser *pParser;
  pParser = (yyParser *)(*mallocProc)((YYMALLOCARGTYPE)sizeof(yyParser));
  if (pParser)
    ParseInit(pParser);
  return pParser;
}
#endif /* Parse_ENGINEALWAYSONSTACK */

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(yyParser *yypParser,  /* The parser */
                          YYCODETYPE yymajor,   /* Type code for object to destroy */
                          YYMINORTYPE *yypminor /* The object to be destroyed */
) {
  ParseARG_FETCH;
  switch (yymajor) {
      /* Here is inserted the actions which take place when a
      ** terminal or non-terminal is destroyed.  This can happen
      ** when the symbol is popped from the stack during a
      ** reduce or during error processing or when a parser is
      ** being destroyed before it is finished parsing.
      **
      ** Note: during a reduce, the only symbols destroyed are those
      ** which appear on the RHS of the rule, but which are *not* used
      ** inside the C code.
      */
      /********* Begin destructor definitions ***************************************/
    case 1:  /* TOKEN_STRING_LITERAL */
    case 2:  /* TOKEN_INTEGER_LITERAL */
    case 3:  /* TOKEN_BOOLEAN_LITERAL */
    case 4:  /* TOKEN_TERMINAL_IDENTIFIER */
    case 5:  /* TOKEN_RULE_IDENTIFIER */
    case 6:  /* TOKEN_EPSILON */
    case 7:  /* TOKEN_PIPE */
    case 8:  /* TOKEN_SEMICOLON */
    case 9:  /* TOKEN_EQUALS */
    case 10: /* TOKEN_COMMA */
    case 11: /* TOKEN_DOUBLE_DOT */
    case 12: /* TOKEN_OPEN_PAREN */
    case 13: /* TOKEN_CLOSE_PAREN */
    case 14: /* TOKEN_OPEN_BRACE */
    case 15: /* TOKEN_CLOSE_BRACE */
    case 16: /* TOKEN_OPEN_SQUARE */
    case 17: /* TOKEN_CLOSE_SQUARE */
    case 18: /* TOKEN_OPEN_ANGLE */
    case 19: /* TOKEN_CLOSE_ANGLE */
    case 20: /* TOKEN_PLUS */
    case 21: /* TOKEN_STAR */
    case 22: /* TOKEN_QUESTION */
    case 23: /* TOKEN_KW_PARAMETER_UNPACK */
    case 24: /* TOKEN_KW_PARAMETER_HIDE */
    case 25: /* TOKEN_KW_PARAMETER_MERGE */
    case 26: /* TOKEN_KW_PARAMETER_ID */
    case 27: /* TOKEN_KW_PARAMETER_CASE_SENSITIVE */
    case 28: /* TOKEN_GRAMMAR_PROPERTY_START */
    case 29: /* TOKEN_GRAMMAR_PROPERTY_WHITESPACE */
    case 30: /* TOKEN_GRAMMAR_PROPERTY_SINGLE_LINE_COMMENT */
    case 31: /* TOKEN_GRAMMAR_PROPERTY_MULTI_LINE_COMMENT */
    case 32: /* TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE */
    case 34: /* grammar */
    case 35: /* statement_list */
    case 36: /* statement */
    case 37: /* terminal_production */
    case 38: /* rule_production */
    case 39: /* grammar_property */
    case 40: /* terminal_choices */
    case 41: /* terminal_parameter_list */
    case 42: /* terminal_parameter */
    case 43: /* terminal_sequence */
    case 44: /* terminal_expression */
    case 45: /* terminal_repetition_range */
    case 46: /* range_expression */
    case 47: /* range_literal */
    case 48: /* rule_choices */
    case 49: /* rule_parameter_list */
    case 50: /* rule_parameter */
    case 51: /* rule_sequence */
    case 52: /* rule_expression */
    case 53: /* grammar_property_single_line_comment_choices */
    case 54: /* grammar_property_multi_line_comment_choices */
    case 55: /* grammar_property_whitespace_choices */
    case 56: /* string_literal_pair */
    case 57: /* grammar_property_whitespace_expression */
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yypminor->yy0));
      break;
    case 0: /* $ */
      yy_destructor<void>(std::addressof(yypminor->yy71));
      break;
      /********* End destructor definitions *****************************************/
    default:
      break; /* If no destructor action specified: do nothing */
  }
}

/*
 * moves an object (such as when growing the stack).
 * Source is constructed.
 * Destination is also destructed.
 *
 */
static void yy_move(YYCODETYPE yymajor,   /* Type code for object to destroy */
                    YYMINORTYPE *yyDest,  /*  */
                    YYMINORTYPE *yySource /*  */
) {
  switch (yymajor) {
      /********* Begin move definitions ***************************************/
    case 1:  /* TOKEN_STRING_LITERAL */
    case 2:  /* TOKEN_INTEGER_LITERAL */
    case 3:  /* TOKEN_BOOLEAN_LITERAL */
    case 4:  /* TOKEN_TERMINAL_IDENTIFIER */
    case 5:  /* TOKEN_RULE_IDENTIFIER */
    case 6:  /* TOKEN_EPSILON */
    case 7:  /* TOKEN_PIPE */
    case 8:  /* TOKEN_SEMICOLON */
    case 9:  /* TOKEN_EQUALS */
    case 10: /* TOKEN_COMMA */
    case 11: /* TOKEN_DOUBLE_DOT */
    case 12: /* TOKEN_OPEN_PAREN */
    case 13: /* TOKEN_CLOSE_PAREN */
    case 14: /* TOKEN_OPEN_BRACE */
    case 15: /* TOKEN_CLOSE_BRACE */
    case 16: /* TOKEN_OPEN_SQUARE */
    case 17: /* TOKEN_CLOSE_SQUARE */
    case 18: /* TOKEN_OPEN_ANGLE */
    case 19: /* TOKEN_CLOSE_ANGLE */
    case 20: /* TOKEN_PLUS */
    case 21: /* TOKEN_STAR */
    case 22: /* TOKEN_QUESTION */
    case 23: /* TOKEN_KW_PARAMETER_UNPACK */
    case 24: /* TOKEN_KW_PARAMETER_HIDE */
    case 25: /* TOKEN_KW_PARAMETER_MERGE */
    case 26: /* TOKEN_KW_PARAMETER_ID */
    case 27: /* TOKEN_KW_PARAMETER_CASE_SENSITIVE */
    case 28: /* TOKEN_GRAMMAR_PROPERTY_START */
    case 29: /* TOKEN_GRAMMAR_PROPERTY_WHITESPACE */
    case 30: /* TOKEN_GRAMMAR_PROPERTY_SINGLE_LINE_COMMENT */
    case 31: /* TOKEN_GRAMMAR_PROPERTY_MULTI_LINE_COMMENT */
    case 32: /* TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE */
    case 34: /* grammar */
    case 35: /* statement_list */
    case 36: /* statement */
    case 37: /* terminal_production */
    case 38: /* rule_production */
    case 39: /* grammar_property */
    case 40: /* terminal_choices */
    case 41: /* terminal_parameter_list */
    case 42: /* terminal_parameter */
    case 43: /* terminal_sequence */
    case 44: /* terminal_expression */
    case 45: /* terminal_repetition_range */
    case 46: /* range_expression */
    case 47: /* range_literal */
    case 48: /* rule_choices */
    case 49: /* rule_parameter_list */
    case 50: /* rule_parameter */
    case 51: /* rule_sequence */
    case 52: /* rule_expression */
    case 53: /* grammar_property_single_line_comment_choices */
    case 54: /* grammar_property_multi_line_comment_choices */
    case 55: /* grammar_property_whitespace_choices */
    case 56: /* string_literal_pair */
    case 57: /* grammar_property_whitespace_expression */
      yy_move<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yyDest->yy0), std::addressof(yySource->yy0));
      break;
    case 0: /* $ */
      yy_move<void>(std::addressof(yyDest->yy71), std::addressof(yySource->yy71));
      break;
      /********* End move &&definitions *****************************************/
    default:
      break; /* If no destructor action specified: do nothing */
             // yyDest.minor = yySource.minor;
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser) {
  yyStackEntry *yytos;
  assert(pParser->yytos != 0);
  assert(pParser->yytos > pParser->yystack);
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if (yyTraceFILE) {
    fprintf(yyTraceFILE, "%sPopping %s\n", yyTracePrompt, yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/*
** Clear all secondary memory allocations from the parser
*/
void ParseFinalize(void *p) {
  yyParser *pParser = (yyParser *)p;
  while (pParser->yytos > pParser->yystack)
    yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH <= 0
  if (pParser->yystack != &pParser->yystk0)
    free(pParser->yystack);
#endif
}

#ifndef Parse_ENGINEALWAYSONSTACK
/*
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void ParseFree(void *p,                 /* The parser to be deleted */
               void (*freeProc)(void *) /* Function used to reclaim memory */
) {
#ifndef YYPARSEFREENEVERNULL
  if (p == 0)
    return;
#endif
  ParseFinalize(p);
  (*freeProc)(p);
}
#endif /* Parse_ENGINEALWAYSONSTACK */
/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p) {
  yyParser *pParser = (yyParser *)p;
  return pParser->yyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(yyParser *pParser,    /* The parser */
                                         YYCODETYPE iLookAhead /* The look-ahead token */
) {
  int i;
  int stateno = pParser->yytos->stateno;

  if (stateno >= YY_MIN_REDUCE)
    return stateno;
  assert(stateno <= YY_SHIFT_COUNT);
  do {
    i = yy_shift_ofst[stateno];
    assert(iLookAhead != YYNOCODE);
    i += iLookAhead;
    if (i < 0 || i >= YY_ACTTAB_COUNT || yy_lookahead[i] != iLookAhead) {
#ifdef YYFALLBACK
      YYCODETYPE iFallback; /* Fallback token */
      if (iLookAhead < sizeof(yyFallback) / sizeof(yyFallback[0]) && (iFallback = yyFallback[iLookAhead]) != 0) {
#ifndef NDEBUG
        if (yyTraceFILE) {
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n", yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert(yyFallback[iFallback] == 0); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if (
#if YY_SHIFT_MIN + YYWILDCARD < 0
         j >= 0 &&
#endif
#if YY_SHIFT_MAX + YYWILDCARD >= YY_ACTTAB_COUNT
         j < YY_ACTTAB_COUNT &&
#endif
         yy_lookahead[j] == YYWILDCARD && iLookAhead > 0) {
#ifndef NDEBUG
          if (yyTraceFILE) {
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n", yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    } else {
      return yy_action[i];
    }
  } while (1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(int stateno,          /* Current state number */
                                 YYCODETYPE iLookAhead /* The look-ahead token */
) {
  int i;
#ifdef YYERRORSYMBOL
  if (stateno > YY_REDUCE_COUNT) {
    return yy_default[stateno];
  }
#else
  assert(stateno <= YY_REDUCE_COUNT);
#endif
  i = yy_reduce_ofst[stateno];
  assert(i != YY_REDUCE_USE_DFLT);
  assert(iLookAhead != YYNOCODE);
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if (i < 0 || i >= YY_ACTTAB_COUNT || yy_lookahead[i] != iLookAhead) {
    return yy_default[stateno];
  }
#else
  assert(i >= 0 && i < YY_ACTTAB_COUNT);
  assert(yy_lookahead[i] == iLookAhead);
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser) {
  ParseARG_FETCH;
#ifndef NDEBUG
  if (yyTraceFILE) {
    fprintf(yyTraceFILE, "%sStack Overflow!\n", yyTracePrompt);
  }
#endif
  while (yypParser->yytos > yypParser->yystack)
    yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will execute if the parser
  ** stack ever overflows */
  /******** Begin %stack_overflow code ******************************************/
  /******** End %stack_overflow code ********************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState) {
  if (yyTraceFILE) {
    if (yyNewState < YYNSTATE) {
      fprintf(yyTraceFILE, "%sShift '%s', go to state %d\n", yyTracePrompt, yyTokenName[yypParser->yytos->major], yyNewState);
    } else {
      fprintf(yyTraceFILE, "%sShift '%s'\n", yyTracePrompt, yyTokenName[yypParser->yytos->major]);
    }
  }
}
#else
#define yyTraceShift(X, Y)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(yyParser *yypParser,   /* The parser to be shifted */
                     int yyNewState,        /* The new state to shift in */
                     int yyMajor,           /* The major token to shift in */
                     ParseTOKENTYPE yyMinor /* The minor token to shift in */
) {
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if ((int)(yypParser->yytos - yypParser->yystack) > yypParser->yyhwm) {
    yypParser->yyhwm++;
    assert(yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
  }
#endif
#if YYSTACKDEPTH > 0
  if (yypParser->yytos > yypParser->yystackEnd) {
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if (yypParser->yytos >= &yypParser->yystack[yypParser->yystksz]) {
    if (yyGrowStack(yypParser)) {
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if (yyNewState > YY_MAX_SHIFT) {
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  // yytos->minor.yy0 = yyMinor;
  // yy_move<ParseTOKENTYPE>(std::addressof(yytos->minor.yy0), std::addressof(yyMinor));
  // yy_move also calls destructor.
  yy_constructor<ParseTOKENTYPE>(std::addressof(yytos->minor.yy0), std::forward<ParseTOKENTYPE>(yyMinor));
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;   /* Symbol on the left-hand side of the rule */
  signed char nrhs; /* Negative of the number of RHS symbols in the rule */
} yyRuleInfo[] = {
 {34, -1}, {35, -2}, {35, -1}, {36, -1}, {36, -1}, {36, -1}, {37, -4}, {37, -7}, {41, -3}, {41, -1}, {42, -3}, {42, -3}, {40, -1}, {40, -3}, {43, -1}, {43, -2}, {44, -1}, {44, -1}, {44, -1}, {44, -1}, {44, -3}, {44, -2}, {44, -2}, {44, -2}, {44, -2}, {44, -1}, {46, -5}, {47, -1}, {47, -1}, {45, -3}, {45, -4}, {45, -4}, {45, -5},
 {45, -3}, {38, -4}, {38, -7}, {49, -3}, {49, -1}, {50, -3}, {50, -3}, {50, -3}, {50, -3}, {48, -1}, {48, -3}, {51, -1}, {51, -2}, {52, -1}, {52, -1}, {52, -1}, {52, -1}, {52, -1}, {39, -4}, {39, -4}, {39, -4}, {39, -4}, {39, -4}, {53, -1}, {53, -3}, {54, -1}, {54, -3}, {55, -1}, {55, -3}, {56, -2}, {57, -1}, {57, -1}, {57, -1},
};

static void yy_accept(yyParser *); /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(yyParser *yypParser,  /* The parser */
                      unsigned int yyruleno /* Number of the rule by which to reduce */
) {
  int yygoto;          /* The next state */
  int yyact;           /* The next action */
  yyStackEntry *yymsp; /* The top of the parser's stack */
  int yysize;          /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if (yyTraceFILE && yyruleno < (int)(sizeof(yyRuleName) / sizeof(yyRuleName[0]))) {
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s], go to state %d.\n", yyTracePrompt, yyRuleName[yyruleno], yymsp[yysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if (yyRuleInfo[yyruleno].nrhs == 0) {
#ifdef YYTRACKMAXSTACKDEPTH
    if ((int)(yypParser->yytos - yypParser->yystack) > yypParser->yyhwm) {
      yypParser->yyhwm++;
      assert(yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH > 0
    if (yypParser->yytos >= yypParser->yystackEnd) {
      yyStackOverflow(yypParser);
      return;
    }
#else
    if (yypParser->yytos >= &yypParser->yystack[yypParser->yystksz - 1]) {
      if (yyGrowStack(yypParser)) {
        yyStackOverflow(yypParser);
        return;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch (yyruleno) {
      /* Beginning here are the reduction cases.  A typical example
      ** follows:
      **   case 0:
      **  #line <lineno> <grammarfile>
      **     { ... }           // User supplied code
      **  #line <lineno> <thisfile>
      **     break;
      */
      /********** Begin reduce actions **********************************************/
    case 0: /* grammar ::= statement_list */
    {
      auto &A = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 46 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        *ast_ = std::move(A);
        (*ast_)->set_id(pmt::parserbuilder::GrmAst::NtGrammar);
      }
#line 1218 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(A);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
    } break;
    case 1:  /* statement_list ::= statement_list statement */
    case 15: /* terminal_sequence ::= terminal_sequence terminal_expression */
      yytestcase(yyruleno == 15);
      {
        pmt::util::parsert::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 52 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = std::move(B);
          A->give_child_at_back(std::move(C));
        }
#line 1234 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
      }
      break;
    case 2: /* statement_list ::= statement */
    case 9: /* terminal_parameter_list ::= terminal_parameter */
      yytestcase(yyruleno == 9);
    case 37: /* rule_parameter_list ::= rule_parameter */
      yytestcase(yyruleno == 37);
      {
        pmt::util::parsert::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 57 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children);
          A->give_child_at_back(std::move(B));
        }
#line 1251 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
      }
      break;
    case 3: /* statement ::= terminal_production */
    case 4: /* statement ::= rule_production */
      yytestcase(yyruleno == 4);
    case 5: /* statement ::= grammar_property */
      yytestcase(yyruleno == 5);
    case 16: /* terminal_expression ::= TOKEN_TERMINAL_IDENTIFIER */
      yytestcase(yyruleno == 16);
    case 17: /* terminal_expression ::= TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 17);
    case 18: /* terminal_expression ::= TOKEN_INTEGER_LITERAL */
      yytestcase(yyruleno == 18);
    case 19: /* terminal_expression ::= TOKEN_EPSILON */
      yytestcase(yyruleno == 19);
    case 25: /* terminal_expression ::= range_expression */
      yytestcase(yyruleno == 25);
    case 27: /* range_literal ::= TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 27);
    case 28: /* range_literal ::= TOKEN_INTEGER_LITERAL */
      yytestcase(yyruleno == 28);
    case 44: /* rule_sequence ::= rule_expression */
      yytestcase(yyruleno == 44);
    case 46: /* rule_expression ::= TOKEN_RULE_IDENTIFIER */
      yytestcase(yyruleno == 46);
    case 47: /* rule_expression ::= TOKEN_TERMINAL_IDENTIFIER */
      yytestcase(yyruleno == 47);
    case 48: /* rule_expression ::= TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 48);
    case 49: /* rule_expression ::= TOKEN_INTEGER_LITERAL */
      yytestcase(yyruleno == 49);
    case 50: /* rule_expression ::= TOKEN_EPSILON */
      yytestcase(yyruleno == 50);
    case 63: /* grammar_property_whitespace_expression ::= TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 63);
    case 64: /* grammar_property_whitespace_expression ::= TOKEN_INTEGER_LITERAL */
      yytestcase(yyruleno == 64);
    case 65: /* grammar_property_whitespace_expression ::= range_expression */
      yytestcase(yyruleno == 65);
      {
        pmt::util::parsert::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 63 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        { A = std::move(B); }
#line 1282 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
      }
      break;
    case 6: /* terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_EQUALS terminal_choices TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 76 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1300 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
    } break;
    case 7: /* terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_OPEN_ANGLE terminal_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_choices TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-5].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &D = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 82 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
        A->unpack(1);
        A->give_child_at_back(std::move(D));
      }
#line 1324 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_destructor(D);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0), std::move(A));
    } break;
    case 8:  /* terminal_parameter_list ::= terminal_parameter_list TOKEN_COMMA terminal_parameter */
    case 13: /* terminal_choices ::= terminal_choices TOKEN_PIPE terminal_sequence */
      yytestcase(yyruleno == 13);
    case 36: /* rule_parameter_list ::= rule_parameter_list TOKEN_COMMA rule_parameter */
      yytestcase(yyruleno == 36);
    case 43: /* rule_choices ::= rule_choices TOKEN_PIPE rule_sequence */
      yytestcase(yyruleno == 43);
    case 57: /* grammar_property_single_line_comment_choices ::= grammar_property_single_line_comment_choices TOKEN_PIPE TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 57);
    case 59: /* grammar_property_multi_line_comment_choices ::= grammar_property_multi_line_comment_choices TOKEN_PIPE string_literal_pair */
      yytestcase(yyruleno == 59);
    case 61: /* grammar_property_whitespace_choices ::= grammar_property_whitespace_choices TOKEN_PIPE grammar_property_whitespace_expression */
      yytestcase(yyruleno == 61);
      {
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        pmt::util::parsert::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 91 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = std::move(B);
          A->give_child_at_back(std::move(C));
        }
#line 1348 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
      }
      break;
    case 10: /* terminal_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
    case 11: /* terminal_parameter ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 11);
      {
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        pmt::util::parsert::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 102 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalParameter);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1367 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
      }
      break;
    case 12: /* terminal_choices ::= terminal_sequence */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 115 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalChoices);
        A->give_child_at_back(std::move(B));
      }
#line 1382 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 14: /* terminal_sequence ::= terminal_expression */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 126 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalSequence);
        A->give_child_at_back(std::move(B));
      }
#line 1396 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 20: /* terminal_expression ::= TOKEN_OPEN_PAREN terminal_choices TOKEN_CLOSE_PAREN */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      auto &A = yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 153 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      { A = std::move(B); }
#line 1411 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
    } break;
    case 21: /* terminal_expression ::= terminal_expression TOKEN_QUESTION */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 157 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: "," "1"
        auto repetition_info = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetitionRange);

        auto comma = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

        auto rhs = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
        rhs->set_string("10#1");
        repetition_info->give_child_at_back(std::move(rhs));

        A->give_child_at_back(std::move(repetition_info));
      }
#line 1438 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 22: /* terminal_expression ::= terminal_expression TOKEN_STAR */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 175 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: ","
        auto repetition_info = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetitionRange);

        auto comma = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");

        repetition_info->give_child_at_back(std::move(comma));
        A->give_child_at_back(std::move(repetition_info));
      }
#line 1462 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 23: /* terminal_expression ::= terminal_expression TOKEN_PLUS */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 189 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: "1" ","
        auto repetition_info = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetitionRange);

        auto lhs = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
        lhs->set_string("10#1");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(lhs));

        auto comma = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

        A->give_child_at_back(std::move(repetition_info));
      }
#line 1490 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 24: /* terminal_expression ::= terminal_expression terminal_repetition_range */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 207 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetition);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1506 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 26: /* range_expression ::= TOKEN_OPEN_SQUARE range_literal TOKEN_DOUBLE_DOT range_literal TOKEN_CLOSE_SQUARE */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      auto &A = yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 218 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRange);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1526 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
    } break;
    case 29: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
    case 33: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
      yytestcase(yyruleno == 33);
      {
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        auto &A = yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 234 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetitionRange);
          A->give_child_at_back(std::move(B));
        }
#line 1543 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
      }
      break;
    case 30: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
    case 31: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
      yytestcase(yyruleno == 31);
      {
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        auto &A = yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 239 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetitionRange);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1561 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
      }
      break;
    case 32: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      auto &A = yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      auto &D = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 251 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRepetitionRange);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
        A->give_child_at_back(std::move(D));
      }
#line 1581 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_destructor(D);
    } break;
    case 34: /* rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_EQUALS rule_choices TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 264 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1600 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
    } break;
    case 35: /* rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_OPEN_ANGLE rule_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_choices TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-5].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &D = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 270 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
        A->unpack(1);
        A->give_child_at_back(std::move(D));
      }
#line 1624 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_destructor(D);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0), std::move(A));
    } break;
    case 38: /* rule_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
    case 39: /* rule_parameter ::= TOKEN_KW_PARAMETER_UNPACK TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 39);
    case 40: /* rule_parameter ::= TOKEN_KW_PARAMETER_HIDE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 40);
    case 41: /* rule_parameter ::= TOKEN_KW_PARAMETER_MERGE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 41);
      {
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        pmt::util::parsert::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 290 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleParameter);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1646 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
      }
      break;
    case 42: /* rule_choices ::= rule_sequence */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 315 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleChoices);
        A->give_child_at_back(std::move(B));
      }
#line 1661 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 45: /* rule_sequence ::= rule_sequence rule_expression */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 330 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleSequence);
        A->give_child_at_back(std::move(B));
        if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtRuleSequence) {
          A->unpack(A->get_children_size() - 1);
        }
        A->give_child_at_back(std::move(C));
        if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtRuleSequence) {
          A->unpack(A->get_children_size() - 1);
        }
      }
#line 1683 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 51: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL TOKEN_SEMICOLON */
    case 52: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_START TOKEN_EQUALS TOKEN_RULE_IDENTIFIER TOKEN_SEMICOLON */
      yytestcase(yyruleno == 52);
    case 53: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_SINGLE_LINE_COMMENT TOKEN_EQUALS grammar_property_single_line_comment_choices TOKEN_SEMICOLON */
      yytestcase(yyruleno == 53);
    case 54: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_MULTI_LINE_COMMENT TOKEN_EQUALS grammar_property_multi_line_comment_choices TOKEN_SEMICOLON */
      yytestcase(yyruleno == 54);
    case 55: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS grammar_property_whitespace_choices TOKEN_SEMICOLON */
      yytestcase(yyruleno == 55);
      {
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        yy_destructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        pmt::util::parsert::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 364 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1706 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
      }
      break;
    case 56: /* grammar_property_single_line_comment_choices ::= TOKEN_STRING_LITERAL */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 395 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarPropertySingleLineCommentChoices);
        A->give_child_at_back(std::move(B));
      }
#line 1721 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 58: /* grammar_property_multi_line_comment_choices ::= string_literal_pair */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 406 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarPropertyMultiLineCommentChoices);
        A->give_child_at_back(std::move(B));
      }
#line 1735 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 60: /* grammar_property_whitespace_choices ::= grammar_property_whitespace_expression */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 417 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarPropertyWhitespaceChoices);
        A->give_child_at_back(std::move(B));
      }
#line 1749 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 62: /* string_literal_pair ::= TOKEN_STRING_LITERAL TOKEN_STRING_LITERAL */
    {
      pmt::util::parsert::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
      auto &C = yy_cast<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 428 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::parsert::GenericAst::construct(pmt::util::parsert::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtStringLiteralPair);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1765 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::parsert::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    default:
      break;
      /********** End reduce actions ************************************************/
  };
  assert(yyruleno < sizeof(yyRuleInfo) / sizeof(yyRuleInfo[0]));
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[yysize].stateno, (YYCODETYPE)yygoto);

  /* There are no SHIFTREDUCE actions on nonterminals because the table
  ** generator has simplified them to pure REDUCE actions. */
  assert(!(yyact > YY_MAX_SHIFT && yyact <= YY_MAX_SHIFTREDUCE));

  /* It is not possible for a REDUCE to be followed by an error */
  assert(yyact != YY_ERROR_ACTION);

  if (yyact == YY_ACCEPT_ACTION) {
    yypParser->yytos += yysize;
    yy_accept(yypParser);
  } else {
    yymsp += yysize + 1;
    yypParser->yytos = yymsp;
    yymsp->stateno = (YYACTIONTYPE)yyact;
    yymsp->major = (YYCODETYPE)yygoto;
    yyTraceShift(yypParser, yyact);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(yyParser *yypParser /* The parser */
) {
  ParseARG_FETCH;
#ifndef NDEBUG
  if (yyTraceFILE) {
    fprintf(yyTraceFILE, "%sFail!\n", yyTracePrompt);
  }
#endif
  while (yypParser->yytos > yypParser->yystack)
    yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  /************ Begin %parse_failure code ***************************************/
  /************ End %parse_failure code *****************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(yyParser *yypParser,   /* The parser */
                            int yymajor,           /* The major type of the error token */
                            ParseTOKENTYPE yyminor /* The minor type of the error token */
) {
  ParseARG_FETCH;
  // #define TOKEN yyminor
  auto &TOKEN = yyminor;
/************ Begin %syntax_error code ****************************************/
#line 8 "/home/pmt/repos/pmt/grammars/grm_parser.y"

  throw std::runtime_error("Syntax error");
#line 1836 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
  /************ End %syntax_error code ******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(yyParser *yypParser /* The parser */
) {
  ParseARG_FETCH;
#ifndef NDEBUG
  if (yyTraceFILE) {
    fprintf(yyTraceFILE, "%sAccept!\n", yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert(yypParser->yytos == yypParser->yystack);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  /*********** Begin %parse_accept code *****************************************/
  /*********** End %parse_accept code *******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(void *yyp,             /* The parser */
           int yymajor,           /* The major token code number */
           ParseTOKENTYPE yyminor /* The value for the token */
            ParseARG_PDECL        /* Optional %extra_argument parameter */
) {
  // YYMINORTYPE yyminorunion;    /* lemon++ -- not needed for destructor */
  unsigned int yyact; /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput; /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0; /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser; /* The parser */

  yypParser = (yyParser *)yyp;
  assert(yypParser->yytos != 0);
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor == 0);
#endif
  ParseARG_STORE;

#ifndef NDEBUG
  if (yyTraceFILE) {
    fprintf(yyTraceFILE, "%sInput '%s'\n", yyTracePrompt, yyTokenName[yymajor]);
  }
#endif

  do {
    yyact = yy_find_shift_action(yypParser, (YYCODETYPE)yymajor);
    if (yyact <= YY_MAX_SHIFTREDUCE) {
      yy_shift(yypParser, yyact, yymajor, std::forward<ParseTOKENTYPE>(yyminor));
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    } else if (yyact <= YY_MAX_REDUCE) {
      yy_reduce(yypParser, yyact - YY_MIN_REDUCE);
    } else {
      assert(yyact == YY_ERROR_ACTION);
      // yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if (yyTraceFILE) {
        fprintf(yyTraceFILE, "%sSyntax Error!\n", yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if (yypParser->yyerrcnt < 0) {
        yy_syntax_error(yypParser, yymajor, std::forward<ParseTOKENTYPE>(yyminor));
      }
      yymx = yypParser->yytos->major;
      if (yymx == YYERRORSYMBOL || yyerrorhit) {
#ifndef NDEBUG
        if (yyTraceFILE) {
          fprintf(yyTraceFILE, "%sDiscard input token %s\n", yyTracePrompt, yyTokenName[yymajor]);
        }
#endif
        // yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      } else {
        while (yypParser->yytos >= yypParser->yystack && yymx != YYERRORSYMBOL && (yyact = yy_find_reduce_action(yypParser->yytos->stateno, YYERRORSYMBOL)) >= YY_MIN_REDUCE) {
          yy_pop_parser_stack(yypParser);
        }
        if (yypParser->yytos < yypParser->yystack || yymajor == 0) {
          // yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        } else if (yymx != YYERRORSYMBOL) {
          yy_shift(yypParser, yyact, YYERRORSYMBOL, std::forward<ParseTOKENTYPE>(yyminor));
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser, yymajor, std::forward<ParseTOKENTYPE>(yyminor));
      // yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;

#else /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if (yypParser->yyerrcnt <= 0) {
        yy_syntax_error(yypParser, yymajor, std::forward<ParseTOKENTYPE>(yyminor));
      }
      yypParser->yyerrcnt = 3;
      // yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if (yyendofinput) {
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      yymajor = YYNOCODE;
#endif
    }
  } while (yymajor != YYNOCODE && yypParser->yytos > yypParser->yystack);
#ifndef NDEBUG
  if (yyTraceFILE) {
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE, "%sReturn. Stack=", yyTracePrompt);
    for (i = &yypParser->yystack[1]; i <= yypParser->yytos; i++) {
      fprintf(yyTraceFILE, "%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    if (cDiv == '[')
      fprintf(yyTraceFILE, "[");
    fprintf(yyTraceFILE, "]\n");
  }
#endif
  return;
}
