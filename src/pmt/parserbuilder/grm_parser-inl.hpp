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
#include <cstdio>
#include <cstring>
#include <cassert>
#include <type_traits>
#include <new>
#include <memory>

namespace {

  // use std::allocator etc?


  // this is here so you can do something like Parse(void *, int, my_token &&) or (... const my_token &)
  template<class T> struct yy_fix_type {
    typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
  };

  template<>
  struct yy_fix_type<void> {
    typedef struct {} type;
  };

  template<class T, class... Args>
  typename yy_fix_type<T>::type &yy_constructor(void *vp, Args&&... args ) {
    typedef typename yy_fix_type<T>::type TT;
    TT *tmp = ::new(vp) TT(std::forward<Args>(args)...);
    return *tmp;
  }

  template<class T>
  typename yy_fix_type<T>::type &yy_cast(void *vp) {
    typedef typename yy_fix_type<T>::type TT;
    return *(TT *)vp;
  }


  template<class T>
  void yy_destructor(void *vp) {
    typedef typename yy_fix_type<T>::type TT;
    ((TT *)vp)->~TT();
  }


  template<class T>
  void yy_destructor(T &t) {
    t.~T();
  }



  template<class T>
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
  template<class T>
  struct yy_auto_deleter {

    yy_auto_deleter(T &t) : ref(t), enaged(true)
    {}
    yy_auto_deleter(const yy_auto_deleter &) = delete;
    yy_auto_deleter(yy_auto_deleter &&) = delete;
    yy_auto_deleter &operator=(const yy_auto_deleter &) = delete;
    yy_auto_deleter &operator=(yy_auto_deleter &&) = delete;

    ~yy_auto_deleter() {
      if (enaged) yy_destructor(ref);
    }
    void cancel() { enaged = false; }

  private:
    T& ref;
    bool enaged=false;
  };

  template<class T>
  class yy_storage {
  private:
    typedef typename yy_fix_type<T>::type TT;

  public:
    typedef typename std::conditional<
      std::is_trivial<TT>::value,
      TT,
      typename std::aligned_storage<sizeof(TT),alignof(TT)>::type
    >::type type;
  };

}

