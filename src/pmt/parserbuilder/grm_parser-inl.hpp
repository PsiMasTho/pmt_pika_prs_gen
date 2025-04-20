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
#include "pmt/util/smrt/generic_ast.hpp"
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
#define YYNOCODE 58
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE pmt::util::smrt::GenericAst::UniqueHandle
typedef union {
  int yyinit;
  yy_storage<ParseTOKENTYPE>::type yy0;
  yy_storage<void>::type yy61;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL pmt::util::smrt::GenericAst::UniqueHandle *ast_;
#define ParseARG_PDECL , pmt::util::smrt::GenericAst::UniqueHandle *ast_
#define ParseARG_FETCH pmt::util::smrt::GenericAst::UniqueHandle *ast_ = yypParser->ast_
#define ParseARG_STORE yypParser->ast_ = ast_
#define YYNSTATE 81
#define YYNRULE 70
#define YY_MAX_SHIFT 80
#define YY_MIN_SHIFTREDUCE 140
#define YY_MAX_SHIFTREDUCE 209
#define YY_MIN_REDUCE 210
#define YY_MAX_REDUCE 279
#define YY_ERROR_ACTION 280
#define YY_ACCEPT_ACTION 281
#define YY_NO_ACTION 282
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
#define YY_ACTTAB_COUNT (188)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */ 210, 19,  21,  63,  43,  36,  67,  18,  21,  43,
 /*    10 */ 36,  281, 6,   212, 213, 214, 215, 24,  44,  236,
 /*    20 */ 3,   79,  13,  23,  22,  236, 30,  248, 51,  45,
 /*    30 */ 48,  54,  247, 51,  45,  48,  54,  158, 159, 267,
 /*    40 */ 157, 68,  160, 235, 67,  18,  21,  205, 9,   28,
 /*    50 */ 190, 191, 27,  189, 188, 192, 218, 211, 213, 214,
 /*    60 */ 215, 10,  46,  72,  70,  79,  13,  23,  47,  236,
 /*    70 */ 12,  79,  13,  23,  73,  236, 193, 79,  13,  23,
 /*    80 */ 80,  236, 278, 79,  13,  23,  32,  236, 38,  13,
 /*    90 */ 23,  202, 236, 60,  58,  56,  62,  11,  35,  37,
 /*   100 */ 219, 65,  42,  161, 196, 195, 194, 1,   164, 163,
 /*   110 */ 162, 31,  18,  21,  199, 14,  23,  20,  236, 29,
 /*   120 */ 277, 168, 169, 66,  33,  8,   64,  77,  25,  78,
 /*   130 */ 209, 40,  34,  39,  15,  5,   198, 74,  174, 41,
 /*   140 */ 171, 75,  170, 76,  16,  206, 2,   17,  204, 49,
 /*   150 */ 50,  203, 52,  182, 53,  55,  181, 57,  180, 179,
 /*   160 */ 59,  61,  176, 212, 167, 7,   151, 12,  150, 175,
 /*   170 */ 212, 69,  201, 212, 200, 71,  147, 212, 4,   212,
 /*   180 */ 173, 26,  11,  212, 146, 212, 212, 172,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */ 0,  52, 53, 48, 4,  5,  51, 52, 53, 4,
 /*    10 */ 5,  33, 34, 35, 36, 37, 38, 44, 39, 46,
 /*    20 */ 14, 42, 43, 44, 53, 46, 49, 50, 28, 29,
 /*    30 */ 30, 31, 50, 28, 29, 30, 31, 1,  2,  54,
 /*    40 */ 4,  48, 6,  45, 51, 52, 53, 8,  12, 10,
 /*    50 */ 1,  2,  16, 4,  5,  6,  41, 35, 36, 37,
 /*    60 */ 38, 12, 39, 26, 27, 42, 43, 44, 39, 46,
 /*    70 */ 7,  42, 43, 44, 39, 46, 13, 42, 43, 44,
 /*    80 */ 39, 46, 56, 42, 43, 44, 10, 46, 42, 43,
 /*    90 */ 44, 15, 46, 23, 24, 25, 26, 7,  14, 40,
 /*   100 */ 41, 2,  14, 13, 20, 21, 22, 9,  20, 21,
 /*   110 */ 22, 51, 52, 53, 15, 43, 44, 10, 46, 55,
 /*   120 */ 56, 1,  2,  2,  2,  9,  19, 2,  10, 2,
 /*   130 */ 15, 2,  10, 10, 18, 9,  15, 19, 15, 10,
 /*   140 */ 15, 47, 15, 47, 18, 8,  10, 9,  8,  5,
 /*   150 */ 9,  8,  3,  3,  9,  9,  3,  9,  3,  1,
 /*   160 */ 9,  9,  8,  57, 17, 9,  3,  7,  1,  8,
 /*   170 */ 57, 9,  15, 57, 15, 9,  8,  57, 9,  57,
 /*   180 */ 15, 11, 7,  57, 8,  57, 57, 15,
};
#define YY_SHIFT_USE_DFLT (188)
#define YY_SHIFT_COUNT (80)
#define YY_SHIFT_MIN (0)
#define YY_SHIFT_MAX (176)
static const unsigned char yy_shift_ofst[] = {
 /*     0 */ 5,   36,  36,  36,  36,  36,  0,   49,  49,  36,
 /*    10 */ 49,  36,  49,  36,  36,  70,  37,  6,   49,  49,
 /*    20 */ 70,  84,  84,  88,  88,  37,  120, 120, 6,   39,
 /*    30 */ 107, 63,  99,  76,  121, 122, 116, 118, 90,  125,
 /*    40 */ 123, 127, 129, 126, 137, 98,  115, 136, 138, 140,
 /*    50 */ 144, 141, 143, 149, 145, 150, 146, 153, 148, 155,
 /*    60 */ 151, 158, 152, 154, 156, 157, 159, 160, 161, 163,
 /*    70 */ 162, 167, 166, 168, 169, 147, 170, 165, 172, 175,
 /*    80 */ 176,
};
#define YY_REDUCE_USE_DFLT (-52)
#define YY_REDUCE_COUNT (28)
#define YY_REDUCE_MIN (-51)
#define YY_REDUCE_MAX (96)
static const signed char yy_reduce_ofst[] = {
 /*     0 */ -22, -21, 23,  29,  35,  41,  22, -45, -7,  46,
 /*    10 */ 60,  72,  -51, -27, -27, -23, 59, 64,  -29, -29,
 /*    20 */ -18, -15, -15, -2,  -2,  15,  94, 96,  26,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
 /*    10 */ 280, 280, 280, 223, 224, 280, 280, 280, 254, 255,
 /*    20 */ 280, 256, 257, 225, 226, 280, 280, 280, 280, 280,
 /*    30 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
 /*    40 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
 /*    50 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
 /*    60 */ 280, 280, 280, 280, 280, 280, 280, 253, 280, 280,
 /*    70 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 222,
 /*    80 */ 280,
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
 "TOKEN_GRAMMAR_PROPERTY_COMMENT",
 "TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE",
 "error",
 "grammar",
 "statement_list",
 "statement",
 "terminal_production",
 "rule_production",
 "grammar_property",
 "terminal_definition",
 "terminal_parameter_list",
 "terminal_parameter",
 "terminal_choices",
 "terminal_sequence",
 "terminal_expression",
 "terminal_repetition_range",
 "range_expression",
 "range_literal",
 "rule_definition",
 "rule_parameter_list",
 "rule_parameter",
 "rule_choices",
 "rule_sequence",
 "rule_expression",
 "rule_repetition_range",
 "terminal_definition_pair_list",
 "terminal_definition_pair",
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
 /*   6 */ "terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON",
 /*   7 */ "terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_OPEN_ANGLE terminal_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON",
 /*   8 */ "terminal_parameter_list ::= terminal_parameter_list TOKEN_COMMA terminal_parameter",
 /*   9 */ "terminal_parameter_list ::= terminal_parameter",
 /*  10 */ "terminal_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL",
 /*  11 */ "terminal_parameter ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  12 */ "terminal_definition ::= terminal_choices",
 /*  13 */ "terminal_choices ::= terminal_sequence",
 /*  14 */ "terminal_choices ::= terminal_choices TOKEN_PIPE terminal_sequence",
 /*  15 */ "terminal_sequence ::= terminal_expression",
 /*  16 */ "terminal_sequence ::= terminal_sequence terminal_expression",
 /*  17 */ "terminal_expression ::= TOKEN_TERMINAL_IDENTIFIER",
 /*  18 */ "terminal_expression ::= TOKEN_STRING_LITERAL",
 /*  19 */ "terminal_expression ::= TOKEN_INTEGER_LITERAL",
 /*  20 */ "terminal_expression ::= TOKEN_EPSILON",
 /*  21 */ "terminal_expression ::= TOKEN_OPEN_PAREN terminal_choices TOKEN_CLOSE_PAREN",
 /*  22 */ "terminal_expression ::= terminal_expression TOKEN_QUESTION",
 /*  23 */ "terminal_expression ::= terminal_expression TOKEN_STAR",
 /*  24 */ "terminal_expression ::= terminal_expression TOKEN_PLUS",
 /*  25 */ "terminal_expression ::= terminal_expression terminal_repetition_range",
 /*  26 */ "terminal_expression ::= range_expression",
 /*  27 */ "range_expression ::= TOKEN_OPEN_SQUARE range_literal TOKEN_DOUBLE_DOT range_literal TOKEN_CLOSE_SQUARE",
 /*  28 */ "range_literal ::= TOKEN_STRING_LITERAL",
 /*  29 */ "range_literal ::= TOKEN_INTEGER_LITERAL",
 /*  30 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE",
 /*  31 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE",
 /*  32 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  33 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  34 */ "terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  35 */ "rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_EQUALS rule_definition TOKEN_SEMICOLON",
 /*  36 */ "rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_OPEN_ANGLE rule_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_definition TOKEN_SEMICOLON",
 /*  37 */ "rule_parameter_list ::= rule_parameter_list TOKEN_COMMA rule_parameter",
 /*  38 */ "rule_parameter_list ::= rule_parameter",
 /*  39 */ "rule_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL",
 /*  40 */ "rule_parameter ::= TOKEN_KW_PARAMETER_UNPACK TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  41 */ "rule_parameter ::= TOKEN_KW_PARAMETER_HIDE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  42 */ "rule_parameter ::= TOKEN_KW_PARAMETER_MERGE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL",
 /*  43 */ "rule_definition ::= rule_choices",
 /*  44 */ "rule_choices ::= rule_sequence",
 /*  45 */ "rule_choices ::= rule_choices TOKEN_PIPE rule_sequence",
 /*  46 */ "rule_sequence ::= rule_expression",
 /*  47 */ "rule_sequence ::= rule_sequence rule_expression",
 /*  48 */ "rule_expression ::= TOKEN_RULE_IDENTIFIER",
 /*  49 */ "rule_expression ::= TOKEN_TERMINAL_IDENTIFIER",
 /*  50 */ "rule_expression ::= TOKEN_STRING_LITERAL",
 /*  51 */ "rule_expression ::= TOKEN_INTEGER_LITERAL",
 /*  52 */ "rule_expression ::= TOKEN_EPSILON",
 /*  53 */ "rule_expression ::= TOKEN_OPEN_PAREN rule_choices TOKEN_CLOSE_PAREN",
 /*  54 */ "rule_expression ::= rule_expression TOKEN_QUESTION",
 /*  55 */ "rule_expression ::= rule_expression TOKEN_STAR",
 /*  56 */ "rule_expression ::= rule_expression TOKEN_PLUS",
 /*  57 */ "rule_expression ::= rule_expression rule_repetition_range",
 /*  58 */ "rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE",
 /*  59 */ "rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE",
 /*  60 */ "rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  61 */ "rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  62 */ "rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE",
 /*  63 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL TOKEN_SEMICOLON",
 /*  64 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_START TOKEN_EQUALS TOKEN_RULE_IDENTIFIER TOKEN_SEMICOLON",
 /*  65 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_COMMENT TOKEN_EQUALS terminal_definition_pair_list TOKEN_SEMICOLON",
 /*  66 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON",
 /*  67 */ "terminal_definition_pair_list ::= terminal_definition_pair",
 /*  68 */ "terminal_definition_pair_list ::= terminal_definition_pair_list TOKEN_COMMA terminal_definition_pair",
 /*  69 */ "terminal_definition_pair ::= TOKEN_OPEN_BRACE terminal_definition TOKEN_COMMA terminal_definition TOKEN_CLOSE_BRACE",
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
    case 30: /* TOKEN_GRAMMAR_PROPERTY_COMMENT */
    case 31: /* TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE */
    case 33: /* grammar */
    case 34: /* statement_list */
    case 35: /* statement */
    case 36: /* terminal_production */
    case 37: /* rule_production */
    case 38: /* grammar_property */
    case 39: /* terminal_definition */
    case 40: /* terminal_parameter_list */
    case 41: /* terminal_parameter */
    case 42: /* terminal_choices */
    case 43: /* terminal_sequence */
    case 44: /* terminal_expression */
    case 45: /* terminal_repetition_range */
    case 46: /* range_expression */
    case 47: /* range_literal */
    case 48: /* rule_definition */
    case 49: /* rule_parameter_list */
    case 50: /* rule_parameter */
    case 51: /* rule_choices */
    case 52: /* rule_sequence */
    case 53: /* rule_expression */
    case 54: /* rule_repetition_range */
    case 55: /* terminal_definition_pair_list */
    case 56: /* terminal_definition_pair */
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yypminor->yy0));
      break;
    case 0: /* $ */
      yy_destructor<void>(std::addressof(yypminor->yy61));
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
    case 30: /* TOKEN_GRAMMAR_PROPERTY_COMMENT */
    case 31: /* TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE */
    case 33: /* grammar */
    case 34: /* statement_list */
    case 35: /* statement */
    case 36: /* terminal_production */
    case 37: /* rule_production */
    case 38: /* grammar_property */
    case 39: /* terminal_definition */
    case 40: /* terminal_parameter_list */
    case 41: /* terminal_parameter */
    case 42: /* terminal_choices */
    case 43: /* terminal_sequence */
    case 44: /* terminal_expression */
    case 45: /* terminal_repetition_range */
    case 46: /* range_expression */
    case 47: /* range_literal */
    case 48: /* rule_definition */
    case 49: /* rule_parameter_list */
    case 50: /* rule_parameter */
    case 51: /* rule_choices */
    case 52: /* rule_sequence */
    case 53: /* rule_expression */
    case 54: /* rule_repetition_range */
    case 55: /* terminal_definition_pair_list */
    case 56: /* terminal_definition_pair */
      yy_move<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yyDest->yy0), std::addressof(yySource->yy0));
      break;
    case 0: /* $ */
      yy_move<void>(std::addressof(yyDest->yy61), std::addressof(yySource->yy61));
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
 {33, -1}, {34, -2}, {34, -1}, {35, -1}, {35, -1}, {35, -1}, {36, -4}, {36, -7}, {40, -3}, {40, -1}, {41, -3}, {41, -3}, {39, -1}, {42, -1}, {42, -3}, {43, -1}, {43, -2}, {44, -1}, {44, -1}, {44, -1}, {44, -1}, {44, -3}, {44, -2}, {44, -2}, {44, -2}, {44, -2}, {44, -1}, {46, -5}, {47, -1}, {47, -1}, {45, -3}, {45, -4}, {45, -4}, {45, -5}, {45, -3},
 {37, -4}, {37, -7}, {49, -3}, {49, -1}, {50, -3}, {50, -3}, {50, -3}, {50, -3}, {48, -1}, {51, -1}, {51, -3}, {52, -1}, {52, -2}, {53, -1}, {53, -1}, {53, -1}, {53, -1}, {53, -1}, {53, -3}, {53, -2}, {53, -2}, {53, -2}, {53, -2}, {54, -3}, {54, -4}, {54, -4}, {54, -5}, {54, -3}, {38, -4}, {38, -4}, {38, -4}, {38, -4}, {55, -1}, {55, -3}, {56, -5},
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
      auto &A = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 45 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        *ast_ = std::move(A);
        (*ast_)->set_id(pmt::parserbuilder::GrmAst::NtGrammar);
      }
