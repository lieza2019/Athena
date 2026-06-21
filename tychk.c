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

static TYPE_CONS_PTR add_tyv ( TYPE_CONS_PTR *pptvs, const char *tv_ident, SRC_POS_C pos ) {
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

static TYPE_CONS_PTR enum_tvs ( TYPE_CONS_PTR *ppacc, TYPE_CONS_PTR pty, SRC_POS_C pos );
static TYPE_CONS_PTR enum_expr_tvs ( TYPE_CONS_PTR *ppacc, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {  
  assert( ppacc );
  assert( pexpr );
  
  switch( pexpr->mnemonic ) {
  case MNC_CALL:
    break;
  case MNC_ASGN:
    assert( EXAM_ASGN_EXPR( pexpr ) );
    enum_expr_tvs( ppacc, pexpr->kids.pleft, pos );
    enum_expr_tvs( ppacc, pexpr->kids.pright, pos );
    break;
  case MNC_ARITH:
    break;
  case MNC_CONST:
    assert( EXAM_CONST_EXPR( pexpr ) );
    enum_tvs( ppacc, (TYPE_CONS_PTR)(pexpr->kids.pdaugh), pos );
    break;
  case MNC_LVALUE:
    assert( EXAM_LVALUE_EXPR( pexpr ) );
    enum_tvs( ppacc, ((VAR_ATTRIB_PTR)(pexpr->kids.pdaugh))->ptype, pos );
    break;
  case MNC_RVALUE:
    break;
  case END_OF_MNEMONIC_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  assert( pexpr->ptype );
  enum_tvs( ppacc, pexpr->ptype, pos );
  return *ppacc;
}
static TYPE_CONS_PTR enum_tvs ( TYPE_CONS_PTR *ppacc, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  assert( ppacc );
  assert( pty );
  
  switch( pty->type.ty ) {
#if 0 // !!!!!
  case TY_EXPR:
    assert( pty->attrs.expr.pexpr );
    enum_expr_tvs( ppacc, pty->attrs.expr.pexpr, pos );
    break;
#endif
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    break;
  case TY_LIST:
    assert( pty->attrs.list.pty_elem );
    if( pty->attrs.list.car ) {
      TYPE_CONS_PTR pe = pty;
      do {
	enum_tvs( ppacc, pe->attrs.list.pty_elem, pos );
	pe = pe->attrs.list.cdr;
      } while( pe );
    } else
      enum_tvs( ppacc, pty->attrs.list.pty_elem, pos );
    break;
  case TY_POLY:
    assert( ! pty->type.tyvars.var.pnext );
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

TYPE_CONS_PTR gen_tvs ( TYPE_ENV_PTR penv, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR ptvs = NULL;
  assert( penv );
  assert( pty );
  
  enum_tvs( &ptvs, pty, pos );
  {
    TYPE_CONS_PTR ptv = ptvs;
    while( ptv ) {
      assert( ptv->type.ty == TY_OTHERS );
      assert( ptv->type.tyvars.var.ident );
      if( ! env_lkup( penv, ptv->type.tyvars.var.ident ) ) {
	TYPE_CONS_PTR pgv = ptv;
	assert( pgv );
	pgv->type.ty = TY_GEN;
	pgv->type.tyvars.var.pnext = pty->type.tyvars.pgenvars;
	pty->type.tyvars.pgenvars = pgv;
      }
      ptv = ptv->type.tyvars.var.pnext;
    }
  }
  return pty;
}

TYPE_CONS_PTR inst_gtvs ( TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_inst = NULL;
  TYPE_SUBST_PTR ps_inst = NULL;
  assert( pty );
  
  ps_inst = alloc_type_subst( pos );
  if( ps_inst ) {
    TYPE_CONS_PTR pgv = NULL;
    pgv = pty->type.tyvars.pgenvars;
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
    pty_inst = ty_subst( ps_inst, pty,pos );
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pty_inst;
}

static BOOL chk_tyvar_occur ( const char *tyvar_ident, TYPE_CONS_PTR pty ) {
  BOOL r = FALSE;
  assert( tyvar_ident );
  assert( pty );
  
  switch( pty->type.ty ) {
#if 0 // !!!!!!
  case TY_LTE_VAR:
    assert( pty->attrs.lte.pln_var );
    assert( ((VAR_ATTRIB_PTR)pty->attrs.lte.pln_var)->ptype );
    r = chk_tyvar_occur( tyvar_ident, ((VAR_ATTRIB_PTR)pty->attrs.lte.pln_var)->ptype );
    break;
#endif
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
#if 0 // !!!!!
  case TY_LTE_VAR:
    assert( pty_1->attrs.lte.pln_var );
    assert( ((VAR_ATTRIB_PTR)pty_1->attrs.lte.pln_var)->ptype );
    ty_unify( pps_unif, ((VAR_ATTRIB_PTR)pty_1->attrs.lte.pln_var)->ptype, pty_2, pos );
    break;
#endif
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

#if 0
static TYPE_CONS_PTR ty_infer ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_expr = NULL;
  assert( ppsubst );
  assert( penv );
  assert( pexpr );
  
  switch( pexpr->mnemonic ) {
  case MNC_CALL:
    break;
  case MNC_ASGN:
    assert( pexpr->kids.pleft );
    assert( pexpr->kids.pright );
    {
      TYPE_SUBST_PTR pS_l = NULL;
      TYPE_CONS_PTR pty_l = NULL;
      TYPE_SUBST_PTR pS_r = NULL;
      TYPE_CONS_PTR pty_r = NULL;
      pty_l = ty_infer( &pS_l, penv, pexpr->kids.pleft, pos );
      if( pty_l ) {
	TYPE_ENV_PTR penv_r = NULL;
	assert( pty_l == (pexpr->kids.pleft)->ptype );
	penv_r = dup_env( penv, pos );
	if( penv_r ) {
	  pty_r = ty_infer( &pS_r, penv_r, pexpr->kids.pright, pos );
	} else
	  goto failed_memalloc;
	if( pty_r ) {
	  assert( pty_r == (pexpr->kids.pright)->ptype );
	  if( ty_isequ( pty_l, pty_r ) )
	    pty_expr = pty_l;
	}
      }
    failed_memalloc:
      ath_abort( pos, ABORT_MEMLACK );
    }
    break;
  case MNC_ARITH:
    break;
  case MNC_CONST:
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
#else
static EXPR_CONS_PTR ty_infer ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  EXPR_CONS_PTR pexp_inf = NULL;
  assert( ppsubst );
  assert( penv );
  assert( pexpr );
  
  switch( pexpr->mnemonic ) {
  case MNC_CALL:
    break;
  case MNC_ASGN:
    assert( EXAM_ASGN_EXPR( pexpr ) );
    assert( EXAM_LVALUE_EXPR( pexpr->kids.pleft ) );
    {
      EXPR_CONS_PTR pe_inf_l = NULL;
      TYPE_SUBST_PTR psubst_l = NULL;
      EXPR_CONS_PTR pe_inf_r = NULL;
      TYPE_SUBST_PTR psubst_r = NULL;
      TYPE_SUBST_PTR psubst_u = NULL;
      pe_inf_l = ty_infer( &psubst_l, penv, pexpr->kids.pleft, pos );
      if( pe_inf_l ) {
	assert( pe_inf_l->ptype );
	assert( EXAM_LVALUE_EXPR( pe_inf_l ) );
	pe_inf_r = ty_infer( &psubst_r, penv, pexpr->kids.pright, pos );
	if( pe_inf_r ) {
	  assert( pe_inf_r->ptype );
	  if( ty_unify( &psubst_u, pe_inf_l->ptype, pe_inf_r->ptype, pos ) ) {
	    EXPR_CONS_PTR pe_asgn = NULL;
	    pe_inf_l->ptype = ty_subst( psubst_u, pe_inf_l->ptype, pos );
	    assert( pe_inf_l->ptype );
	    pe_inf_r->ptype = ty_subst( psubst_u, pe_inf_r->ptype, pos );
	    assert( pe_inf_r->ptype );
	    pe_asgn = alloc_expr_cons( pos );
	    if( pe_asgn ) {
	      pe_asgn->pos = pos;
	      pe_asgn->mnemonic = MNC_ASGN;
	      pe_asgn->kids.pdaugh = NULL;
	      pe_asgn->kids.pleft = pe_inf_l;
	      pe_asgn->kids.pright = pe_inf_r;
	      pe_asgn->ptype = (pe_asgn->kids.pleft)->ptype;
	      pexp_inf = pe_asgn;
	    } else
	      goto failed_memalloc;
	  }
	}
      }
    failed_memalloc:
      ath_abort( pos, ABORT_MEMLACK );
    }
    break;
  case MNC_ARITH:
    break;
  case MNC_CONST:
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
	break;
#endif
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
    break;
  case MNC_LVALUE:
    assert( EXAM_LVALUE_EXPR( pexpr ) );
    pexp_inf = alloc_expr_cons( pos );
    if( pexp_inf ) {
      VAR_ATTRIB_PTR pvar_attr = pexpr->kids.pdaugh;
      assert( pvar_attr->ptype );
      pvar_attr->ptype = inst_gtvs( pvar_attr->ptype, pos );
      assert( pvar_attr->ptype );
      ;
    } else
      ath_abort( pos, ABORT_MEMLACK );
    break;
  case MNC_RVALUE:
    break;
  case END_OF_MNEMONIC_CODE:
    /* fall thru. */
  default:
    break;
  }
  return pexp_inf;
}
#endif

static char *asgn_fresh_tyvar ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) {
  char *tv_ident = NULL;
  assert( pty_cons );
  
  if( ! pty_cons->type.tyvars.var.ident ) {
    assert( ! pty_cons->type.tyvars.var.pnext );
    tv_ident = fresh_tyvar( pos );
    assert( tv_ident );
    pty_cons->type.tyvars.var.ident = tv_ident;
  }
  return tv_ident;
}
static TYPE_CONS_PTR travers_asgn_tyv ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) {
  assert( pty_cons );
  switch( pty_cons->type.ty ) {
#if 0 // !!!!!
  case TY_EXPR:
    assert( pty_cons->attrs.expr.pexpr );
    if( (pty_cons->attrs.expr.pexpr)->ptype )
      travers_asgn_tyv( (pty_cons->attrs.expr.pexpr)->ptype, pos );
    break;
#endif
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    break;  
  case TY_LIST:
    assert( pty_cons->attrs.list.pty_elem );
    if( pty_cons->attrs.list.car ) {
      TYPE_CONS_PTR pcell = pty_cons;
      do {
	assert( pcell->attrs.list.car );
	assert( pcell->attrs.list.pty_elem );
	travers_asgn_tyv( pcell->attrs.list.pty_elem, pos );
	pcell = pcell->attrs.list.cdr;
      } while( pcell );
    } else {
      assert( pty_cons->attrs.list.cdr );
      travers_asgn_tyv( pty_cons->attrs.list.pty_elem, pos );
    }
    break;
  case TY_POLY:
    assert( ! pty_cons->type.tyvars.var.pnext );
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

static TYPE_CONS_PTR tc_decl_var ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, VAR_ATTRIB_PTR pvar_attr, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_declvar = NULL;
  EXPR_CONS_PTR pe_lval = NULL;
  EXPR_CONS_PTR pe_cnst = NULL;
  EXPR_CONS_PTR pe_asgn = NULL;
  assert( ppsubst );
  assert( penv );
  assert( pvar_attr );
  assert( pvar_attr->ptype );
  
  pe_lval = alloc_expr_cons( pos );
  if( pe_lval ) {
    EXPR_CONS_PTR pvardecl_exp_inf = NULL;
    EXPR_CONS_PTR pe_tychk = NULL;
    pe_lval->pos = pos;
    pe_lval->mnemonic = MNC_LVALUE;
    pe_lval->kids.pdaugh = pvar_attr;
    pe_tychk = pe_lval;
    if( pvar_attr->pinit ) {
      pe_cnst = alloc_expr_cons( pos );
      if( pe_cnst ) {
	pe_cnst->pos = pos;
	pe_cnst->mnemonic = MNC_CONST;
	pe_cnst->kids.pdaugh = pvar_attr->pinit;
	pe_asgn = alloc_expr_cons( pos );
	if( pe_asgn ) {
	  pe_asgn->pos = pos;
	  pe_asgn->mnemonic = MNC_ASGN;
	  pe_asgn->kids.pleft = pe_lval;
	  pe_asgn->kids.pright = pe_cnst;
	  pe_tychk = pe_asgn;
	} else
	  goto failed_memalloc;
      } else
	goto failed_memalloc;
    }
    assert( pe_tychk );
    pvardecl_exp_inf = ty_infer( ppsubst, penv, pe_tychk, pos );
    if( pvardecl_exp_inf ) {
      EXPR_CONS_PTR pe_declvar = NULL;
      assert( pe_lval );
      if( pe_cnst ) {
	assert( pvar_attr->pinit );
	assert( pe_asgn );
	assert( EXAM_ASGN_EXPR( pvardecl_exp_inf ) );
	assert( EXAM_CONST_EXPR( pvardecl_exp_inf->kids.pright ) );
	pvar_attr->pinit = (pvardecl_exp_inf->kids.pright)->kids.pdaugh;
	pe_declvar = pvardecl_exp_inf->kids.pleft;
	goto var_type;
      } else {
	assert( ! pvar_attr->pinit );
	assert( ! pe_asgn );
	pe_declvar = pvardecl_exp_inf;
      var_type:
	assert( pe_declvar );
	assert( EXAM_LVALUE_EXPR( pe_declvar ) );
	pvar_attr->ptype = pe_declvar->ptype;
	pty_declvar = pvar_attr->ptype;
      }
    }
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pty_declvar;
}

TYPE_CONS_PTR typecheck2 ( STATEMENT_PTR pstmt, SRC_POS_C pos ) {
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
      r = tc_decl_var( &psubst, pstmt->penv, (pstmt->u.pdecl)->u.variable.pvar, pos );
      break;
    case END_OF_DECL_KIND:
      /* fall thru. */
    default:
      assert( FALSE );
    }
    break;
  case STMT_EXPR:
    ty_infer( &psubst, pstmt->penv, pstmt->u.pexpr, pos );
    break;
  case END_OF_STMT_SORT:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return r;
}
