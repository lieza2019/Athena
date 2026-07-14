#include <stdio.h>
#include <assert.h>
#include "athena.h"

TYPE_CONS_PTR tychk_decl_var ( STATEMENT_PTR pstmt, SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  TYPE_ENV_PTR pe_vardecl = NULL;
  assert( pstmt );
  assert( pstmt->sort == STMT_DECL );
  assert( pstmt->u.pdecl );
  assert( (pstmt->u.pdecl)->kind == DECL_VAR );
  assert( (pstmt->u.pdecl)->u.variable.pvar );
  
  pe_vardecl = alloc_type_env( pos );
  if( pe_vardecl ) {
    if( statements.plast ) {
      assert( statements.phead );
      env_lnk( statements.plast->penv, pe_vardecl );
    } else
      assert( ! statements.phead );
    pstmt->penv = pe_vardecl;
    assert( (pstmt->u.pdecl->u.variable.pvar)->ident );
    assert( (pstmt->u.pdecl->u.variable.pvar)->ptype );
    ty_curve( (pstmt->u.pdecl->u.variable.pvar)->ptype, pos );
    env_add( pstmt->penv, (pstmt->u.pdecl->u.variable.pvar)->ident,
	     (pstmt->u.pdecl->u.variable.pvar)->ptype, pos );
    assert( pstmt->penv );
    assert( env_lkup( pstmt->penv, (pstmt->u.pdecl->u.variable.pvar)->ident ) );
    r = typecheck( pstmt, pos );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
