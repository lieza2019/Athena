#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

static BOOL chk_tyvar_occur ( char *tyvar_ident, TYPE_CONS_PTR pty ) {
  BOOL r = FALSE;
  assert( tyvar_ident );
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_LTE_VAR:
    assert( pty->attrs.lte.pln_var );
    assert( ((VAR_ATTRIB_PTR)pty->attrs.lte.pln_var)->ptype );
    r = chk_tyvar_occur( tyvar_ident, ((VAR_ATTRIB_PTR)pty->attrs.lte.pln_var)->ptype );
    break;
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    r = TRUE;
    break;
  case TY_LIST:
    assert( pty->attrs.list.pty_elem );
    r = chk_tyvar_occur( tyvar_ident, pty->attrs.list.pty_elem );
    break;
  case TY_POLY:
    assert( pty->type.tyvars.var.ident );
    r = (strcmp( pty->type.tyvars.var.ident, tyvar_ident ) == 0);
    break;
  case TY_GEN:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return r;
}

static BOOL unif_mkequ ( TYPE_SUBST_PTR ps_unif, TYPE_CONS_PTR pvar, TYPE_CONS_PTR pty_equ, SRC_POS_C pos ) {
  BOOL r = FALSE;
  assert( ps_unif );  
  assert( pty_equ );
  
  assert( pvar );
  assert( pvar->type.ty == TY_POLY );
  assert( pvar->type.tyvars.var.ident );
  if( chk_tyvar_occur( pvar->type.tyvars.var.ident, pty_equ ) ) {
    subst_add( ps_unif, pvar->type.tyvars.var.ident, pty_equ, pos );
    r = TRUE;
  }
  return r;
}

BOOL ty_unify ( TYPE_SUBST_PTR *pps_unif, TYPE_CONS_PTR pty_1, TYPE_CONS_PTR pty_2, SRC_POS_C pos ) {
  BOOL r = FALSE;
  assert( pps_unif );
  assert( pty_1 );
  assert( pty_2 );
  
  *pps_unif = NULL;
  switch( pty_1->type.ty ) {
  case TY_LTE_VAR:
    assert( pty_1->attrs.lte.pln_var );
    assert( ((VAR_ATTRIB_PTR)pty_1->attrs.lte.pln_var)->ptype );
    ty_unify( pps_unif, ((VAR_ATTRIB_PTR)pty_1->attrs.lte.pln_var)->ptype, pty_2, pos );
    break;
  case TY_INT:
    r = (pty_2->type.ty == TY_INT);
    if( !r ) {
      if( pty_2->type.ty == TY_POLY ) {
	assert( pty_2->type.tyvars.var.ident );
	*pps_unif = alloc_type_subst( pos );
	if( *pps_unif )
	  r = unif_mkequ( *pps_unif, pty_2, pty_1, pos );
	else
	  ath_abort( pos, ABORT_MEMLACK );
      }
    }
    break;
  case TY_CHAR:
    r = (pty_2->type.ty == TY_CHAR);
    if( !r ) {
      if( pty_2->type.ty == TY_POLY ) {
	assert( pty_2->type.tyvars.var.ident );
	*pps_unif = alloc_type_subst( pos );
	if( *pps_unif )
	  r = unif_mkequ( *pps_unif, pty_2, pty_1, pos );
	else
	  ath_abort( pos, ABORT_MEMLACK );
      }
    }
    break;
  case TY_STRING:
    r = (pty_2->type.ty == TY_STRING);
    if( !r ) {
      if( pty_2->type.ty == TY_POLY ) {
	assert( pty_2->type.tyvars.var.ident );
	*pps_unif = alloc_type_subst( pos );
	if( *pps_unif )
	  r = unif_mkequ( *pps_unif, pty_2, pty_1, pos );
	else
	  ath_abort( pos, ABORT_MEMLACK );
      }
    }
  case TY_LIST:
    assert( pty_1->attrs.list.pty_elem );
    if( pty_2->type.ty == TY_LIST ) {
      assert( pty_2->attrs.list.pty_elem );
      r = ty_unify( pps_unif, pty_1->attrs.list.pty_elem, pty_2->attrs.list.pty_elem, pos );
    }
    break;
  case TY_POLY:
    assert( pty_1->type.tyvars.var.ident );
    r = unif_mkequ( *pps_unif, pty_1, pty_2, pos );
    break;
  case TY_GEN:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return r;
}

static TYPE_CONS_PTR infer ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_expr = NULL;
  assert( penv );
  assert( pexpr );
  switch( pexpr->mnemonic ) {
  case MNC_CALL:
    break;
  case MNC_ASGN:
    {
      TYPE_SUBST_PTR pS_l = NULL;
      TYPE_CONS_PTR pty_l = NULL;
      TYPE_SUBST_PTR pS_r = NULL;
      TYPE_CONS_PTR pty_r = NULL;
      pty_l = infer( &pS_l, penv, pexpr->kids.pleft, pos );
      {
	TYPE_ENV_PTR penv_r = NULL;
	penv_r = dup_env( penv, pos );
	if( penv_r ) {
	  pty_r = infer( &pS_r, penv_r, pexpr->kids.pright, pos );
	} else
	  ;
      }
      ;
    }
    break;
  case MNC_ARITH:
    break;
  case MNC_LVALUE:
    break;
  case MNC_RVALUE:
    break;
  case END_OF_MNEMONIC_CODE:
    /* fall thru. */
  default:
    break;
  }
  return pty_expr;
}

TYPE_CONS_PTR typematch ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, STATEMENT_PTR pstmt, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_stmt = NULL;
  assert( penv );
  assert( pstmt );
  switch( pstmt->sort ) {
  case STMT_DECL:
    assert( pstmt->u.pdecl );
    assert( (pstmt->u.pdecl)->pinit );
    infer( NULL, penv, (pstmt->u.pdecl)->pinit, pos );
    break;
  case STMT_EXPR:
    infer( NULL, penv, pstmt->u.pexpr, pos );
    break;
  case END_OF_STMT_SORT:
    /* fall thru. */
  default:
    assert( FALSE );    
  }
  return pty_stmt;
}

BOOL typecheck ( TYPE_CONS_PTR_C pty1, TYPE_CONS_PTR_C pty2 ) {
  BOOL r = FALSE;
  
  assert( pty1 );
  assert( pty2 );
  if( pty1->type.ty == TY_LIST ) {
    if( pty2->type.ty == TY_LIST ) {
      assert( pty1->attrs.list.pty_elem );
      assert( pty2->attrs.list.pty_elem );
      r = typecheck( pty1->attrs.list.pty_elem, pty2->attrs.list.pty_elem );
    }
  } else {
    assert( pty1->type.ty != TY_LIST );
    switch( pty1->type.ty ) {
    case TY_INT:
    case TY_STRING:
      r = (pty1->type.ty == pty2->type.ty);
      break;
    default:
      assert( FALSE );
    }
  }
  return r;
}
