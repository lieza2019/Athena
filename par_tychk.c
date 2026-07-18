#include <stdio.h>
#include <string.h>
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
      env_link( statements.plast->penv, pe_vardecl );
    } else
      assert( ! statements.phead );
    pstmt->penv = pe_vardecl;
    assert( (pstmt->u.pdecl->u.variable.pvar)->ident );
    assert( (pstmt->u.pdecl->u.variable.pvar)->ptype );
    ty_curve( (pstmt->u.pdecl->u.variable.pvar)->ptype, pos );
    env_add( pstmt->penv, pstmt->u.pdecl, pos );
    assert( pstmt->penv );
    assert( env_lkup( pstmt->penv, (pstmt->u.pdecl->u.variable.pvar)->ident ) );
    r = typecheck( pstmt, pos );
    assert( pstmt );
    assert( pstmt->penv );
    if( r ) {
      TYENV_ELEM_PTR pe = (pstmt->penv)->pmappings;
      while( pe ) {
	assert( pe->decl.var.plnk_symtbl );
	assert( pe->decl.var.v.ident );
	assert( pe->decl.var.v.ptype );
	(pe->decl.var.plnk_symtbl)->ptype = pe->decl.var.v.ptype;
	pe = pe->pnext;
      }
#if 1 // for temoral debugging.
      {
	const char *var_id = "a";
	SYM_ENTITY_PTR psym = NULL;
	TYPE_ENV_PTR pev = pstmt->penv;
	BOOL found = FALSE;
	while( pev ) {
	  TYENV_ELEM_PTR pe = pev->pmappings;
	  while( pe ) {
	    if( strcmp( pe->decl.var.v.ident, var_id ) == 0 ) {
	      found = TRUE;
	      break;
	    }
	    pe = pe->pnext;
	  }
	  pev = pev->uplink;
	}
	if( found ) {
	  psym = find_symbol( var_id );
	  if( psym ) {
	    assert( TRUE );
	  }
	}
      }
#endif
    }
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
