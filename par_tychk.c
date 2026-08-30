#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"
#include "y.tab.h"

static void reveal_env ( TYPE_ENV_PTR penv_stmt ) {
  TYPE_ENV_PTR penv = penv_stmt;
  while( penv ) {
    TYENV_ELEM_PTR pelem = penv->pmappings;
    while( pelem ) {
      assert( pelem->decl.var.v.ident );
      assert( pelem->decl.var.v.ptype );
      assert( pelem->decl.var.plnk_symtbl );
      (pelem->decl.var.plnk_symtbl)->ptype = pelem->decl.var.v.ptype;
      pelem = pelem->pnext;
    }
    penv = penv->uplink;
  }
}

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
    TYCON_MISMATCH_REASON reason = TYCON_WELLTYPED;
    TYPE_SUBST_PTR psubst = NULL;
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
#if 0 // *****
    r = typecheck( pstmt, pos );
#else
    r = tyinf_decl_var( &reason, &psubst, &pstmt->penv, (pstmt->u.pdecl)->u.variable.pvar, pos );
#endif
    assert( pstmt );
    assert( pstmt->penv );
    if( r ) {
#if 0 // *****
      TYENV_ELEM_PTR pe = (pstmt->penv)->pmappings;
      while( pe ) {
	assert( pe->decl.var.plnk_symtbl );
	assert( pe->decl.var.v.ident );
	assert( pe->decl.var.v.ptype );
	(pe->decl.var.plnk_symtbl)->ptype = pe->decl.var.v.ptype;
	pe = pe->pnext;
      }
#else
      if( reason != TYCON_WELLTYPED ) {
	assert( ((pstmt->u.pdecl)->u.variable.pvar)->pinit );
	//ERRMSG_TYCON_MISMATCH( reason, ((pstmt->u.pdecl)->u.variable.pvar)->pinit, pos );
      }
      reveal_env( pstmt->penv );
#endif
#if 1 // for use of temporal debugging.
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
	  if( !found )
	    pev = pev->uplink;
	  else
	    break;
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
