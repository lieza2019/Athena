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
    SYM_ENTITY_PTR psym = NULL;
    psym = find_symbol( "a" );
    if( psym ) {
      char sbuf[8 * 1024] = "";
#if 0
      show_var_type( sbuf, &psym->u.decl.u.var );
      printf( "type of a: %s\n", sbuf );
#else
      show_var_decl( sbuf, &psym->u.decl.u.var );
      printf( "%s\n", sbuf );
#endif
    } else
      printf( "symbol a isnt declared.\n " );
  }
  return r;
}
