#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

#define TYVER_SEQDIGITS_MAXLEN 8
static struct {
  int seq;
} tyver_ctrl;
static char *fresh_tyvar ( SRC_POS_C pos ) {
  const char *prefix = "t_";
  char *ident = NULL;
  
  ident = (char *)new_memarea( strlen(prefix) + (TYVER_SEQDIGITS_MAXLEN + 1) );
  if( ident ) {
    const int n = strlen( prefix );
    snprintf( &ident[n], TYVER_SEQDIGITS_MAXLEN, "%d", tyver_ctrl.seq );
    (&ident[n])[TYVER_SEQDIGITS_MAXLEN] = 0;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return ident;
}
TYPE_CONS_PTR asgn_tyvar ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) {
  assert( pty_cons );
  switch( pty_cons->type.ty ) {
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    break;  
  case TY_LIST:
    asgn_tyvar( pty_cons->attrs.list.pty_elem, pos );
    break;
  case TY_POLY:
    assert( ! pty_cons->type.tyvars.var.pnext );
    if( ! pty_cons->type.tyvars.var.ident )
      pty_cons->type.tyvars.var.ident = fresh_tyvar( pos );
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

static struct {
  TYPE_CONS_PTR pavail;
  TYPE_CONS_PTR palive;
} type_cons_manage;
TYPE_CONS_PTR alloc_type_cons ( SRC_POS_C pos ) {
  TYPE_CONS_PTR ptycons = NULL;
  
  ptycons = (TYPE_CONS_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.pavail,
				       (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.palive,
				       sizeof(TYPE_CONS), NUM_TYCONS_PER_ALLOC, pos );
  return ptycons;
}

void free_type_cons ( TYPE_CONS_PTR ptycons ) {
  if( ptycons ) {
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_cons_manage.palive,
		(ALLOC_NODE_LINKS_PTR)ptycons );
  }
}

TYPE_CONS_PTR exam_tycon ( TYPE_CONS_PTR pty ) {
  TYPE_CONS_PTR r = NULL;
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_EXPR:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    assert( pty->attrs.expr.pexpr );
    assert( (pty->attrs.expr.pexpr)->ptype );
    r = exam_tycon( (pty->attrs.expr.pexpr)->ptype );
    break;
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    assert( ! pty->type.tyvars.pgenvars );
    r = pty;
    break;
  case TY_LIST:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    assert( pty->attrs.list.pty_elem );
    r = exam_tycon( pty->attrs.list.pty_elem );
    assert( r );
    if( pty->attrs.list.car ) {
      LIST_CELL_PTR pc = pty;
      do {
	LIST_CELL_PTR car = NULL;
	TYPE_CONS_PTR pty_c = NULL;
	assert( pc->attrs.list.car );
	car = pc->attrs.list.car;
	switch( car->type.ty ) {
	case TY_EXPR:
	  assert( car->attrs.expr.pexpr );
	  assert( (car->attrs.expr.pexpr)->ptype );
	  pty_c = exam_tycon( (car->attrs.expr.pexpr)->ptype );
	  assert( pty_c );
	  break;
	case TY_INT:
	case TY_CHAR:
	case TY_STRING:
	  pty_c = exam_tycon( car );
	  assert( pty_c );
	  break;
	case TY_LIST:
	  pty_c = exam_tycon( car );
	  assert( pty_c );
	  break;
	case TY_POLY:
	  /* fall thru. */
	case TY_GEN:
	  /* fall thru. */
	case TY_OTHERS:
	  /* fall thru. */
	case END_OF_TYPE_CODE:
	  /* fall thru. */
	default:
	  assert( FALSE );
	}
	assert( pc->attrs.list.pty_elem == pty_c );
	if( pc->attrs.list.cdr )
	  assert( pty->attrs.list.plast == pc );
	pc = pc->attrs.list.cdr;
      } while( pc );
    } else {
      assert( ! pty->attrs.list.cdr );
      assert( pty->attrs.list.plast == pty );
    }
    r = pty;
    break;
  case TY_POLY:
    assert( ! pty->type.tyvars.var.pnext );
    r = pty;
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

TYPE_CONS_PTR dup_tydesc ( TYPE_CONS_PTR ptydesc_org, SRC_POS_C pos ) {
  TYPE_CONS_PTR ptydesc = NULL;
  assert( ptydesc_org );
  
  ptydesc = alloc_type_cons( pos );
  if( ptydesc ) {
    ptydesc->pos = pos;
    ptydesc->type = ptydesc_org->type;
    ptydesc->attrs = ptydesc_org->attrs;
    if( ptydesc_org->type.pstuck )      
      ptydesc->type.pstuck = ptydesc_org->type.pstuck;
    else
      ptydesc->type.pstuck = ptydesc_org;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return ptydesc;
}

int enum_gentyvers ( TYPE_CONS_PTR *ppgenvars, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  int ngvs = 0;
  assert( ppgenvars );
  assert( pty );
  
  *ppgenvars = NULL;
  if( pty->type.tyvars.pgenvars ) {
    TYPE_CONS_PTR ptail = NULL;
    TYPE_CONS_PTR pgv = pty->type.tyvars.pgenvars;
    assert( pgv );
    do {
      TYPE_CONS_PTR pnote = NULL;
      assert( pgv->type.ty == TY_GEN );
      assert( pgv->type.tyvars.var.ident );
      pnote = alloc_type_cons( pos );
      if( pnote ) {
	bzero( pnote, sizeof(TYPE_CONS) );
	pnote->type.ty = TY_OTHERS;
	pnote->type.tyvars.var.ident = pgv->type.tyvars.var.ident;
	pnote->type.tyvars.var.pnext = NULL;
	if( ptail )
	  ptail->type.tyvars.var.pnext = pnote;	
	else
	  *ppgenvars = pnote;
	ptail = pnote;
	ngvs++;
      } else {
	*ppgenvars = NULL;
	ath_abort( pos, ABORT_MEMLACK );
      }
      pgv = pgv->type.tyvars.var.pnext;
    } while( pgv );
  }
  return ngvs;
}

TYPE_CONS_PTR gen_tyvars ( TYPE_CONS_PTR pty, TYPE_CONS_PTR pgen_tyvers, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_gen = NULL;
  assert( pty );
  assert( pgen_tyvers );
  
  pty_gen = dup_tydesc( pty, pos );
  if( pty_gen ) {
    TYPE_CONS_PTR pv = pgen_tyvers;
    assert( pv );
    do {
      BOOL found = FALSE;
      TYPE_CONS_PTR pg = pty_gen->type.tyvars.pgenvars;
      assert( pv->type.ty == TY_OTHERS );
      assert( pv->type.tyvars.var.ident );
      while( pg ) {
	assert( pg->type.tyvars.var.ident );
	if( strcmp( pg->type.tyvars.var.ident, pv->type.tyvars.var.ident ) == 0 ) {
	  found = TRUE;
	  break;
	}
	pg = pg->type.tyvars.var.pnext;
      }
      if( !found ) {
	TYPE_CONS_PTR pnew = NULL;
	assert( !pg );
	pnew = alloc_type_cons( pos );
	if( pnew ) {
	  pnew->pos = pos;
	  pnew->type.ty = TY_GEN;
	  pnew->type.tyvars.pgenvars = NULL;
	  pnew->type.pstuck = NULL;
	  pnew->type.tyvars.var.ident = pv->type.tyvars.var.ident;
	  pnew->type.tyvars.var.pnext = pty_gen->type.tyvars.pgenvars;
	  pty_gen->type.tyvars.pgenvars = pnew;
	} else {
	  pty_gen = NULL;
	  goto failed_memalloc;
	}
	assert( !pg );
      } else {
	assert( pg );
	assert( strcmp( pg->type.tyvars.var.ident, pv->type.tyvars.var.ident ) == 0 );
      }
      pv = pv->type.tyvars.var.pnext;
    } while( pv );
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pty_gen;
}

struct {
  struct {
    TYPE_MAPSTO_PTR pavail;
    TYPE_MAPSTO_PTR palive;
  } mapping;
  struct {
    TYPE_SUBST_PTR pavail;
    TYPE_SUBST_PTR palive;
  } subst;
} type_subst_manage;
TYPE_MAPSTO_PTR alloc_type_mapping ( SRC_POS_C pos ) {
  TYPE_MAPSTO_PTR ptymap = NULL;
  
  ptymap = (TYPE_MAPSTO_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.pavail,
					(ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.palive,
					sizeof(TYPE_MAPSTO), NUM_TYMAPS_PER_ALLOC, pos );
  return ptymap;
}

void free_type_mapping ( TYPE_MAPSTO_PTR ptymap ) {
  if( ptymap ) {
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.palive,
		(ALLOC_NODE_LINKS_PTR)ptymap );
  }
}

TYPE_SUBST_PTR alloc_type_subst ( SRC_POS_C pos ) {
  TYPE_SUBST_PTR ptysubst = NULL;
  
  ptysubst = (TYPE_SUBST_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.pavail,
					 (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.palive,
					 sizeof(TYPE_SUBST), NUM_TYSUBSTS_PER_ALLOC, pos );
  return ptysubst;
}

void free_type_subst ( TYPE_SUBST_PTR psubst ) {
  if( psubst ) {
    TYPE_MAPSTO_PTR ptymap = NULL;
    if( psubst->pcomposit )
      free_type_subst( psubst->pcomposit );
    ptymap = psubst->pmappings;
    while( ptymap ) {
      TYPE_MAPSTO_PTR pn = ptymap->pnext;
      free_type_mapping( ptymap );
      ptymap = pn;
    }
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.palive,
		(ALLOC_NODE_LINKS_PTR)psubst );
  }
}

TYPE_CONS_PTR subst_map ( TYPE_SUBST_PTR psubst, const char *tyvar_ident, SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  TYPE_MAPSTO_PTR ps_elem = NULL;
  assert( psubst );
  assert( tyvar_ident );
  
  ps_elem = psubst->pmappings;
  while( ps_elem ) {
    assert( ps_elem->ident );
    assert( ps_elem->ptype );
    if( strcmp( ps_elem->ident, tyvar_ident ) == 0 ) {
      r = ps_elem->ptype;
      break;
    }
    ps_elem = ps_elem->pnext;
  }
  return r;
}

TYPE_SUBST_PTR subst_add ( TYPE_SUBST_PTR psubst, const char *tyvar_ident, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_MAPSTO_PTR pe = NULL;
  assert( psubst );
  assert( tyvar_ident );
  assert( pty );
  
  pe = alloc_type_mapping( pos );
  if( pe ) {
    pe->ident = tyvar_ident;
    pe->ptype = pty;
    pe->pnext = psubst->pmappings;
    psubst->pmappings = pe;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return psubst;
}

TYPE_SUBST_PTR dup_subst ( TYPE_SUBST_PTR psub_org, SRC_POS_C pos ) {
  TYPE_SUBST_PTR ps_dup = NULL;
  assert( psub_org );

  ps_dup = alloc_type_subst( pos );
  if( ps_dup ) {
    TYPE_SUBST_PTR pc_dup = NULL;
    TYPE_MAPSTO_PTR pprev = NULL;
    TYPE_MAPSTO_PTR pmap = NULL;
    if( psub_org->pcomposit ) {
      pc_dup = dup_subst( psub_org->pcomposit, pos );
      assert( pc_dup );
    }
    pmap = psub_org->pmappings;
    while( pmap ) {
      TYPE_MAPSTO_PTR pnew = NULL;
      assert( pmap->ident );
      assert( pmap->ptype );
      pnew = alloc_type_mapping( pos );
      if( pnew ) {
	pnew->ident = pmap->ident;
	pnew->ptype = pmap->ptype;
	pnew->pnext = NULL;
	if( pprev )
	  pprev->pnext = pnew;
	else
	  ps_dup->pmappings = pnew;
	pprev = pnew;
	pmap = pmap->pnext;
      } else {
	ps_dup = NULL;
	goto failed_memalloc;
      }
      ps_dup->pcomposit = pc_dup;
    }
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return ps_dup;
}

TYPE_SUBST_PTR comp_subst ( TYPE_SUBST_PTR psub_1, TYPE_SUBST_PTR psub_2, SRC_POS_C pos ) {
  TYPE_SUBST_PTR pnew_1 = NULL;
  TYPE_SUBST_PTR pnew_2 = NULL;
  pnew_1 = dup_subst( psub_1, pos );
  if( pnew_1 ) {
    pnew_2 = dup_subst( psub_2, pos );
    if( pnew_2 ) {
      pnew_1->pcomposit = pnew_2;
    } else {
      pnew_1 = NULL;
      goto failed_memalloc;
    }
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pnew_1;
}

static TYPE_SUBST_PTR elim_subst_elems ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR tyvers_omit, SRC_POS_C pos ) {
  TYPE_SUBST_PTR ps_elim = NULL;
  assert( psubst );
  assert( tyvers_omit );
  
  if( psubst->pcomposit )
    ps_elim = elim_subst_elems( psubst->pcomposit, tyvers_omit, pos );
  assert( ps_elim );
  {
    TYPE_CONS_PTR pev = tyvers_omit;
    while( pev ) {
      TYPE_MAPSTO_PTR *ppm = NULL;
      assert( pev->type.ty == TY_OTHERS );
      ppm = &ps_elim->pmappings;
      while( *ppm ) {
	assert( pev->type.tyvars.var.ident );
	if( strcmp( (*ppm)->ident, pev->type.tyvars.var.ident ) == 0 ) {
	  *ppm = (*ppm)->pnext;
	  break;
	} else
	  ppm = &(*ppm)->pnext;
      }
      pev = pev->type.tyvars.var.pnext;
    }
  }
  return ps_elim;
}
TYPE_SUBST_PTR restr_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR tyvers_omit, SRC_POS_C pos ) {
  TYPE_SUBST_PTR pr_subst = NULL;
  assert( psubst );
  assert( tyvers_omit );
  
  {
    TYPE_SUBST_PTR pnew = NULL;
    pnew = dup_subst( psubst, pos );
    if( pnew ) {
      pr_subst = elim_subst_elems( pnew, tyvers_omit, pos );
      assert( pr_subst );
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  return pr_subst;
}

static TYPE_CONS_PTR tyvar_rewrt ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_subst = NULL;
  assert( psubst );
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_EXPR:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    assert( pty->attrs.expr.pexpr );
    assert( (pty->attrs.expr.pexpr)->ptype );
    {
      TYPE_CONS_PTR pty_s_expr = NULL;
      pty_s_expr = tyvar_rewrt( psubst, (pty->attrs.expr.pexpr)->ptype, pos );
      assert( pty_s_expr );
      if( pty_s_expr != (pty->attrs.expr.pexpr)->ptype ) {
	pty_subst = dup_tydesc( pty, pos );
	if( pty_subst ) {
	  assert( pty_subst->type.ty == TY_EXPR );
	  assert( pty_subst->attrs.expr.pexpr == pty->attrs.expr.pexpr );
	  (pty_subst->attrs.expr.pexpr)->ptype = pty_s_expr;
	} else
	  ath_abort( pos, ABORT_MEMLACK );
      } else
	pty_subst = pty;
    }
    break;
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    assert( ! pty->type.tyvars.pgenvars );
    pty_subst = pty;
    break;
  case TY_LIST:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    assert( pty->attrs.list.pty_elem );
    pty_subst = dup_tydesc( pty, pos );
    if( pty_subst ) {
      TYPE_CONS_PTR pty_s_elem = NULL;
      assert( pty_subst->type.ty == TY_LIST );
      assert( pty_subst->attrs.list.pty_elem == pty->attrs.list.pty_elem );
#ifdef RUNTIME_CONSITENCY_CHECK
      exam_tycon( pty_subst );
#endif // RUNTIME_CONSITENCY_CHECK
      pty_s_elem = tyvar_rewrt( psubst, pty_subst->attrs.list.pty_elem, pos );
      assert( pty_s_elem );
      pty_subst->attrs.list.pty_elem = pty_s_elem;
      if( pty->attrs.list.cdr ) {
	TYPE_CONS_PTR pty_s_cdr = NULL;
	assert( pty->attrs.list.car );
	assert( pty_subst->attrs.list.car == pty->attrs.list.car );
	assert( pty_subst->attrs.list.cdr == pty->attrs.list.cdr );
	pty_s_cdr = tyvar_rewrt( psubst, pty_subst->attrs.list.cdr, pos );
	assert( pty_s_cdr );
	pty_subst->attrs.list.cdr = pty_s_cdr;
      } else
	assert( ! pty_subst->attrs.list.cdr );
      pty_subst->type.pstuck = NULL;
      pty_subst->type.tyvars.pgenvars = NULL;
      if( pty_subst->attrs.list.pty_elem == pty->attrs.list.pty_elem ) {
	BOOL dirty = FALSE;
	if( pty_subst->attrs.list.car ) {
	  assert( pty->attrs.list.car );
	  if( pty_subst->attrs.list.cdr ) {
	    assert( pty->attrs.list.cdr );
	    dirty = (pty_subst->attrs.list.cdr != pty->attrs.list.cdr);
	  } else
	    assert( ! pty->attrs.list.cdr );
	} else
	  assert( ! pty_subst->attrs.list.cdr );
	if( !dirty )
	  pty_subst = pty;
      }
    } else
      ath_abort( pos, ABORT_MEMLACK );
    break;
  case TY_POLY:      
    assert( pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    {
      TYPE_CONS_PTR pty_m = NULL;
      pty_m = subst_map( psubst, pty->type.tyvars.var.ident, pos );
      if( pty_m ) {
	pty_subst = dup_tydesc( pty_m, pos );
	if( !pty_subst )
	  ath_abort( pos, ABORT_MEMLACK );
      } else
	pty_subst = pty;
    }
    assert( pty_subst );
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
  return pty_subst;
}
TYPE_CONS_PTR ty_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_s = NULL;
  TYPE_CONS_PTR pty_c = NULL;
  assert( psubst );
  assert( pty );
  
  pty_c = pty;
  if( psubst->pcomposit )
    pty_c = ty_subst( psubst->pcomposit, pty, pos );
  assert( pty_c );
  pty_s = tyvar_rewrt( psubst, pty_c, pos );
  assert( pty_s );
  return pty_s;
}

static struct {
  struct {
    TYENV_ELEM_PTR pavail;
    TYENV_ELEM_PTR palive;
  } mapping;
  struct {
    TYPE_ENV_PTR pavail;
    TYPE_ENV_PTR palive;
  } env;
} type_env_manage;
TYENV_ELEM_PTR alloc_tyenv_elem ( SRC_POS_C pos ) {
  TYENV_ELEM_PTR penv = NULL;
  
  penv = (TYENV_ELEM_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.pavail,
				     (ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.palive,
				     sizeof(TYENV_ELEM), NUM_TYELEMS_PER_ALLOC, pos );
  return penv;
}

void free_tyenv_elems ( TYENV_ELEM_PTR pelem ) {
  if( pelem ) {    
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.palive,
		(ALLOC_NODE_LINKS_PTR)pelem );
  }
}

TYPE_ENV_PTR alloc_type_env ( SRC_POS_C pos ) {
  TYPE_ENV_PTR penv = NULL;
  
  penv = (TYPE_ENV_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.pavail,
				   (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.palive,
				   sizeof(TYPE_ENV), NUM_TYENVS_PER_ALLOC, pos );
  return penv;
}

void free_type_env ( TYPE_ENV_PTR penv ) {
  if( penv ) {
    TYENV_ELEM_PTR pelem = penv->pmappings;
    while( pelem ) {
      TYENV_ELEM_PTR pnext = pelem->pnext;
      free_tyenv_elems( pelem );
      pelem = pnext;
    }    
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.palive,
		(ALLOC_NODE_LINKS_PTR)penv );    
  }
}

TYPE_ENV_PTR env_rid ( TYPE_ENV_PTR penv, const char *var_ident ) {
  BOOL found = FALSE;
  TYENV_ELEM_PTR *ppe = NULL;
  assert( penv );
  assert( var_ident );
  
  ppe = &penv->pmappings;
  while( *ppe ) {
    assert( (*ppe)->var.ident );
    assert( (*ppe)->var.ptype );
    if( strcmp( (*ppe)->var.ident, var_ident ) == 0 ) {
      found = TRUE;
      *ppe = (*ppe)->pnext;
      break;
    }
    ppe = &(*ppe)->pnext;
  }
  assert( penv );
  if( !found ) {
    assert( ! *ppe );
    if( penv->uplink )
      penv = env_rid( penv->uplink, var_ident );
    else
      penv = NULL;
  }
  return penv;
}

TYPE_ENV_PTR env_add ( TYPE_ENV_PTR penv, const char *var_ident, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( var_ident );
  assert( pty );
  
  pe = alloc_tyenv_elem( pos );
  if( pe ) {
    pe->var.ident = var_ident;
    pe->var.ptype = pty;
    pe->pnext = penv->pmappings;
    penv->pmappings = pe;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return penv;
}

TYENV_ELEM_PTR env_lkup ( TYPE_ENV_PTR penv, const char *var_ident ) {
  BOOL found = FALSE;
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( var_ident );
  
  pe = penv->pmappings;
  while( pe ) {
    assert( pe->var.ident );
    assert( pe->var.ptype );
    if( strcmp( pe->var.ident, var_ident ) == 0 ) {
      found = TRUE;
      break;
    }
    pe = pe->pnext;
  }
  if( !found ) {
    assert( !pe );
    assert( penv );
    if( penv->uplink )
      pe = env_lkup( penv->uplink, var_ident );
  } else
    assert( pe );
  return pe;
}

TYPE_ENV_PTR dup_env ( TYPE_ENV_PTR penv_org, SRC_POS_C pos ) {
  TYPE_ENV_PTR penv = NULL;
  assert( penv_org );
  
  penv = alloc_type_env( pos );
  if( penv ) {
    TYPE_ENV_PTR pupps = NULL;
    TYENV_ELEM_PTR pprev = NULL;
    TYENV_ELEM_PTR pmap = NULL;
    if( penv_org->uplink ) {
      pupps = dup_env( penv_org->uplink, pos );
      assert( pupps );
    }
    penv->pmappings = NULL;    
    pmap = penv_org->pmappings;
    while( pmap ) {
      TYENV_ELEM_PTR pnew = NULL;
      pnew = alloc_tyenv_elem( pos );
      if( pnew ) {
	assert( pmap->var.ident );
	assert( pmap->var.ptype );
	pnew->var = pmap->var;
	pnew->pnext = NULL;
      } else {
	penv = NULL;
	goto failed_memalloc;
      }
      if( pprev )
	pprev->pnext = pnew;
      else
	penv->pmappings = pnew;
      pprev = pnew;
      pmap = pmap->pnext;
    }
    penv->uplink = pupps;
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return penv;
}

TYPE_ENV_PTR env_subst ( TYPE_ENV_PTR penv, TYPE_SUBST_PTR psubst, SRC_POS_C pos ) {
  TYPE_ENV_PTR penv_s = NULL;
  assert( penv );
  assert( psubst );
  
  penv_s = dup_env( penv, pos );
  if( penv_s ) {
    TYPE_ENV_PTR penv_u = NULL;
    TYENV_ELEM_PTR pe = NULL;
    if( penv->uplink ) {
      penv_u = env_subst( penv->uplink, psubst, pos );
      assert( penv_u );
    }
    pe = penv_s->pmappings;
    while( pe ) {
      TYPE_CONS_PTR pty_s = NULL;
      assert( pe->var.ident );
      assert( pe->var.ptype );
      pty_s = ty_subst( psubst, pe->var.ptype, pos );
      assert( pty_s );
      pe->var.ptype = pty_s;
      pe = pe->pnext;
    }
    penv_s->uplink = penv_u;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return penv_s; 
}

char *print_var_type ( char *sbuf, TYPE_CONS_PTR_C pty_desc ) {
  SRC_POS pos;
  char *ps = NULL;  
  assert( sbuf );
  assert( pty_desc );
  
  ps = sbuf;
  pos = pty_desc->pos;
  switch( pty_desc->type.ty ) {
  case TY_INT:
    strcpy( ps, "int" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_CHAR:
    strcpy( ps, "char" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_STRING:
    strcpy( ps, "string" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_LIST:
    strcpy( ps, "[" );
    ps++;
    if( pty_desc->attrs.list.pty_elem ) {
      ps = print_var_type( ps, pty_desc->attrs.list.pty_elem );
      assert( *ps == 0 );
    } else {
      strcpy( ps, "UNKNOWN_TYPE" );
      ps += strlen( ps );
      assert( *ps == 0 );
    }    
    strcpy( ps, "]" );
    ps++;
    assert( *ps == 0 );
    break;
  case TY_POLY:
    strcpy( ps, "poly" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ps;
}
