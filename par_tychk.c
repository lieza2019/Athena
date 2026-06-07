#include <stdio.h>
#include <assert.h>
#include "athena.h"

TYPE_CONS_PTR tychk_decl_var ( STATEMENT_PTR pstmt, SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  TYPE_ENV_PTR pe_vardecl = NULL;
  assert( pstmt );
  
  pe_vardecl = alloc_type_env( pos );
  if( pe_vardecl ) {
    if( statements.plast ) {
      assert( statements.plast->penv );
      env_lnk( statements.plast->penv, pe_vardecl );
      ;
    } else
      assert( statements.phead );
    {
      TYPE_SUBST_PTR psubst = NULL;
      r = typecheck1( &psubst, pstmt, pos );
    }
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