/************ Begin %include sections from the grammar ************************/
#line 1 "grm_parser.y"
#include "pmt/parserbuilder/grm_ast.hpp"
#line 2 "grm_parser.y"
#include "pmt/util/smrt/generic_ast.hpp"
#line 130 "grm_parser.cpp"
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
**    ParseARG_PARAM     Code to pass %extra_argument as a subroutine parameter
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    ParseCTX_*         As ParseARG_ except for %extra_context
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYNTOKEN           Number of terminal symbols
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
**    YY_MIN_REDUCE      Minimum value for reduce actions
**    YY_MAX_REDUCE      Maximum value for reduce actions
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 57
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
#define ParseARG_SDECL pmt::util::smrt::GenericAst::UniqueHandle* ast_;
#define ParseARG_PDECL ,pmt::util::smrt::GenericAst::UniqueHandle* ast_
#define ParseARG_PARAM ,ast_
#define ParseARG_FETCH pmt::util::smrt::GenericAst::UniqueHandle* ast_=yypParser->ast_;
#define ParseARG_STORE yypParser->ast_=ast_;
#define ParseCTX_SDECL
#define ParseCTX_PDECL
#define ParseCTX_PARAM
#define ParseCTX_FETCH
#define ParseCTX_STORE
#define YYNSTATE             84
#define YYNRULE              71
#define YYNTOKEN             33
#define YY_MAX_SHIFT         83
#define YY_MIN_SHIFTREDUCE   144
#define YY_MAX_SHIFTREDUCE   214
#define YY_ERROR_ACTION      215
#define YY_ACCEPT_ACTION     216
#define YY_NO_ACTION         217
#define YY_MIN_REDUCE        218
#define YY_MAX_REDUCE        288
/************* End control #defines *******************************************/
#define YY_NLOOKAHEAD ((int)(sizeof(yy_lookahead)/sizeof(yy_lookahead[0])))

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
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
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X.
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array.
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
#define YY_ACTTAB_COUNT (197)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   218,   20,   22,   66,   44,   37,   70,   19,   22,   44,
 /*    10 */    37,  216,    7,  220,  221,  222,  223,   25,   45,  244,
 /*    20 */     4,   82,   14,   24,   23,  244,   31,  256,   54,   46,
 /*    30 */    51,   48,   57,   54,   46,   51,   48,   57,  162,  163,
 /*    40 */   209,  161,   29,  164,  255,  219,  221,  222,  223,   10,
 /*    50 */    68,  194,  195,   28,  193,  192,  196,   63,   61,   59,
 /*    60 */    65,   47,   11,  203,   82,   14,   24,  275,  244,   49,
 /*    70 */    75,   73,   82,   14,   24,   50,  244,   13,   82,   14,
 /*    80 */    24,   76,  244,  197,   82,   14,   24,   12,  244,   15,
 /*    90 */    24,   83,  244,  165,   82,   14,   24,   71,  244,   34,
 /*   100 */    70,   19,   22,   36,   39,   14,   24,   35,  244,  200,
 /*   110 */   199,  198,   43,  243,   32,   19,   22,   69,  168,  167,
 /*   120 */   166,   38,  227,   21,   30,  286,  172,  173,   26,   33,
 /*   130 */   202,    9,   67,   80,  206,   81,   40,   77,   78,   41,
 /*   140 */    16,  178,  226,    6,  287,    1,  175,   42,  174,   79,
 /*   150 */   211,  210,   17,    2,  214,    3,   18,  208,   52,   53,
 /*   160 */   207,   55,  186,   56,   58,  185,   60,  184,  183,   62,
 /*   170 */    64,  180,  217,  171,    8,  155,   13,  154,  179,  217,
 /*   180 */    72,  205,  217,  204,   74,  151,  217,    5,  217,  177,
 /*   190 */    27,   12,  217,  150,  217,  217,  176,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     0,   52,   53,   48,    4,    5,   51,   52,   53,    4,
 /*    10 */     5,   33,   34,   35,   36,   37,   38,   44,   39,   46,
 /*    20 */    14,   42,   43,   44,   53,   46,   49,   50,   28,   29,
 /*    30 */    30,   31,   32,   28,   29,   30,   31,   32,    1,    2,
 /*    40 */     8,    4,   10,    6,   50,   35,   36,   37,   38,   12,
 /*    50 */     2,    1,    2,   16,    4,    5,    6,   23,   24,   25,
 /*    60 */    26,   39,   12,   15,   42,   43,   44,   54,   46,   39,
 /*    70 */    26,   27,   42,   43,   44,   39,   46,    7,   42,   43,
 /*    80 */    44,   39,   46,   13,   42,   43,   44,    7,   46,   43,
 /*    90 */    44,   39,   46,   13,   42,   43,   44,   48,   46,    2,
 /*   100 */    51,   52,   53,   14,   42,   43,   44,   10,   46,   20,
 /*   110 */    21,   22,   14,   45,   51,   52,   53,    2,   20,   21,
 /*   120 */    22,   40,   41,   10,   55,   56,    1,    2,   10,   10,
 /*   130 */    15,    9,   19,    2,   15,    2,   10,   19,   47,    2,
 /*   140 */    18,   15,   41,    9,   56,    9,   15,   10,   15,   47,
 /*   150 */     8,    8,   18,    9,   15,   10,    9,    8,    5,    9,
 /*   160 */     8,    3,    3,    9,    9,    3,    9,    3,    1,    9,
 /*   170 */     9,    8,   57,   17,    9,    3,    7,    1,    8,   57,
 /*   180 */     9,   15,   57,   15,    9,    8,   57,    9,   57,   15,
 /*   190 */    11,    7,   57,    8,   57,   57,   15,   57,   57,   57,
 /*   200 */    57,   57,   57,   57,   57,   57,   57,   57,   57,   57,
 /*   210 */    57,   57,   57,   57,   57,   57,   57,   57,   57,   57,
 /*   220 */    57,   57,   57,   57,   57,   57,   57,   57,   57,   57,
};
#define YY_SHIFT_COUNT    (83)
#define YY_SHIFT_MIN      (0)
#define YY_SHIFT_MAX      (185)
static const unsigned char yy_shift_ofst[] = {
 /*     0 */     5,   37,   37,   37,   37,   37,   37,    0,   50,   50,
 /*    10 */    37,   50,   37,   50,   37,   37,   34,   44,    6,   50,
 /*    20 */    50,   34,   89,   89,   98,   98,   44,  125,  125,    6,
 /*    30 */    32,  113,   70,   48,  119,  115,   97,  122,  118,   80,
 /*    40 */   131,  126,  133,  137,  134,  142,  136,  143,  144,  139,
 /*    50 */   145,  147,  149,  153,  150,  152,  158,  154,  159,  155,
 /*    60 */   162,  157,  164,  160,  167,  161,  163,  165,  166,  168,
 /*    70 */   169,  170,  172,  171,  176,  175,  177,  178,  156,  179,
 /*    80 */   174,  181,  184,  185,
};
#define YY_REDUCE_COUNT (29)
#define YY_REDUCE_MIN   (-51)
#define YY_REDUCE_MAX   (102)
static const signed char yy_reduce_ofst[] = {
 /*     0 */   -22,  -21,   22,   30,   36,   42,   52,   10,  -45,   49,
 /*    10 */    62,   63,   46,  -51,  -27,  -27,  -23,   81,   69,  -29,
 /*    20 */   -29,   -6,   13,   13,   68,   68,  101,   91,  102,   88,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   215,  215,  215,  215,  215,  215,  215,  215,  215,  215,
 /*    10 */   215,  215,  215,  215,  231,  232,  215,  215,  215,  262,
 /*    20 */   263,  215,  264,  265,  233,  234,  215,  215,  215,  215,
 /*    30 */   215,  215,  215,  215,  215,  215,  215,  215,  215,  215,
 /*    40 */   215,  215,  215,  215,  215,  215,  215,  215,  215,  215,
 /*    50 */   215,  215,  215,  215,  215,  215,  215,  215,  215,  215,
 /*    60 */   215,  215,  215,  215,  215,  215,  215,  215,  215,  215,
 /*    70 */   261,  215,  215,  215,  215,  215,  215,  215,  215,  215,
 /*    80 */   215,  215,  230,  215,
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
static const YYCODETYPE yyFallback[] = {
};
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
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  ParseARG_SDECL                /* A place to hold %extra_argument */
  ParseCTX_SDECL                /* A place to hold %extra_context */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
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
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#if defined(YYCOVERAGE) || !defined(NDEBUG)
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  /*    0 */ "$",
  /*    1 */ "TOKEN_STRING_LITERAL",
  /*    2 */ "TOKEN_INTEGER_LITERAL",
  /*    3 */ "TOKEN_BOOLEAN_LITERAL",
  /*    4 */ "TOKEN_TERMINAL_IDENTIFIER",
  /*    5 */ "TOKEN_RULE_IDENTIFIER",
  /*    6 */ "TOKEN_EPSILON",
  /*    7 */ "TOKEN_PIPE",
  /*    8 */ "TOKEN_SEMICOLON",
  /*    9 */ "TOKEN_EQUALS",
  /*   10 */ "TOKEN_COMMA",
  /*   11 */ "TOKEN_DOUBLE_DOT",
  /*   12 */ "TOKEN_OPEN_PAREN",
  /*   13 */ "TOKEN_CLOSE_PAREN",
  /*   14 */ "TOKEN_OPEN_BRACE",
  /*   15 */ "TOKEN_CLOSE_BRACE",
  /*   16 */ "TOKEN_OPEN_SQUARE",
  /*   17 */ "TOKEN_CLOSE_SQUARE",
  /*   18 */ "TOKEN_OPEN_ANGLE",
  /*   19 */ "TOKEN_CLOSE_ANGLE",
  /*   20 */ "TOKEN_PLUS",
  /*   21 */ "TOKEN_STAR",
  /*   22 */ "TOKEN_QUESTION",
  /*   23 */ "TOKEN_KW_PARAMETER_UNPACK",
  /*   24 */ "TOKEN_KW_PARAMETER_HIDE",
  /*   25 */ "TOKEN_KW_PARAMETER_MERGE",
  /*   26 */ "TOKEN_KW_PARAMETER_ID",
  /*   27 */ "TOKEN_KW_PARAMETER_CASE_SENSITIVE",
  /*   28 */ "TOKEN_GRAMMAR_PROPERTY_START",
  /*   29 */ "TOKEN_GRAMMAR_PROPERTY_WHITESPACE",
  /*   30 */ "TOKEN_GRAMMAR_PROPERTY_COMMENT",
  /*   31 */ "TOKEN_GRAMMAR_PROPERTY_NEWLINE",
  /*   32 */ "TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE",
  /*   33 */ "grammar",
  /*   34 */ "statement_list",
  /*   35 */ "statement",
  /*   36 */ "terminal_production",
  /*   37 */ "rule_production",
  /*   38 */ "grammar_property",
  /*   39 */ "terminal_definition",
  /*   40 */ "terminal_parameter_list",
  /*   41 */ "terminal_parameter",
  /*   42 */ "terminal_choices",
  /*   43 */ "terminal_sequence",
  /*   44 */ "terminal_expression",
  /*   45 */ "terminal_repetition_range",
  /*   46 */ "range_expression",
  /*   47 */ "range_literal",
  /*   48 */ "rule_definition",
  /*   49 */ "rule_parameter_list",
  /*   50 */ "rule_parameter",
  /*   51 */ "rule_choices",
  /*   52 */ "rule_sequence",
  /*   53 */ "rule_expression",
  /*   54 */ "rule_repetition_range",
  /*   55 */ "terminal_definition_pair_list",
  /*   56 */ "terminal_definition_pair",
};
#endif /* defined(YYCOVERAGE) || !defined(NDEBUG) */

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
 /*  66 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_NEWLINE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON",
 /*  67 */ "grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON",
 /*  68 */ "terminal_definition_pair_list ::= terminal_definition_pair",
 /*  69 */ "terminal_definition_pair_list ::= terminal_definition_pair_list TOKEN_COMMA terminal_definition_pair",
 /*  70 */ "terminal_definition_pair ::= TOKEN_OPEN_BRACE terminal_definition TOKEN_COMMA terminal_definition TOKEN_CLOSE_BRACE",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0