#line 1230 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(A);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
    } break;
    case 1:  /* statement_list ::= statement_list statement */
    case 16: /* terminal_sequence ::= terminal_sequence terminal_expression */
      yytestcase(yyruleno == 16);
      {
        pmt::util::smrt::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 51 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = std::move(B);
          A->give_child_at_back(std::move(C));
        }
#line 1246 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
      }
      break;
    case 2: /* statement_list ::= statement */
    case 9: /* terminal_parameter_list ::= terminal_parameter */
      yytestcase(yyruleno == 9);
    case 38: /* rule_parameter_list ::= rule_parameter */
      yytestcase(yyruleno == 38);
      {
        pmt::util::smrt::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 56 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children);
          A->give_child_at_back(std::move(B));
        }
#line 1263 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
      }
      break;
    case 3: /* statement ::= terminal_production */
    case 4: /* statement ::= rule_production */
      yytestcase(yyruleno == 4);
    case 5: /* statement ::= grammar_property */
      yytestcase(yyruleno == 5);
    case 17: /* terminal_expression ::= TOKEN_TERMINAL_IDENTIFIER */
      yytestcase(yyruleno == 17);
    case 18: /* terminal_expression ::= TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 18);
    case 19: /* terminal_expression ::= TOKEN_INTEGER_LITERAL */
      yytestcase(yyruleno == 19);
    case 20: /* terminal_expression ::= TOKEN_EPSILON */
      yytestcase(yyruleno == 20);
    case 26: /* terminal_expression ::= range_expression */
      yytestcase(yyruleno == 26);
    case 28: /* range_literal ::= TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 28);
    case 29: /* range_literal ::= TOKEN_INTEGER_LITERAL */
      yytestcase(yyruleno == 29);
    case 46: /* rule_sequence ::= rule_expression */
      yytestcase(yyruleno == 46);
    case 48: /* rule_expression ::= TOKEN_RULE_IDENTIFIER */
      yytestcase(yyruleno == 48);
    case 49: /* rule_expression ::= TOKEN_TERMINAL_IDENTIFIER */
      yytestcase(yyruleno == 49);
    case 50: /* rule_expression ::= TOKEN_STRING_LITERAL */
      yytestcase(yyruleno == 50);
    case 51: /* rule_expression ::= TOKEN_INTEGER_LITERAL */
      yytestcase(yyruleno == 51);
    case 52: /* rule_expression ::= TOKEN_EPSILON */
      yytestcase(yyruleno == 52);
      {
        pmt::util::smrt::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 62 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        { A = std::move(B); }
#line 1291 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
      }
      break;
    case 6: /* terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 75 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1309 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
    } break;
    case 7: /* terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_OPEN_ANGLE terminal_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-5].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &D = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 81 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
        A->unpack(1);
        A->give_child_at_back(std::move(D));
      }
#line 1333 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_destructor(D);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0), std::move(A));
    } break;
    case 8:  /* terminal_parameter_list ::= terminal_parameter_list TOKEN_COMMA terminal_parameter */
    case 14: /* terminal_choices ::= terminal_choices TOKEN_PIPE terminal_sequence */
      yytestcase(yyruleno == 14);
    case 37: /* rule_parameter_list ::= rule_parameter_list TOKEN_COMMA rule_parameter */
      yytestcase(yyruleno == 37);
    case 45: /* rule_choices ::= rule_choices TOKEN_PIPE rule_sequence */
      yytestcase(yyruleno == 45);
    case 68: /* terminal_definition_pair_list ::= terminal_definition_pair_list TOKEN_COMMA terminal_definition_pair */
      yytestcase(yyruleno == 68);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        pmt::util::smrt::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 90 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = std::move(B);
          A->give_child_at_back(std::move(C));
        }
