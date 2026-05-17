#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

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

static BOOL chk_tyvar_occur ( const char *tyvar_ident, TYPE_CONS_PTR pty ) {
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
  case TY_OTHERS:
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
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return r;
}

static TYPE_CONS_PTR ty_infer ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_expr = NULL;
  assert( ppsubst );
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
      pty_l = ty_infer( &pS_l, penv, pexpr->kids.pleft, pos );
      {
	TYPE_ENV_PTR penv_r = NULL;
	penv_r = dup_env( penv, pos );
	if( penv_r ) {
	  pty_r = ty_infer( &pS_r, penv_r, pexpr->kids.pright, pos );
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

static TYPE_CONS_PTR add_tv ( TYPE_CONS_PTR ptvs, const char *tv_ident, SRC_POS_C pos ) {
  BOOL found = FALSE;
  TYPE_CONS_PTR ptv = NULL;
  assert( tv_ident );
  
  ptv = ptvs;
  while( ptv ) {
    assert( ptv->type.ty == TY_OTHERS );
    assert( ptv->type.tyvars.var.ident );
    if( strcmp( ptv->type.tyvars.var.ident, tv_ident ) == 0 ) {
      found = TRUE;
      break;
    }
    ptv = ptv->type.tyvars.var.pnext;
  }
  if( !found ) {
    TYPE_CONS_PTR pnew = NULL;
    pnew = alloc_type_cons( pos );
    if( pnew ) {
      pnew->type.ty = TY_OTHERS;
      pnew->type.tyvars.var.ident = tv_ident;
      pnew->type.tyvars.var.pnext = ptvs;
      ptv = pnew;
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  return ptv;
}
static TYPE_CONS_PTR enum_tvs ( TYPE_CONS_PTR pacc, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR ptvs = NULL;
  assert( pacc );
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_LTE_VAR:
    assert( pty->attrs.lte.pln_var );
    assert( ((VAR_ATTRIB_PTR)pty->attrs.lte.pln_var)->ptype );
    ptvs = enum_tvs( pacc, ((VAR_ATTRIB_PTR)pty->attrs.lte.pln_var)->ptype, pos );
    break;
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    break;
  case TY_LIST:
    assert( pty->attrs.list.pty_elem );
    ptvs = enum_tvs( pacc, pty->attrs.list.pty_elem, pos );
    break;
  case TY_POLY:
    assert( ! pty->type.tyvars.var.pnext );
    assert( pty->type.tyvars.var.ident );
    ptvs = add_tv( pacc, pty->type.tyvars.var.ident, pos );
    break;
  case TY_GEN:
    /* fall thru. */
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ptvs;
}

static TYPE_CONS_PTR gen_tvs ( TYPE_ENV_PTR penv, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR ptvs = NULL;
  assert( penv );
  assert( pty );
  
  ptvs = enum_tvs( NULL, pty, pos );
  {
    TYPE_CONS_PTR ptv = ptvs;
    while( ptv ) {
      assert( ptv->type.ty == TY_OTHERS );
      assert( ptv->type.tyvars.var.ident );
      if( ! env_lkup( penv, ptv->type.tyvars.var.ident ) ) {
	TYPE_CONS_PTR pgv = ptv;
	assert( pgv );
	ptv = ptv->type.tyvars.var.pnext;
	pgv->type.ty = TY_GEN;
	pgv->type.tyvars.var.pnext = pty->type.tyvars.pgenvars;
	pty->type.tyvars.pgenvars = pgv;
      } else
	ptv = ptv->type.tyvars.var.pnext;
    }
  }
  return pty;
}

TYPE_CONS_PTR typecheck1 ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, STATEMENT_PTR pstmt, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_stmt = NULL;
  assert( ppsubst );
  assert( penv );
  assert( pstmt );
  
  switch( pstmt->sort ) {
  case STMT_DECL:
    assert( pstmt->u.pdecl );
    assert( (pstmt->u.pdecl)->pinit );
    ty_infer( ppsubst, penv, (pstmt->u.pdecl)->pinit, pos );
    break;
  case STMT_EXPR:
    ty_infer( ppsubst, penv, pstmt->u.pexpr, pos );
    break;
  case END_OF_STMT_SORT:
    /* fall thru. */
  default:
    assert( FALSE );    
  }
  pstmt->penv = penv;
  return pty_stmt;
}