static void yy_transfer(
  yyParser *yypParser,    /* The parser */
  yyStackEntry *yySource,     /*  */
  yyStackEntry *yyDest     /*  */
);


/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;
  yyStackEntry *pOld = p->yystack;
  int oldSize = p->yystksz;

  newSize = oldSize*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;

  pNew = (yyStackEntry *)calloc(newSize, sizeof(pNew[0]));

  if( pNew ){

    for (int i = 0; i < oldSize; ++i) {
      pNew[i].stateno = pOld[i].stateno;
      pNew[i].major = pOld[i].major;
      yy_move(pOld[i].major, &pNew[i].minor, &pOld[i].minor);
    }
    if (pOld != &p->yystk0) free(pOld);

    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
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
# define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
void ParseInit(void *yypRawParser ParseCTX_PDECL){
  yyParser *yypParser = (yyParser*)yypRawParser;
  ParseCTX_STORE
  //std::memset(pParser, 0, sizeof(yyParser)); /* not safe if extra_argument is not POD */
#ifdef YYTRACKMAXSTACKDEPTH
  yypParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
  yypParser->yytos = NULL;
  yypParser->yystack = NULL;
  yypParser->yystksz = 0;
  if( yyGrowStack(yypParser) ){
    yypParser->yystack = &yypParser->yystk0;
    yypParser->yystksz = 1;
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  yypParser->yytos = yypParser->yystack;
  yypParser->yystack[0].stateno = 0;
  yypParser->yystack[0].major = 0;
#if YYSTACKDEPTH>0
  yypParser->yystackEnd = &yypParser->yystack[YYSTACKDEPTH-1];
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
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE) ParseCTX_PDECL){
  yyParser *yypParser;
  yypParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( yypParser ){
    ParseCTX_STORE
    ParseInit(yypParser ParseCTX_PARAM);
  }
  return (void*)yypParser;
}
#endif /* Parse_ENGINEALWAYSONSTACK */


/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH
  ParseCTX_FETCH
  switch( yymajor ){
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
    case 1: /* TOKEN_STRING_LITERAL */
    case 2: /* TOKEN_INTEGER_LITERAL */
    case 3: /* TOKEN_BOOLEAN_LITERAL */
    case 4: /* TOKEN_TERMINAL_IDENTIFIER */
    case 5: /* TOKEN_RULE_IDENTIFIER */
    case 6: /* TOKEN_EPSILON */
    case 7: /* TOKEN_PIPE */
    case 8: /* TOKEN_SEMICOLON */
    case 9: /* TOKEN_EQUALS */
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
    case 31: /* TOKEN_GRAMMAR_PROPERTY_NEWLINE */
    case 32: /* TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE */
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
    default:  break;   /* If no destructor action specified: do nothing */
  }
}


/*
 * moves an object (such as when growing the stack). 
 * Source is constructed.
 * Destination is also destructed.
 * 
 */
static void yy_move(
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yyDest,     /*  */
  YYMINORTYPE *yySource     /*  */
){
  switch( yymajor ){

/********* Begin move definitions ***************************************/
    case 1: /* TOKEN_STRING_LITERAL */
    case 2: /* TOKEN_INTEGER_LITERAL */
    case 3: /* TOKEN_BOOLEAN_LITERAL */
    case 4: /* TOKEN_TERMINAL_IDENTIFIER */
    case 5: /* TOKEN_RULE_IDENTIFIER */
    case 6: /* TOKEN_EPSILON */
    case 7: /* TOKEN_PIPE */
    case 8: /* TOKEN_SEMICOLON */
    case 9: /* TOKEN_EQUALS */
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
    case 31: /* TOKEN_GRAMMAR_PROPERTY_NEWLINE */
    case 32: /* TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE */
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
    default:  break;   /* If no destructor action specified: do nothing */
      //yyDest.minor = yySource.minor;
  }
}


/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/*
** Clear all secondary memory allocations from the parser
*/
void ParseFinalize(void *p){
  yyParser *pParser = (yyParser*)p;
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
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
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  ParseFinalize(p);
  (*freeProc)(p);
}
#endif /* Parse_ENGINEALWAYSONSTACK */
/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
#endif

/* This array of booleans keeps track of the parser statement
** coverage.  The element yycoverage[X][Y] is set when the parser
** is in state X and has a lookahead token Y.  In a well-tested
** systems, every element of this matrix should end up being set.
*/
#if defined(YYCOVERAGE)
static unsigned char yycoverage[YYNSTATE][YYNTOKEN];
#endif