#line 1355 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
      }
      break;
    case 10: /* terminal_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
    case 11: /* terminal_parameter ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 11);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        pmt::util::smrt::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 101 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalParameter);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1374 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
      }
      break;
    case 12: /* terminal_definition ::= terminal_choices */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 114 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinition);
        A->give_child_at_back(std::move(B));
      }
#line 1389 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 13: /* terminal_choices ::= terminal_sequence */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 120 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalChoices);
        A->give_child_at_back(std::move(B));
      }
#line 1403 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 15: /* terminal_sequence ::= terminal_expression */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 131 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalSequence);
        A->give_child_at_back(std::move(B));
      }
#line 1417 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 21: /* terminal_expression ::= TOKEN_OPEN_PAREN terminal_choices TOKEN_CLOSE_PAREN */
    case 53: /* rule_expression ::= TOKEN_OPEN_PAREN rule_choices TOKEN_CLOSE_PAREN */
      yytestcase(yyruleno == 53);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 158 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        { A = std::move(B); }
#line 1433 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
      }
      break;
    case 22: /* terminal_expression ::= terminal_expression TOKEN_QUESTION */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 162 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: "," "1"
        auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);

        auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

        auto rhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
        rhs->set_string("10#1");
        repetition_info->give_child_at_back(std::move(rhs));

        A->give_child_at_back(std::move(repetition_info));
      }
