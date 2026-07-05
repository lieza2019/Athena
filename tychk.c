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

static TYPE_CONS_PTR add_tyv ( TYPE_CONS_PTR *pptvs, const char *tv_ident, SRC_POS_C pos ) { // CHECKED.
  BOOL found = FALSE;
  assert( pptvs );
  assert( tv_ident );
  
  {
    TYPE_CONS_PTR ptv = *pptvs;
    while( ptv ) {
      assert( ptv->type.ty == TY_OTHERS );
      assert( ptv->type.tyvars.var.ident );
      if( strcmp( ptv->type.tyvars.var.ident, tv_ident ) == 0 ) {
	found = TRUE;
	break;
      }
      ptv = ptv->type.tyvars.var.pnext;
    }
  }
  if( !found ) {
    TYPE_CONS_PTR pnew = NULL;
    pnew = alloc_type_cons( pos );
    if( pnew ) {
      pnew->type.ty = TY_OTHERS;
      pnew->type.tyvars.var.ident = tv_ident;
      pnew->type.tyvars.var.pnext = *pptvs;
      *pptvs = pnew;
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  return *pptvs;
}

static TYPE_CONS_PTR enum_tvs ( TYPE_CONS_PTR *ppacc, TYPE_CONS_PTR pty, SRC_POS_C pos ) { // REVISED.
  assert( ppacc );
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    break;
  case TY_LIST:
    assert( pty->attrs.list.pty_elem );
    enum_tvs( ppacc, pty->attrs.list.pty_elem, pos );
    break;
  case TY_POLY:
    if( pty->type.tyvars.var.ident )
      add_tyv( ppacc, pty->type.tyvars.var.ident, pos );
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
  return *ppacc;
}
TYPE_CONS_PTR gen_tvs ( TYPE_ENV_PTR penv, TYPE_CONS_PTR pty, SRC_POS_C pos ) { // REVISED.
  TYPE_CONS_PTR ptvs = NULL;
  assert( penv );
  assert( pty );
  
  enum_tvs( &ptvs, pty, pos );
  {
    TYPE_CONS_PTR ptv = ptvs;
    while( ptv ) {
      TYPE_CONS_PTR pgv = ptv;
      assert( ptv->type.ty == TY_OTHERS );
      assert( ptv->type.tyvars.var.ident );
      ptv = ptv->type.tyvars.var.pnext;
      if( ! env_lkup( penv, pgv->type.tyvars.var.ident ) ) {
	pgv->type.ty = TY_GEN;
	pgv->type.tyvars.var.pnext = pty->type.tyvars.pgenvars;
	pty->type.tyvars.pgenvars = pgv;
      }
    }
  }
  return pty;
}

TYPE_CONS_PTR inst_gtvs ( TYPE_CONS_PTR pty, SRC_POS_C pos ) { // REVISED.
  TYPE_CONS_PTR pty_inst = NULL;
  TYPE_SUBST_PTR ps_inst = NULL;
  assert( pty );
  
  ps_inst = alloc_type_subst( pos );
  if( ps_inst ) {
    TYPE_CONS_PTR pgv = pty->type.tyvars.pgenvars;
    while( pgv ) {
      TYPE_CONS_PTR ptyv_fresh = NULL;
      assert( pgv->type.tyvars.var.ident );
      ptyv_fresh = alloc_type_cons( pos );
      if( ptyv_fresh ) {
	ptyv_fresh->type.ty = TY_POLY;
	ptyv_fresh->type.tyvars.var.ident = fresh_tyvar( pos );
	assert( ptyv_fresh->type.tyvars.var.ident );
	ptyv_fresh->type.tyvars.var.pnext = NULL;
	ptyv_fresh->tycons_directiv = SUBST_TYVAR_ONLYRENAMING;
	subst_add( ps_inst, pgv->type.tyvars.var.ident, ptyv_fresh, pos );
      } else
	goto failed_memalloc;
      pgv = pgv->type.tyvars.var.pnext;
    }
    pty_inst = ty_subst( ps_inst, pty, pos );
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pty_inst;
}

static BOOL chk_tyvar_occur ( const char *tyv_ident, TYPE_CONS_PTR pty ) { // REVISED.
  BOOL r = FALSE;
  assert( tyv_ident );
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    break;
  case TY_LIST:
    assert( pty->attrs.list.pty_elem );
    r = chk_tyvar_occur( tyv_ident, pty->attrs.list.pty_elem );
    break;
  case TY_POLY:
    assert( pty->type.tyvars.var.ident );
    r = (strcmp( pty->type.tyvars.var.ident, tyv_ident ) == 0);
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
static BOOL unif_mkequ ( TYPE_SUBST_PTR ps_unif, TYPE_CONS_PTR pvar, TYPE_CONS_PTR pty, SRC_POS_C pos ) { // CHECKED.
  BOOL r = FALSE;
  assert( ps_unif );
  assert( pvar );
  assert( pty );
  
  assert( pvar->type.ty == TY_POLY );
  assert( pvar->type.tyvars.var.ident );
  if( ! chk_tyvar_occur( pvar->type.tyvars.var.ident, pty ) ) {
    subst_add( ps_unif, pvar->type.tyvars.var.ident, pty, pos );
    r = TRUE;
  }
  return r;
}

BOOL ty_unify ( TYPE_SUBST_PTR *pps_unif, TYPE_CONS_PTR pty_1, TYPE_CONS_PTR pty_2, SRC_POS_C pos ) { // REVISED.
  BOOL r = FALSE;
  assert( pps_unif );
  assert( pty_1 );
  assert( pty_2 );
  
  *pps_unif = NULL;
  switch( pty_1->type.ty ) {
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
    if( pty_2->type.ty == TY_POLY ) {
      assert( pty_2->type.tyvars.var.ident );
      r = unif_mkequ( *pps_unif, pty_2, pty_1, pos );
    } else
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

static EXPR_CONS_PTR ty_infer ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR *ppenv, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  EXPR_CONS_PTR pexp_inf = NULL;
  assert( ppsubst );
  assert( ppenv );
  assert( pexpr );
  
  switch( pexpr->mnemonic ) {
  case MNC_CALL:
    break;
  case MNC_ASGN:
    assert( EXAM_ASGN_EXPR( pexpr ) );
    {
      TYPE_SUBST_PTR psubst_l = NULL;
      EXPR_CONS_PTR pe_infl = NULL;
      pe_infl = ty_infer( &psubst_l, ppenv, pexpr->kids.pleft, pos );
      if( pe_infl ) {
	TYPE_SUBST_PTR psubst_r = NULL;
	EXPR_CONS_PTR pe_infr = NULL;
	assert( pe_infl->ptype );
	assert( psubst_l );
	pe_infr = ty_infer( &psubst_r, ppenv, pexpr->kids.pright, pos );
	if( pe_infr ) {
	  TYPE_SUBST_PTR psubst_u = NULL;
	  TYPE_CONS_PTR pty_infl_sr = NULL;
	  assert( pe_infr->ptype );
	  assert( psubst_r );
	  pty_infl_sr = ty_subst( psubst_r, pe_infl->ptype, pos );
	  assert( pty_infl_sr );
	  if( ty_unify( &psubst_u, pty_infl_sr, pe_infr->ptype, pos ) ) {
	    EXPR_CONS_PTR pe_infl_su = NULL;
	    assert( psubst_u );
	    pe_infl_su = alloc_expr_cons( pos );
	    if( pe_infl_su ) {
	      pe_infl_su->pos = pos;
	      pe_infl_su->mnemonic = MNC_ASGN;
	      pe_infl_su->kids.pleft = pe_infl;
	      pe_infl_su->kids.pright = pe_infr;
	      pe_infl_su->ptype = ty_subst( psubst_u, pty_infl_sr, pos );
	      assert( pe_infl_su->ptype );
	      {
		TYPE_ENV_PTR penv_inf = NULL;
		penv_inf = dup_env( *ppenv, pos );
		assert( penv_inf );
		*ppenv = env_subst( penv_inf, psubst_u, pos );
		*ppsubst = comp_subst( psubst_u, comp_subst( psubst_r, psubst_l, pos ), pos );
		pexp_inf = pe_infl_su;
	      }
	    } else
	      ath_abort( pos, ABORT_MEMLACK );
	  }
	}
      }
    }
    break;
  case MNC_ARITH:
    break;
  case MNC_LVALUE:
    assert( EXAM_LVALUE_EXPR( pexpr ) );
    assert( pexpr->kids.body.refaddr.var.ident );
    pexp_inf = alloc_expr_cons( pos );
    if( pexp_inf ) {
      pexp_inf->pos = pos;
      pexp_inf->mnemonic = MNC_LVALUE;
      pexp_inf->kids = pexpr->kids;
      assert( pexp_inf->kids.body.refaddr.var.ptype );
      inst_gtvs( pexp_inf->kids.body.refaddr.var.ptype, pos );
      pexp_inf->ptype = pexp_inf->kids.body.refaddr.var.ptype;
      *ppenv = env_add( *ppenv, pexpr->kids.body.refaddr.var.ident, pexp_inf->ptype, pos );
    } else
      ath_abort( pos, ABORT_MEMLACK );
    break;
  case MNC_RVALUE:
    break;
  case MNC_LIST:
    break;
  case MNC_CONST:
#if 0
    assert( EXAM_CONST_EXPR( pexpr ) );
    pexp_inf = alloc_expr_cons( pos );
    if( pexp_inf ) {
#if 0 // !!!!!
      EXPR_CONS_PTR pe_inf = NULL;
#endif
      TYPE_CONS_PTR pty_cnst = pexpr->kids.pdaugh;
      assert( pty_cnst );
      pexp_inf->pos = pos;
      pexp_inf->mnemonic = MNC_CONST;
      switch( pty_cnst->type.ty ) {
#if 0 // !!!!!
      case TY_EXPR:
	assert( pty_cnst->attrs.expr.pexpr );
	pe_inf = ty_infer( ppsubst, penv, pty_cnst->attrs.expr.pexpr, pos );
	assert( pe_inf );
	assert( pe_inf->ptype );
	if( pe_inf != pty_cnst->attrs.expr.pexpr ) {
	  TYPE_CONS_PTR pty_cnst_inf = NULL;
	  pty_cnst_inf = alloc_type_cons( pos );
	  if( pty_cnst_inf ) {
	    pty_cnst_inf->pos = pos;
	    pty_cnst_inf->type = pty_cnst->type;
	    pty_cnst_inf->attrs = pty_cnst->attrs;
	    pty_cnst_inf->attrs.expr.pexpr = pe_inf;
	    pexp_inf->kids.pdaugh = pty_cnst_inf;
	    //pexp_inf->ptype = pe_inf->ptyep;
	  } else
	    ath_abort( pos, ABORT_MEMLACK );
	} else
	  pexp_inf = pexpr;
#endif
	break;
      case TY_INT:
      case TY_CHAR:
      case TY_STRING:
	pexp_inf = pexpr;
	break;
      case TY_LIST:
	assert( pty_cnst->attrs.list.pty_elem );
	
	if( pty_cnst->attrs.list.car ) {
	  TYPE_CONS_PTR pcell = pty_cnst->attrs.list.car;
	  do {
	    ;
	  } while( pcell );
	} else
	  ;
	break;
      case TY_POLY:
      case TY_GEN:
	/* fall thru. */
      case TY_OTHERS:
	/* fall thru. */
      case END_OF_TYPE_CODE:
	/* fall thru. */
      default:
	assert( FALSE );
      }
    } else
      ath_abort( pos, ABORT_MEMLACK );
#endif
    break;
  case END_OF_MNEMONIC_CODE:
    /* fall thru. */
  default:
    break;
  }
  return pexp_inf;
}

static const char *asgn_fresh_tyvar ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) { // REVISED.
  assert( pty_cons );
  
  if( ! pty_cons->type.tyvars.var.ident ) {
    pty_cons->type.tyvars.var.ident = fresh_tyvar( pos );
    assert( pty_cons->type.tyvars.var.ident );
  }
  return pty_cons->type.tyvars.var.ident;
}
static TYPE_CONS_PTR travers_asgn_tyv ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) { // REVISED.
  assert( pty_cons );
  
  switch( pty_cons->type.ty ) {
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    break;
  case TY_LIST:
    assert( pty_cons->attrs.list.pty_elem );
    travers_asgn_tyv( pty_cons->attrs.list.pty_elem, pos );
    break;
  case TY_POLY:
    asgn_fresh_tyvar( pty_cons, pos );
    assert( pty_cons->type.tyvars.var.ident );
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
  return pty_cons;
}

static TYPE_CONS_PTR tc_decl_var ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR *ppenv, VAR_ATTRIB_PTR pvar_attr, SRC_POS_C pos ) { // REVISED.
  TYPE_CONS_PTR r = NULL;
  assert( ppsubst );
  assert( ppenv );
  assert( pvar_attr );
  
  if( pvar_attr->pinit ) {
    EXPR_CONS_PTR pe_asgn = NULL;
    pe_asgn = alloc_expr_cons( pos );
    if( pe_asgn ) {
      EXPR_CONS_PTR pe_lval = NULL;
      pe_asgn->pos = pos;
      pe_asgn->mnemonic = MNC_ASGN;
      pe_lval = alloc_expr_cons( pos );
      if( pe_lval ) {
	EXPR_CONS_PTR pvardecl_inf = NULL;
	pe_lval->pos = pos;
	pe_lval->mnemonic = MNC_LVALUE;
	pe_lval->kids.body.refaddr.var = *pvar_attr;
	pe_lval->kids.pleft = NULL;
	pe_lval->kids.pright = NULL;
	pe_asgn->kids.pleft = pe_lval;
	pe_asgn->kids.pright = pvar_attr->pinit;
	pvardecl_inf = ty_infer( ppsubst, ppenv, pe_asgn, pos );
	if( pvardecl_inf ) {
	  assert( pvardecl_inf->ptype );
	  pvar_attr->pinit = pvardecl_inf;
	  pvar_attr->ptype = (pvar_attr->pinit)->ptype;
	  r = pvar_attr->ptype;
	}
      } else
	goto failed_memalloc;
    } else
    failed_memalloc:
      ath_abort( pos, ABORT_MEMLACK );
  } else
    r = pvar_attr->ptype;
  return r;
}

TYPE_CONS_PTR typecheck2 ( STATEMENT_PTR pstmt, SRC_POS_C pos ) { // REVISED.
  TYPE_CONS_PTR r = NULL;
  TYPE_SUBST_PTR psubst = NULL;
  assert( pstmt );
  assert( pstmt->penv );
  
  switch( pstmt->sort ) {
  case STMT_DECL:
    assert( pstmt->u.pdecl );
    switch( (pstmt->u.pdecl)->kind ) {
    case DECL_FUN:
      break;
    case DECL_VAR:
      assert( (pstmt->u.pdecl)->u.variable.pvar );
      r = tc_decl_var( &psubst, &pstmt->penv, (pstmt->u.pdecl)->u.variable.pvar, pos );
      break;
    case END_OF_DECL_KIND:
      /* fall thru. */
    default:
      assert( FALSE );
    }
    break;
  case STMT_EXPR:
    ty_infer( &psubst, &pstmt->penv, pstmt->u.pexpr, pos );
    break;
  case END_OF_STMT_SORT:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return r;
}