/*
** Write into out a description of every state/lookahead combination that
**
**   (1)  has not been used by the parser, and
**   (2)  is not a syntax error.
**
** Return the number of missed state/lookahead combinations.
*/
#if defined(YYCOVERAGE)
int ParseCoverage(FILE *out){
  int stateno, iLookAhead, i;
  int nMissed = 0;
  for(stateno=0; stateno<YYNSTATE; stateno++){
    i = yy_shift_ofst[stateno];
    for(iLookAhead=0; iLookAhead<YYNTOKEN; iLookAhead++){
      if( yy_lookahead[i+iLookAhead]!=iLookAhead ) continue;
      if( yycoverage[stateno][iLookAhead]==0 ) nMissed++;
      if( out ){
        fprintf(out,"State %d lookahead %s %s\n", stateno,
                yyTokenName[iLookAhead],
                yycoverage[stateno][iLookAhead] ? "ok" : "missed");
      }
    }
  }
  return nMissed;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static YYACTIONTYPE yy_find_shift_action(
  YYCODETYPE iLookAhead,    /* The look-ahead token */
  YYACTIONTYPE stateno      /* Current state number */
){
  int i;

  if( stateno>YY_MAX_SHIFT ) return stateno;
  assert( stateno <= YY_SHIFT_MAX );
#if defined(YYCOVERAGE)
  yycoverage[stateno][iLookAhead] = 1;
#endif
  do{
    i = stateno <= YY_SHIFT_COUNT ? yy_shift_ofst[stateno] : stateno;
    assert( i>=0 );
    assert( i<=YY_ACTTAB_COUNT );
    assert( i+YYNTOKEN<=(int)YY_NLOOKAHEAD );
    assert( iLookAhead!=YYNOCODE );
    assert( iLookAhead < YYNTOKEN );
    i += iLookAhead;
    assert( i<(int)YY_NLOOKAHEAD );
    if( yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      assert( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0]) );
      iFallback = yyFallback[iLookAhead];
      if( iFallback!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        assert( j<(int)(sizeof(yy_lookahead)/sizeof(yy_lookahead[0])) );
        if( yy_lookahead[j]==YYWILDCARD && iLookAhead>0 ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead],
               yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    }else{
      assert( i>=0 && i<sizeof(yy_action)/sizeof(yy_action[0]) );
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static YYACTIONTYPE yy_find_reduce_action(
  YYACTIONTYPE stateno,     /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   ParseARG_FETCH
   ParseCTX_FETCH
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   ParseARG_STORE /* Suppress warning about unused %extra_argument var */
   ParseCTX_STORE
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState, const char *zTag){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%s%s '%s', go to state %d\n",
         yyTracePrompt, zTag, yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%s%s '%s', pending reduce %d\n",
         yyTracePrompt, zTag, yyTokenName[yypParser->yytos->major],
         yyNewState - YY_MIN_REDUCE);
    }
  }
}
#else
# define yyTraceShift(X,Y,Z)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  YYACTIONTYPE yyNewState,      /* The new state to shift in */
  YYCODETYPE yyMajor,           /* The major token to shift in */
  ParseTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>yypParser->yystackEnd ){
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  //yytos->minor.yy0 = yyMinor;
  //yy_move<ParseTOKENTYPE>(std::addressof(yytos->minor.yy0), std::addressof(yyMinor));
  //yy_move also calls destructor.
  yy_constructor<ParseTOKENTYPE>(std::addressof(yytos->minor.yy0), std::forward<ParseTOKENTYPE>(yyMinor));
  yyTraceShift(yypParser, yyNewState, "Shift");
}

