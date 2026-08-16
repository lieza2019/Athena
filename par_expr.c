#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"
#include "y.tab.h"

EXPR_CONS_PTR rval_unary_expr ( EXPR_CONS_PTR pexpr, int unary_ope, SRC_POS_C pos ) {
  EXPR_CONS_PTR pe_una = NULL;
  assert( pexpr );
  pe_una = alloc_expr_cons( pos );
  if( pe_una ) {
    pe_una->pos = pos;
    switch( unary_ope ) {
    case TK_DECL:
      pe_una->mnemonic = MNC_DECL;
      pe_una->kids.pleft = pexpr;
      break;
    case TK_INCL:
      pe_una->mnemonic = MNC_INCL;
      pe_una->kids.pleft = pexpr;
      break;
    default:
      assert( FALSE );
    }
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return pe_una;
}

EXPR_CONS_PTR rval_primary_expr ( const char *ident, SRC_POS_C pos ) {
  EXPR_CONS_PTR pe_pri = NULL;
  assert( ident );
  
  pe_pri = alloc_expr_cons( pos );
  if( pe_pri ) {
    BOOL found = FALSE;
    SYM_ENTITY_PTR psym = NULL;
    pe_pri->pos = pos;
    pe_pri->mnemonic = MNC_RVALUE;
    psym = find_symbol( ident );
    while( psym ) {
      if( psym->kind == SYM_DECL ) {
	TYPE_ENV_PTR penv_last = NULL;
	DECLARATION_PTR pdecl = &psym->u.decl;
	assert( pdecl->ident );
	assert( strcmp( pdecl->ident, ident ) == 0 );
	assert( statements.phead && statements.plast ); // for psym->kind == SYM_DECL.
	penv_last = (statements.plast)->penv;
	switch( pdecl->kind ) {
	case DECL_FUN:
	  break;
	case DECL_VAR:
	  assert( psym->u.decl.u.variable.pvar );
	  {
	    VAR_ATTRIB_PTR pvar = pdecl->u.variable.pvar;
	    assert( pvar->ident );
	    assert( strcmp( pvar->ident, pdecl->ident ) == 0 );
	    {
	      TYENV_ELEM_PTR pe = NULL;
	      pe = env_lkup( penv_last, ident );
	      while( pe ) {
		assert( strcmp( pe->decl.var.v.ident, ident ) == 0 );
		assert( pe->decl.var.plnk_symtbl );
		if( pe->decl.var.plnk_symtbl == psym->u.decl.u.variable.pvar ) {
		  pe_pri->kids.body.refaddr.var = pe->decl.var.v;
		  pe_pri->ptype = pe_pri->kids.body.refaddr.var.ptype;
		  found = TRUE;
		  break;
		}
		pe = NULL;
		assert( penv_last );
		if( penv_last->uplink )
		  pe = env_lkup( penv_last->uplink, ident );
	      }
	    }
	  }
	  break;
	case END_OF_DECL_KIND:
	  /* fall thru. */
	default:
	  assert( FALSE );
	}
	if( found )
	  break;
      }
      psym = find_symbol_again( psym, ident );
    }
    if( !psym )
      err_nodef( ident, pos );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return pe_pri;
}