#line 1460 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 23: /* terminal_expression ::= terminal_expression TOKEN_STAR */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 180 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: ","
        auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);

        auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");

        repetition_info->give_child_at_back(std::move(comma));
        A->give_child_at_back(std::move(repetition_info));
      }
#line 1484 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 24: /* terminal_expression ::= terminal_expression TOKEN_PLUS */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 194 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: "1" ","
        auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);

        auto lhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
        lhs->set_string("10#1");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(lhs));

        auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

        A->give_child_at_back(std::move(repetition_info));
      }
#line 1512 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 25: /* terminal_expression ::= terminal_expression terminal_repetition_range */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 212 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1528 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 27: /* range_expression ::= TOKEN_OPEN_SQUARE range_literal TOKEN_DOUBLE_DOT range_literal TOKEN_CLOSE_SQUARE */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 223 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRange);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1548 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
    } break;
    case 30: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
    case 34: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
      yytestcase(yyruleno == 34);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 239 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
          A->give_child_at_back(std::move(B));
        }
#line 1565 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
      }
      break;
    case 31: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
    case 32: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
      yytestcase(yyruleno == 32);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 244 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1583 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
      }
      break;
    case 33: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      auto &D = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 256 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
        A->give_child_at_back(std::move(D));
      }
#line 1603 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_destructor(D);
    } break;
    case 35: /* rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 269 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1622 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
    } break;
    case 36: /* rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_OPEN_ANGLE rule_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-5].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &D = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 275 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleProduction);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
        A->unpack(1);
        A->give_child_at_back(std::move(D));
      }
#line 1646 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_destructor(D);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0), std::move(A));
    } break;
    case 39: /* rule_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
    case 40: /* rule_parameter ::= TOKEN_KW_PARAMETER_UNPACK TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 40);
    case 41: /* rule_parameter ::= TOKEN_KW_PARAMETER_HIDE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 41);
    case 42: /* rule_parameter ::= TOKEN_KW_PARAMETER_MERGE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
      yytestcase(yyruleno == 42);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
        pmt::util::smrt::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 295 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleParameter);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1668 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
      }
      break;
    case 43: /* rule_definition ::= rule_choices */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 320 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleDefinition);
        A->give_child_at_back(std::move(B));
      }
