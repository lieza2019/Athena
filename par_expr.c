/* purged, 2026/9/12 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"
#include "y.tab.h"

EXPR_CONS_PTR rval_binary_expr ( EXPR_CONS_PTR pexpr1, EXPR_CONS_PTR pexpr2, int binary_ope,
				 SRC_POS_C pos ) {
  EXPR_CONS_PTR pe_bin = NULL;
  assert( pexpr1 );
  assert( pexpr2 );
  pe_bin = alloc_expr_cons( pos );
  if( pe_bin ) {
    TYCHK_RESULT_DESC tychk_res = { TYCON_WELLTYPED };
    TYPE_SUBST_PTR psubst = NULL;
    switch( binary_ope ) {
    case TK_STAR:
      pe_bin->mnemonic = MNC_MUL;
      pe_bin->kids.pleft = pexpr1;
      pe_bin->kids.pright = pexpr2;
      break;
    case TK_SLASH:
      pe_bin->mnemonic = MNC_DIV;
      pe_bin->kids.pleft = pexpr1;
      pe_bin->kids.pright = pexpr2;
      break;
    case TK_CROSS:
      pe_bin->mnemonic = MNC_ADD;
      pe_bin->kids.pleft = pexpr1;
      pe_bin->kids.pright = pexpr2;
      break;
    case TK_MINUS:
      pe_bin->mnemonic = MNC_SUB;
      pe_bin->kids.pleft = pexpr1;
      pe_bin->kids.pright = pexpr2;
      break;
    default:
      assert( FALSE );
    }
    {
      EXPR_CONS_PTR pe_b = NULL;
#if 0 // *****
      pe_b = ty_infer( &tychk_res, &psubst,(statements.plast ? &(statements.plast)->penv : NULL),
		       pe_bin, pos );
#else
      TYPE_ENV_PTR penv = NULL;
      if( statements.plast )
	penv = (statements.plast)->penv;
      else {
	penv = alloc_type_env( pos );
	if( !penv )
	  goto failed_memalloc;
      }
      assert( penv );
      pe_b = ty_infer( &tychk_res, &psubst, &penv, pe_bin, pos );
#endif
      if( pe_b )
	pe_bin = pe_b;
      else {
	assert( tychk_res.reason != TYCON_WELLTYPED );
	pe_bin->ptype = alloc_type_cons( pos );
	if( pe_bin->ptype )
	  (pe_bin->ptype)->type.ty = TY_INT;
	else
	  goto failed_memalloc;
      }
      if( tychk_res.reason != TYCON_WELLTYPED ) {
	ERRMSG_TYCON_MISMATCH( &tychk_res, pos );
      }
    }
    assert( pe_bin->ptype );
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pe_bin;
}

EXPR_CONS_PTR rval_unary_expr ( EXPR_CONS_PTR pexpr, int unary_ope, SRC_POS_C pos ) {
  EXPR_CONS_PTR pe_una = NULL;
  assert( pexpr );
  
  pe_una = alloc_expr_cons( pos );
  if( pe_una ) {
    TYCHK_RESULT_DESC tychk_res = { TYCON_WELLTYPED };
    TYPE_SUBST_PTR psubst = NULL;
    pe_una->pos = pos;
    switch( unary_ope ) {
    case TK_MINUS:
      pe_una->mnemonic = MNC_NEG;
      pe_una->kids.pleft = pexpr;
      break;
    case TK_PREDECR:
      /* fall thru. */
    case TK_PSTDECR:
      if( unary_ope == TK_PSTDECR )
	pe_una->mnemonic = MNC_PSTDECR;
      else {
	assert( unary_ope == TK_PREDECR );
	pe_una->mnemonic = MNC_PREDECR;
      }
      pe_una->kids.pleft = pexpr;
      break;
    case TK_PREINCR:
      /* fall thru. */
    case TK_PSTINCR:
      if( unary_ope == TK_PSTINCR )
	pe_una->mnemonic = MNC_PSTINCR;
      else {
	assert( unary_ope == TK_PREINCR );
	pe_una->mnemonic = MNC_PREINCR;
      }
      pe_una->kids.pleft = pexpr;
      break;
    default:
      assert( FALSE );
    }
    {
      EXPR_CONS_PTR pe_u = NULL;
#if 0 // *****
      pe_u = ty_infer( &tychk_res, &psubst, (statements.plast ? &(statements.plast)->penv : NULL),
		       pe_una, pos );
#else
      TYPE_ENV_PTR penv = NULL;
      if( statements.plast )
	penv = (statements.plast)->penv;
      else {
	penv = alloc_type_env( pos );
	if( !penv )
	  goto failed_memalloc;
      }
      assert( penv );
      pe_u = ty_infer( &tychk_res, &psubst, &penv, pe_una, pos );
#endif
      if( pe_u )
	pe_una = pe_u;
      else {
	assert( tychk_res.reason != TYCON_WELLTYPED );
	pe_una->ptype = alloc_type_cons( pos );
	if( pe_una->ptype )
	  (pe_una->ptype)->type.ty = TY_INT;
	else
	  goto failed_memalloc;
      }
      if( tychk_res.reason != TYCON_WELLTYPED ) {
	ERRMSG_TYCON_MISMATCH( &tychk_res, pos );
      }
    }
    assert( pe_una->ptype );
  } else
  failed_memalloc:
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
