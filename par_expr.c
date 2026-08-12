#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

EXPR_CONS_PTR rval_primary_expr ( const char *ident, SRC_POS_C pos ) {
  EXPR_CONS_PTR prval = NULL;
  assert( ident );
  
  prval = alloc_expr_cons( pos );
  if( prval ) {
    SYM_ENTITY_PTR psym = NULL;
    prval->pos = pos;
    prval->mnemonic = MNC_RVALUE;
    psym = find_symbol( ident );
    if( psym ) {
      TYPE_ENV_PTR penv_last = NULL;
      if( psym->kind == SYM_DECL ) {
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
	      if( pe ) {
		assert( strcmp( pe->decl.var.v.ident, ident ) == 0 );
		assert( pe->decl.var.plnk_symtbl );
		if( pe->decl.var.plnk_symtbl == psym->u.decl.u.variable.pvar ) {
		  prval->kids.body.refaddr.var = pe->decl.var.v;
		  prval->ptype = prval->kids.body.refaddr.var.ptype;
		} else
		  ;
	      } else
		;
	    }	    
	  }
	  break;
	case END_OF_DECL_KIND:
	  /* fall thru. */
	default:
	  assert( FALSE );
	}
	
      }
    } else
      err_nodef( ident, pos );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return NULL;
}
