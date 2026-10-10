/*
  -----------
  (2026/9/14)
  a as poly;
  n as int := ++a;
  a : int := UNKNOWN_VALUE
  -----------
  #(2026/9/14)
  #substitution imposing is missing for the case of MNC_NEG,
  # MNC_PREDECR/MNC_PSTDECR, MNC_PREINCR/MNC_PSTINCR in ty_infer() @tychk.c
  #substitution hasn't applied current type-environment, to record latest
  # type for each type-variable.
  #Only the type-variables belonging to functions, should be generalized,
  # and instantiated also, in tychk.c
  -----------
  #(2026/9/21)
  #n as int;
  #a as int := ++n;
  #Assertion failed: ((pini->mnemonic == MNC_RVALUE) || (pini->mnemonic == MNC_CONST)), function show_var_decl, file misc.c, line 135.
  #Abort trap: 6
  -----------
  (2026/9/22)
  a as int;
  b as int := ++c;
  (2, 16): symbol c has no definition.
  Assertion failed: (EXAM_RVALUE_EXPR( pexpr )), function ty_infer, file tychk.c, line 859.
  Abort trap: 6
  -----------
  (2026/9/22)
  #a as string = 1;
  #symbol a isnt declared.
  #* n as int := "hello world.";
  #* TK_STR_LITERAL: hello world.
  #* (1, 2): type constraint mismatched on assignment from incompatible type.
  #* symbol a isnt declared.
  -----------
  (2026/9/23)
  allocation for the memory-area of ident, is different for each creation with SAME-NAME, calling in ty_infer().
  -----------
  (2026/9/27)
  error-msg emission with SIMPLE printf has no arguments for error described in TYCHK_RESULT_DESC, s.t.
   ptychk_res->reason = TYCON_ASGN_TYPEMISMATCH;
   ptychk_res->err_lv = COMP_ERROR_FATAL;
   ptychk_res->pexpr = pexp_inf;
   ptychk_res->errmsg = NULL;
   pexp_inf = NULL;
  -----------
  (2026/9/27)
  What's the meaning of TYPE_CONS.type.pstuck? Eliminate it if its not needed.
  -----------
  (2026/9/27)
  #cleaning up the redundant / obsolete code fragments in the files as fellows,
  -#Makefile   -#ath_expr.h -#ath_misc.h  -#ath_symtbl.h -#decl.c   -#lisp.c     -#misc.c     -#par_stmt.c  -#stmt.c   -#type.c
  -#ath_decl.h -#ath_lex.l  -#ath_parse.y -#ath_type.h   -#err.c    -#main.c     -#par_decl.c -#par_tychk.c -#symtbl.c
  -#ath_err.h  -#ath_mem.h  -#ath_stmt.h  -#athena.h     -#expr.c   -#mem.c      -#par_expr.c  -#tychk.c     runarg.txt
  -----------
  (2026/10/10)
  #proc int foo ( n as int ) {
  #  an as string;
  #  a as int;
  #assertion "statements.plast == (yyvsp[-1].pstmt_last)" failed: file "ath_parse.y", line 223, function "yyparse"
  -----------
  (2026/10/10)
  proc int foo ( n as int ) {
    m as int;
    a as string;
  assertion "statements.plast == (yyvsp[-1].pstmt_last)" failed: file "ath_parse.y", line 262, function "yyparse"
  Abort trap                 (core dumped) ./athena
  proc int foo() {
    m as int;
    n as int;
    a as string := "hello world.";
  TK_STR_LITERAL: hello world.
  }
  a : string := "hello world.":string
  -----------
  (2026/10/10)
  proc int baz( ) {
    a as string := "hello world.";;
  TK_STR_LITERAL: hello world.
  a : string := "hello world.":string
  [a_goto@:athena]$
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "athena.h"

extern int yyparse( void );
extern FILE *yyin;

int main ( void ) {
  int r = 0;
  FILE *fp_src = stdin;
  
  if( ! enter_scope() ) {
    SRC_POS_C pos = { -1, -1 };
    ath_abort( pos, ABORT_CANNOT_CREAT_SCOPE );
  }
  yyin = fp_src;  
  r = yyparse();  
  {
    const char var_print[] = "a";
    SYM_ENTITY_PTR psym = NULL;
    psym = find_symbol( var_print );
    if( psym ) {
      char sbuf[8 * 1024] = "";
      show_var_decl( sbuf, psym->u.decl.u.variable.pvar );
      printf( "%s\n", sbuf );
    } else
      printf( "symbol a isnt declared.\n " );
  }
  return r;
}
