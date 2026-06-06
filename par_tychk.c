#include <stdio.h>
#include <assert.h>
#include "athena.h"

TYPE_CONS_PTR tychk_decl_var ( STATEMENT_PTR pstmt, SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  TYPE_SUBST_PTR psubst = NULL;
  assert( pstmt );
  
  r = typecheck1( &psubst, pstmt, pos );
  return r;
}