#line 1683 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 44: /* rule_choices ::= rule_sequence */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 326 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleChoices);
        A->give_child_at_back(std::move(B));
      }
#line 1697 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 47: /* rule_sequence ::= rule_sequence rule_expression */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 341 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleSequence);
        A->give_child_at_back(std::move(B));
        if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtRuleSequence) {
          A->unpack(A->get_children_size() - 1);
        }
        A->give_child_at_back(std::move(C));
        if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtRuleSequence) {
          A->unpack(A->get_children_size() - 1);
        }
      }
#line 1719 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 54: /* rule_expression ::= rule_expression TOKEN_QUESTION */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 378 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: "," "1"
        auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetitionRange);

        auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

        auto rhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
        rhs->set_string("10#1");
        repetition_info->give_child_at_back(std::move(rhs));

        A->give_child_at_back(std::move(repetition_info));
      }
#line 1748 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 55: /* rule_expression ::= rule_expression TOKEN_STAR */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 396 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: ","
        auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetitionRange);

        auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");

        repetition_info->give_child_at_back(std::move(comma));
        A->give_child_at_back(std::move(repetition_info));
      }
#line 1772 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 56: /* rule_expression ::= rule_expression TOKEN_PLUS */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 410 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetition);
        A->give_child_at_back(std::move(B));

        // Add repetition info: "1" ","
        auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetitionRange);

        auto lhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
        lhs->set_string("10#1");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(lhs));

        auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
        comma->set_string(",");
        repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

        A->give_child_at_back(std::move(repetition_info));
      }