#ifdef YYERRORSYMBOL
static void yy_shift_error(
  yyParser *yypParser,          /* The parser to be shifted */
  YYACTIONTYPE yyNewState       /* The new state to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>yypParser->yystackEnd ){
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = yyNewState;
  yytos->major = YYERRORSYMBOL;
  yytos->minor.YYERRSYMDT = 0;
}
#endif

/* For rule J, yyRuleInfoLhs[J] contains the symbol on the left-hand side
** of that rule */
static const YYCODETYPE yyRuleInfoLhs[] = {
    33,  /* (0) grammar ::= statement_list */
    34,  /* (1) statement_list ::= statement_list statement */
    34,  /* (2) statement_list ::= statement */
    35,  /* (3) statement ::= terminal_production */
    35,  /* (4) statement ::= rule_production */
    35,  /* (5) statement ::= grammar_property */
    36,  /* (6) terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
    36,  /* (7) terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_OPEN_ANGLE terminal_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
    40,  /* (8) terminal_parameter_list ::= terminal_parameter_list TOKEN_COMMA terminal_parameter */
    40,  /* (9) terminal_parameter_list ::= terminal_parameter */
    41,  /* (10) terminal_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
    41,  /* (11) terminal_parameter ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
    39,  /* (12) terminal_definition ::= terminal_choices */
    42,  /* (13) terminal_choices ::= terminal_sequence */
    42,  /* (14) terminal_choices ::= terminal_choices TOKEN_PIPE terminal_sequence */
    43,  /* (15) terminal_sequence ::= terminal_expression */
    43,  /* (16) terminal_sequence ::= terminal_sequence terminal_expression */
    44,  /* (17) terminal_expression ::= TOKEN_TERMINAL_IDENTIFIER */
    44,  /* (18) terminal_expression ::= TOKEN_STRING_LITERAL */
    44,  /* (19) terminal_expression ::= TOKEN_INTEGER_LITERAL */
    44,  /* (20) terminal_expression ::= TOKEN_EPSILON */
    44,  /* (21) terminal_expression ::= TOKEN_OPEN_PAREN terminal_choices TOKEN_CLOSE_PAREN */
    44,  /* (22) terminal_expression ::= terminal_expression TOKEN_QUESTION */
    44,  /* (23) terminal_expression ::= terminal_expression TOKEN_STAR */
    44,  /* (24) terminal_expression ::= terminal_expression TOKEN_PLUS */
    44,  /* (25) terminal_expression ::= terminal_expression terminal_repetition_range */
    44,  /* (26) terminal_expression ::= range_expression */
    46,  /* (27) range_expression ::= TOKEN_OPEN_SQUARE range_literal TOKEN_DOUBLE_DOT range_literal TOKEN_CLOSE_SQUARE */
    47,  /* (28) range_literal ::= TOKEN_STRING_LITERAL */
    47,  /* (29) range_literal ::= TOKEN_INTEGER_LITERAL */
    45,  /* (30) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
    45,  /* (31) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
    45,  /* (32) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    45,  /* (33) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    45,  /* (34) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    37,  /* (35) rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
    37,  /* (36) rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_OPEN_ANGLE rule_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
    49,  /* (37) rule_parameter_list ::= rule_parameter_list TOKEN_COMMA rule_parameter */
    49,  /* (38) rule_parameter_list ::= rule_parameter */
    50,  /* (39) rule_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
    50,  /* (40) rule_parameter ::= TOKEN_KW_PARAMETER_UNPACK TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
    50,  /* (41) rule_parameter ::= TOKEN_KW_PARAMETER_HIDE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
    50,  /* (42) rule_parameter ::= TOKEN_KW_PARAMETER_MERGE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
    48,  /* (43) rule_definition ::= rule_choices */
    51,  /* (44) rule_choices ::= rule_sequence */
    51,  /* (45) rule_choices ::= rule_choices TOKEN_PIPE rule_sequence */
    52,  /* (46) rule_sequence ::= rule_expression */
    52,  /* (47) rule_sequence ::= rule_sequence rule_expression */
    53,  /* (48) rule_expression ::= TOKEN_RULE_IDENTIFIER */
    53,  /* (49) rule_expression ::= TOKEN_TERMINAL_IDENTIFIER */
    53,  /* (50) rule_expression ::= TOKEN_STRING_LITERAL */
    53,  /* (51) rule_expression ::= TOKEN_INTEGER_LITERAL */
    53,  /* (52) rule_expression ::= TOKEN_EPSILON */
    53,  /* (53) rule_expression ::= TOKEN_OPEN_PAREN rule_choices TOKEN_CLOSE_PAREN */
    53,  /* (54) rule_expression ::= rule_expression TOKEN_QUESTION */
    53,  /* (55) rule_expression ::= rule_expression TOKEN_STAR */
    53,  /* (56) rule_expression ::= rule_expression TOKEN_PLUS */
    53,  /* (57) rule_expression ::= rule_expression rule_repetition_range */
    54,  /* (58) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
    54,  /* (59) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
    54,  /* (60) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    54,  /* (61) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    54,  /* (62) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
    38,  /* (63) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL TOKEN_SEMICOLON */
    38,  /* (64) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_START TOKEN_EQUALS TOKEN_RULE_IDENTIFIER TOKEN_SEMICOLON */
    38,  /* (65) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_COMMENT TOKEN_EQUALS terminal_definition_pair_list TOKEN_SEMICOLON */
    38,  /* (66) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_NEWLINE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
    38,  /* (67) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
    55,  /* (68) terminal_definition_pair_list ::= terminal_definition_pair */
    55,  /* (69) terminal_definition_pair_list ::= terminal_definition_pair_list TOKEN_COMMA terminal_definition_pair */
    56,  /* (70) terminal_definition_pair ::= TOKEN_OPEN_BRACE terminal_definition TOKEN_COMMA terminal_definition TOKEN_CLOSE_BRACE */
};

/* For rule J, yyRuleInfoNRhs[J] contains the negative of the number
** of symbols on the right-hand side of that rule. */
static const signed char yyRuleInfoNRhs[] = {
   -1,  /* (0) grammar ::= statement_list */
   -2,  /* (1) statement_list ::= statement_list statement */
   -1,  /* (2) statement_list ::= statement */
   -1,  /* (3) statement ::= terminal_production */
   -1,  /* (4) statement ::= rule_production */
   -1,  /* (5) statement ::= grammar_property */
   -4,  /* (6) terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
   -7,  /* (7) terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_OPEN_ANGLE terminal_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
   -3,  /* (8) terminal_parameter_list ::= terminal_parameter_list TOKEN_COMMA terminal_parameter */
   -1,  /* (9) terminal_parameter_list ::= terminal_parameter */
   -3,  /* (10) terminal_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
   -3,  /* (11) terminal_parameter ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
   -1,  /* (12) terminal_definition ::= terminal_choices */
   -1,  /* (13) terminal_choices ::= terminal_sequence */
   -3,  /* (14) terminal_choices ::= terminal_choices TOKEN_PIPE terminal_sequence */
   -1,  /* (15) terminal_sequence ::= terminal_expression */
   -2,  /* (16) terminal_sequence ::= terminal_sequence terminal_expression */
   -1,  /* (17) terminal_expression ::= TOKEN_TERMINAL_IDENTIFIER */
   -1,  /* (18) terminal_expression ::= TOKEN_STRING_LITERAL */
   -1,  /* (19) terminal_expression ::= TOKEN_INTEGER_LITERAL */
   -1,  /* (20) terminal_expression ::= TOKEN_EPSILON */
   -3,  /* (21) terminal_expression ::= TOKEN_OPEN_PAREN terminal_choices TOKEN_CLOSE_PAREN */
   -2,  /* (22) terminal_expression ::= terminal_expression TOKEN_QUESTION */
   -2,  /* (23) terminal_expression ::= terminal_expression TOKEN_STAR */
   -2,  /* (24) terminal_expression ::= terminal_expression TOKEN_PLUS */
   -2,  /* (25) terminal_expression ::= terminal_expression terminal_repetition_range */
   -1,  /* (26) terminal_expression ::= range_expression */
   -5,  /* (27) range_expression ::= TOKEN_OPEN_SQUARE range_literal TOKEN_DOUBLE_DOT range_literal TOKEN_CLOSE_SQUARE */
   -1,  /* (28) range_literal ::= TOKEN_STRING_LITERAL */
   -1,  /* (29) range_literal ::= TOKEN_INTEGER_LITERAL */
   -3,  /* (30) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
   -4,  /* (31) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
   -4,  /* (32) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
   -5,  /* (33) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
   -3,  /* (34) terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
   -4,  /* (35) rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
   -7,  /* (36) rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_OPEN_ANGLE rule_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
   -3,  /* (37) rule_parameter_list ::= rule_parameter_list TOKEN_COMMA rule_parameter */
   -1,  /* (38) rule_parameter_list ::= rule_parameter */
   -3,  /* (39) rule_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
   -3,  /* (40) rule_parameter ::= TOKEN_KW_PARAMETER_UNPACK TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
   -3,  /* (41) rule_parameter ::= TOKEN_KW_PARAMETER_HIDE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
   -3,  /* (42) rule_parameter ::= TOKEN_KW_PARAMETER_MERGE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */
   -1,  /* (43) rule_definition ::= rule_choices */
   -1,  /* (44) rule_choices ::= rule_sequence */
   -3,  /* (45) rule_choices ::= rule_choices TOKEN_PIPE rule_sequence */
   -1,  /* (46) rule_sequence ::= rule_expression */
   -2,  /* (47) rule_sequence ::= rule_sequence rule_expression */
   -1,  /* (48) rule_expression ::= TOKEN_RULE_IDENTIFIER */
   -1,  /* (49) rule_expression ::= TOKEN_TERMINAL_IDENTIFIER */
   -1,  /* (50) rule_expression ::= TOKEN_STRING_LITERAL */
   -1,  /* (51) rule_expression ::= TOKEN_INTEGER_LITERAL */
   -1,  /* (52) rule_expression ::= TOKEN_EPSILON */
   -3,  /* (53) rule_expression ::= TOKEN_OPEN_PAREN rule_choices TOKEN_CLOSE_PAREN */
   -2,  /* (54) rule_expression ::= rule_expression TOKEN_QUESTION */
   -2,  /* (55) rule_expression ::= rule_expression TOKEN_STAR */
   -2,  /* (56) rule_expression ::= rule_expression TOKEN_PLUS */
   -2,  /* (57) rule_expression ::= rule_expression rule_repetition_range */
   -3,  /* (58) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
   -4,  /* (59) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
   -4,  /* (60) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
   -5,  /* (61) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
   -3,  /* (62) rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
   -4,  /* (63) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL TOKEN_SEMICOLON */
   -4,  /* (64) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_START TOKEN_EQUALS TOKEN_RULE_IDENTIFIER TOKEN_SEMICOLON */
   -4,  /* (65) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_COMMENT TOKEN_EQUALS terminal_definition_pair_list TOKEN_SEMICOLON */
   -4,  /* (66) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_NEWLINE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
   -4,  /* (67) grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
   -1,  /* (68) terminal_definition_pair_list ::= terminal_definition_pair */
   -3,  /* (69) terminal_definition_pair_list ::= terminal_definition_pair_list TOKEN_COMMA terminal_definition_pair */
   -5,  /* (70) terminal_definition_pair ::= TOKEN_OPEN_BRACE terminal_definition TOKEN_COMMA terminal_definition TOKEN_CLOSE_BRACE */
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
**
** The yyLookahead and yyLookaheadToken parameters provide reduce actions
** access to the lookahead token (if any).  The yyLookahead will be YYNOCODE
** if the lookahead token has already been consumed.  As this procedure is
** only called from one place, optimizing compilers will in-line it, which
** means that the extra parameters have no performance impact.
*/
static YYACTIONTYPE yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno,       /* Number of the rule by which to reduce */
  int yyLookahead,             /* Lookahead token, or YYNOCODE if none */
  const yy_fix_type<ParseTOKENTYPE>::type &yyLookaheadToken  /* Value of the lookahead token */
  ParseCTX_PDECL                   /* %extra_context */
){
  int yygoto;                     /* The next state */
  YYACTIONTYPE yyact;             /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH
  (void)yyLookahead;
  (void)yyLookaheadToken;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfoNRhs[yyruleno];
    if( yysize ){
      fprintf(yyTraceFILE, "%sReduce %d [%s], go to state %d.\n",
        yyTracePrompt,
        yyruleno, yyRuleName[yyruleno], yymsp[yysize].stateno);
    }else{
      fprintf(yyTraceFILE, "%sReduce %d [%s].\n",
        yyTracePrompt, yyruleno, yyRuleName[yyruleno]);
    }
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfoNRhs[yyruleno]==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yytos>=yypParser->yystackEnd ){
      yyStackOverflow(yypParser);
      /* The call to yyStackOverflow() above pops the stack until it is
      ** empty, causing the main parser loop to exit.  So the return value
      ** is never used and does not matter. */
      return 0;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        /* The call to yyStackOverflow() above pops the stack until it is
        ** empty, causing the main parser loop to exit.  So the return value
        ** is never used and does not matter. */
        return 0;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
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
  auto &A=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 46 "grm_parser.y"
{
 *ast_ = std::move(A);
 (*ast_)->set_id(pmt::parserbuilder::GrmAst::NtGrammar);
}
#line 1467 "grm_parser.cpp"
  yy_destructor(A);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
}
        break;
      case 1: /* statement_list ::= statement_list statement */
      case 16: /* terminal_sequence ::= terminal_sequence terminal_expression */ yytestcase(yyruleno==16);
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 52 "grm_parser.y"
{
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}
#line 1483 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 2: /* statement_list ::= statement */
      case 9: /* terminal_parameter_list ::= terminal_parameter */ yytestcase(yyruleno==9);
      case 38: /* rule_parameter_list ::= rule_parameter */ yytestcase(yyruleno==38);
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 57 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children);
 A->give_child_at_back(std::move(B));
}
#line 1500 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 3: /* statement ::= terminal_production */
      case 4: /* statement ::= rule_production */ yytestcase(yyruleno==4);
      case 5: /* statement ::= grammar_property */ yytestcase(yyruleno==5);
      case 17: /* terminal_expression ::= TOKEN_TERMINAL_IDENTIFIER */ yytestcase(yyruleno==17);
      case 18: /* terminal_expression ::= TOKEN_STRING_LITERAL */ yytestcase(yyruleno==18);
      case 19: /* terminal_expression ::= TOKEN_INTEGER_LITERAL */ yytestcase(yyruleno==19);
      case 20: /* terminal_expression ::= TOKEN_EPSILON */ yytestcase(yyruleno==20);
      case 26: /* terminal_expression ::= range_expression */ yytestcase(yyruleno==26);
      case 28: /* range_literal ::= TOKEN_STRING_LITERAL */ yytestcase(yyruleno==28);
      case 29: /* range_literal ::= TOKEN_INTEGER_LITERAL */ yytestcase(yyruleno==29);
      case 46: /* rule_sequence ::= rule_expression */ yytestcase(yyruleno==46);
      case 48: /* rule_expression ::= TOKEN_RULE_IDENTIFIER */ yytestcase(yyruleno==48);
      case 49: /* rule_expression ::= TOKEN_TERMINAL_IDENTIFIER */ yytestcase(yyruleno==49);
      case 50: /* rule_expression ::= TOKEN_STRING_LITERAL */ yytestcase(yyruleno==50);
      case 51: /* rule_expression ::= TOKEN_INTEGER_LITERAL */ yytestcase(yyruleno==51);
      case 52: /* rule_expression ::= TOKEN_EPSILON */ yytestcase(yyruleno==52);
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 63 "grm_parser.y"
{
 A = std::move(B);
}
#line 1528 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 6: /* terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 76 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 1546 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
}
        break;
      case 7: /* terminal_production ::= TOKEN_TERMINAL_IDENTIFIER TOKEN_OPEN_ANGLE terminal_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-5].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  auto &D=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 82 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->unpack(1);
 A->give_child_at_back(std::move(D));
}
#line 1570 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_destructor(D);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0), std::move(A));
}
        break;
      case 8: /* terminal_parameter_list ::= terminal_parameter_list TOKEN_COMMA terminal_parameter */
      case 14: /* terminal_choices ::= terminal_choices TOKEN_PIPE terminal_sequence */ yytestcase(yyruleno==14);
      case 37: /* rule_parameter_list ::= rule_parameter_list TOKEN_COMMA rule_parameter */ yytestcase(yyruleno==37);
      case 45: /* rule_choices ::= rule_choices TOKEN_PIPE rule_sequence */ yytestcase(yyruleno==45);
      case 69: /* terminal_definition_pair_list ::= terminal_definition_pair_list TOKEN_COMMA terminal_definition_pair */ yytestcase(yyruleno==69);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 91 "grm_parser.y"
{
 A = std::move(B);
 A->give_child_at_back(std::move(C));
}
#line 1592 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
}
        break;
      case 10: /* terminal_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
      case 11: /* terminal_parameter ::= TOKEN_KW_PARAMETER_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */ yytestcase(yyruleno==11);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 102 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 1611 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
}
        break;
      case 12: /* terminal_definition ::= terminal_choices */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 115 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinition);
 A->give_child_at_back(std::move(B));
}
#line 1626 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 13: /* terminal_choices ::= terminal_sequence */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 121 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalChoices);
 A->give_child_at_back(std::move(B));
}
#line 1640 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 15: /* terminal_sequence ::= terminal_expression */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 132 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalSequence);
 A->give_child_at_back(std::move(B));
}
#line 1654 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 21: /* terminal_expression ::= TOKEN_OPEN_PAREN terminal_choices TOKEN_CLOSE_PAREN */
      case 53: /* rule_expression ::= TOKEN_OPEN_PAREN rule_choices TOKEN_CLOSE_PAREN */ yytestcase(yyruleno==53);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 159 "grm_parser.y"
{
 A = std::move(B);
}
#line 1670 "grm_parser.cpp"
  yy_destructor(B);
}
        break;
      case 22: /* terminal_expression ::= terminal_expression TOKEN_QUESTION */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 163 "grm_parser.y"
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
#line 1697 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 23: /* terminal_expression ::= terminal_expression TOKEN_STAR */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 181 "grm_parser.y"
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
#line 1721 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 24: /* terminal_expression ::= terminal_expression TOKEN_PLUS */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 195 "grm_parser.y"
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
#line 1749 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 25: /* terminal_expression ::= terminal_expression terminal_repetition_range */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 213 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetition);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 1765 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 27: /* range_expression ::= TOKEN_OPEN_SQUARE range_literal TOKEN_DOUBLE_DOT range_literal TOKEN_CLOSE_SQUARE */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 224 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 1785 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
}
        break;
      case 30: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
      case 34: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */ yytestcase(yyruleno==34);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 240 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
}
#line 1802 "grm_parser.cpp"
  yy_destructor(B);
}
        break;
      case 31: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
      case 32: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */ yytestcase(yyruleno==32);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 245 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 1820 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
}
        break;
      case 33: /* terminal_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &D=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 257 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->give_child_at_back(std::move(D));
}
#line 1840 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_destructor(D);
}
        break;
      case 35: /* rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 270 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 1859 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
}
        break;
      case 36: /* rule_production ::= TOKEN_RULE_IDENTIFIER TOKEN_OPEN_ANGLE rule_parameter_list TOKEN_CLOSE_ANGLE TOKEN_EQUALS rule_definition TOKEN_SEMICOLON */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-5].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  auto &D=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 276 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalProduction);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->unpack(1);
 A->give_child_at_back(std::move(D));
}
#line 1883 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_destructor(D);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-6].minor.yy0), std::move(A));
}
        break;
      case 39: /* rule_parameter ::= TOKEN_KW_PARAMETER_ID TOKEN_EQUALS TOKEN_STRING_LITERAL */
      case 40: /* rule_parameter ::= TOKEN_KW_PARAMETER_UNPACK TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */ yytestcase(yyruleno==40);
      case 41: /* rule_parameter ::= TOKEN_KW_PARAMETER_HIDE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */ yytestcase(yyruleno==41);
      case 42: /* rule_parameter ::= TOKEN_KW_PARAMETER_MERGE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL */ yytestcase(yyruleno==42);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 296 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalParameter);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 1905 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0), std::move(A));
}
        break;
      case 43: /* rule_definition ::= rule_choices */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 321 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalDefinition);
 A->give_child_at_back(std::move(B));
}
#line 1920 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 44: /* rule_choices ::= rule_sequence */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 327 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalChoices);
 A->give_child_at_back(std::move(B));
}
#line 1934 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 47: /* rule_sequence ::= rule_sequence rule_expression */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 342 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalSequence);
 A->give_child_at_back(std::move(B));
 if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtNonterminalSequence) {
  A->unpack(A->get_children_size() - 1);
 }
 A->give_child_at_back(std::move(C));
 if (A->get_child_at(A->get_children_size() - 1)->get_id() == pmt::parserbuilder::GrmAst::NtNonterminalSequence) {
  A->unpack(A->get_children_size() - 1);
 }
}
#line 1956 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 54: /* rule_expression ::= rule_expression TOKEN_QUESTION */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 379 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "," "1"
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));
 
 auto rhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 rhs->set_string("10#1");
 repetition_info->give_child_at_back(std::move(rhs));

 A->give_child_at_back(std::move(repetition_info));
}
#line 1985 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 55: /* rule_expression ::= rule_expression TOKEN_STAR */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 397 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: ","
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");

 repetition_info->give_child_at_back(std::move(comma));
 A->give_child_at_back(std::move(repetition_info));
}
#line 2009 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 56: /* rule_expression ::= rule_expression TOKEN_PLUS */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 411 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));

 // Add repetition info: "1" ","
 auto repetition_info = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);

 auto lhs = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkIntegerLiteral);
 lhs->set_string("10#1");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(lhs));

 auto comma = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::String, pmt::parserbuilder::GrmAst::TkComma);
 comma->set_string(",");
 repetition_info->give_child_at(repetition_info->get_children_size(), std::move(comma));

 A->give_child_at_back(std::move(repetition_info));
}
#line 2037 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 57: /* rule_expression ::= rule_expression rule_repetition_range */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 429 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetition);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 2053 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0), std::move(A));
}
        break;
      case 58: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_CLOSE_BRACE */
      case 62: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */ yytestcase(yyruleno==62);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 436 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
}
#line 2071 "grm_parser.cpp"
  yy_destructor(B);
}
        break;
      case 59: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_CLOSE_BRACE */
      case 60: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */ yytestcase(yyruleno==60);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 441 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 2089 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
}
        break;
      case 61: /* rule_repetition_range ::= TOKEN_OPEN_BRACE TOKEN_INTEGER_LITERAL TOKEN_COMMA TOKEN_INTEGER_LITERAL TOKEN_CLOSE_BRACE */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  auto &D=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 453 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtNonterminalRepetitionRange);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
 A->give_child_at_back(std::move(D));
}
#line 2109 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_destructor(D);
}
        break;
      case 63: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_CASE_SENSITIVE TOKEN_EQUALS TOKEN_BOOLEAN_LITERAL TOKEN_SEMICOLON */
      case 64: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_START TOKEN_EQUALS TOKEN_RULE_IDENTIFIER TOKEN_SEMICOLON */ yytestcase(yyruleno==64);
      case 65: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_COMMENT TOKEN_EQUALS terminal_definition_pair_list TOKEN_SEMICOLON */ yytestcase(yyruleno==65);
      case 66: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_NEWLINE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */ yytestcase(yyruleno==66);
      case 67: /* grammar_property ::= TOKEN_GRAMMAR_PROPERTY_WHITESPACE TOKEN_EQUALS terminal_definition TOKEN_SEMICOLON */ yytestcase(yyruleno==67);
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 466 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtGrammarProperty);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 2132 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0), std::move(A));
}
        break;
      case 68: /* terminal_definition_pair_list ::= terminal_definition_pair */
{
  pmt::util::smrt::GenericAst::UniqueHandle A;
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
#line 497 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinitionPairList);
 A->give_child_at_back(std::move(B));
}
#line 2147 "grm_parser.cpp"
  yy_destructor(B);
  yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0), std::move(A));
}
        break;
      case 70: /* terminal_definition_pair ::= TOKEN_OPEN_BRACE terminal_definition TOKEN_COMMA terminal_definition TOKEN_CLOSE_BRACE */
{
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-2].minor.yy0));
  yy_destructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[0].minor.yy0));
  auto &A=yy_constructor<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-4].minor.yy0));
  auto &B=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-3].minor.yy0));
  auto &C=yy_cast<pmt::util::smrt::GenericAst::UniqueHandle>(std::addressof(yymsp[-1].minor.yy0));
