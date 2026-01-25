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
  
  if( ! enter_scope() )
    ath_abort( ABORT_MEMLACK, -1, -1 );
  yyin = fp_src;
  r = yyparse();
  {
    SYM_ENTITY_PTR psym = NULL;
    psym = find_symbol( "a" );
    return r;
  }
}