#line 1800 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 57: /* rule_expression ::= rule_expression rule_repetition_range */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 428 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetition);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1816 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
    } break;
    case 58: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
    case 62: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
      yytestcase(yyruleno == 62);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 435 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetitionRange);
          A->give_child_at_back(std::move(B));
        }
#line 1834 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
      }
      break;
    case 59: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
    case 60: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
      yytestcase(yyruleno == 60);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 440 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetitionRange);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1852 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
      }
      break;
    case 61: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      auto &D = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 452 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtRuleRepetitionRange);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
        A->give_child_at_back(std::move(D));
      }
#line 1872 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
      yy_destructor(D);
    } break;
    case 63: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL TOKEN_SEMICOLON */
    case 64: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_START TOKEN_EQUALS TOKEN_RULE_IDENTIFIER TOKEN_SEMICOLON */
      yytestcase(yyruleno == 64);
    case 65: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_COMMENT TOKEN_EQUALS terminal_definition_pair_list TOKEN_SEMICOLON */
      yytestcase(yyruleno == 65);
    case 66: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
      yytestcase(yyruleno == 66);
      {
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
        yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
        pmt::util::smrt::GenericAst::UniqueHandle A;
        auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
        auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 465 "/home/pmt/repos/pmt/grammars/grm_parser.y"
        {
          A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
          A->give_child_at_back(std::move(B));
          A->give_child_at_back(std::move(C));
        }
#line 1894 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
        yy_destructor(B);
        yy_destructor(C);
        yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
      }
      break;
    case 67: /* terminal_definition_pair_list ::= terminal_definition_pair */
    {
      pmt::util::smrt::GenericAst::UniqueHandle A;
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 490 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinitionPairList);
        A->give_child_at_back(std::move(B));
      }
#line 1909 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
    } break;
    case 69: /* terminal_definition_pair ::= TOKEN_OPEN_BRACE terminal_definition TOKEN_COMMA terminal_definition TOKEN_CLOSE_BRACE */
    {
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
      yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
      auto &A = yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
      auto &B = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
      auto &C = yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 501 "/home/pmt/repos/pmt/grammars/grm_parser.y"
      {
        A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinitionPair);
        A->give_child_at_back(std::move(B));
        A->give_child_at_back(std::move(C));
      }
#line 1928 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
      yy_destructor(B);
      yy_destructor(C);
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
#line 1998 "/home/pmt/repos/pmt/grammars/grm_parser.cpp"
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
