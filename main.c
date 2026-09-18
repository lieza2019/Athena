/*
  -----------
  (2026/9/14)
  a as poly;
  n as int := ++a;
  a := UNKNOWN_VALUE:poly
  -----------
  (2026/9/14)
  substitution imposing is missing for the case of MNC_NEG,
   MNC_PREDECR/MNC_PSTDECR, MNC_PREINCR/MNC_PSTINCR in ty_infer() @tychk.c
  substitution hasn't applied current type-environment, to record latest
   type for each type-variable.
  Only the type-variables belonging to functions, should be generalized,
   and instantiated also, in tychk.c
  -----------
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