#line 508 "grm_parser.y"
{
 A = pmt::util::smrt::GenericAst::construct(pmt::util::smrt::GenericAst::Tag::Children, pmt::parserbuilder::GrmAst::NtTerminalDefinitionPair);
 A->give_child_at_back(std::move(B));
 A->give_child_at_back(std::move(C));
}
#line 2166 "grm_parser.cpp"
  yy_destructor(B);
  yy_destructor(C);
}
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfoLhs)/sizeof(yyRuleInfoLhs[0]) );
  yygoto = yyRuleInfoLhs[yyruleno];
  yysize = yyRuleInfoNRhs[yyruleno];
  yyact = yy_find_reduce_action(yymsp[yysize].stateno,(YYCODETYPE)yygoto);

  /* There are no SHIFTREDUCE actions on nonterminals because the table
  ** generator has simplified them to pure REDUCE actions. */
  assert( !(yyact>YY_MAX_SHIFT && yyact<=YY_MAX_SHIFTREDUCE) );

  /* It is not possible for a REDUCE to be followed by an error */
  assert( yyact!=YY_ERROR_ACTION );

  yymsp += yysize+1;
  yypParser->yytos = yymsp;
  yymsp->stateno = (YYACTIONTYPE)yyact;
  yymsp->major = (YYCODETYPE)yygoto;
  yyTraceShift(yypParser, yyact, "... then shift");
  return yyact;
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH
  ParseCTX_FETCH
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
/************ End %parse_failure code *****************************************/
  ParseARG_STORE /* Suppress warning about unused %extra_argument variable */
  ParseCTX_STORE
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  yy_fix_type<ParseTOKENTYPE>::type &yyminor         /* The minor type of the error token */
){
  ParseARG_FETCH
  ParseCTX_FETCH
#define TOKEN yyminor
  //auto &TOKEN = yyminor;
/************ Begin %syntax_error code ****************************************/
#line 8 "grm_parser.y"

  throw std::runtime_error("Syntax error");
#line 2235 "grm_parser.cpp"
/************ End %syntax_error code ******************************************/
  ParseARG_STORE /* Suppress warning about unused %extra_argument variable */
  ParseCTX_STORE
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH
  ParseCTX_FETCH
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  ParseARG_STORE /* Suppress warning about unused %extra_argument variable */
  ParseCTX_STORE
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
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  //YYMINORTYPE yyminorunion;    /* lemon++ -- not needed for destructor */
  YYACTIONTYPE yyact;            /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser = (yyParser*)yyp;  /* The parser */
  ParseCTX_FETCH
  ParseARG_STORE

  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif

  yyact = yypParser->yytos->stateno;
#ifndef NDEBUG
  if( yyTraceFILE ){
    if( yyact < YY_MIN_REDUCE ){
      fprintf(yyTraceFILE,"%sInput '%s' in state %d\n",
              yyTracePrompt,yyTokenName[yymajor],yyact);
    }else{
      fprintf(yyTraceFILE,"%sInput '%s' with pending reduce %d\n",
              yyTracePrompt,yyTokenName[yymajor],yyact-YY_MIN_REDUCE);
    }
  }
#endif

  do{
    assert( yyact==yypParser->yytos->stateno );
    yyact = yy_find_shift_action((YYCODETYPE)yymajor,yyact);
    if( yyact >= YY_MIN_REDUCE ){
      // todo -- pass yyminor by reference.
      yyact = yy_reduce(yypParser,yyact-YY_MIN_REDUCE,yymajor,
                        yyminor ParseCTX_PARAM);
    }else if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,(YYCODETYPE)yymajor,std::forward<ParseTOKENTYPE>(yyminor));
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      break;
    }else if( yyact==YY_ACCEPT_ACTION ){
      yypParser->yytos--;
      yy_accept(yypParser);
      return;
    }else{
      assert( yyact == YY_ERROR_ACTION );
      //yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
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
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        //yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos > yypParser->yystack
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) > YY_MAX_SHIFTREDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos == yypParser->yystack || yymajor==0 ){
          //yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift_error(yypParser,yyact);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
      if( yymajor==YYNOCODE ) break;
      yyact = yypParser->yytos->stateno;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      //yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      break;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      //yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      break;
#endif
    }
  }while( yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    if (cDiv == '[') fprintf(yyTraceFILE,"[");
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}

/*
** Return the fallback token corresponding to canonical token iToken, or
** 0 if iToken has no fallback.
*/
int ParseFallback(int iToken){
#ifdef YYFALLBACK
  assert( iToken<(int)(sizeof(yyFallback)/sizeof(yyFallback[0])) );
  return yyFallback[iToken];
#else
  (void)iToken;
#endif
  return 0;
}
