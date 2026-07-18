#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

#define TYVER_SEQDIGITS_MAXLEN 8
const char tyvar_prefix[] = "t_";
static struct {
  int seq;
  char scratch[sizeof(tyvar_prefix) + TYVER_SEQDIGITS_MAXLEN];
} tyvar_ctrl;
const char *fresh_tyvar ( SRC_POS_C pos ) {
  const char *tyv_id = NULL;
  
  const int n = strlen( tyvar_prefix );
  snprintf( &tyvar_ctrl.scratch[n], TYVER_SEQDIGITS_MAXLEN, "%d", tyvar_ctrl.seq++ );
  (&tyvar_ctrl.scratch[n])[TYVER_SEQDIGITS_MAXLEN] = 0;
  tyv_id = find_literal( tyvar_ctrl.scratch, pos );
  assert( tyv_id );
  return tyv_id;
}

static const char *asgn_fresh_tyvar ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) {
  assert( pty_cons );
  
  if( ! pty_cons->type.tyvars.var.ident ) {
    pty_cons->type.tyvars.var.ident = fresh_tyvar( pos );
    assert( pty_cons->type.tyvars.var.ident );
  }
  return pty_cons->type.tyvars.var.ident;
}
TYPE_CONS_PTR ty_curve ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) {
  if( pty_cons )
    switch( pty_cons->type.ty ) {
    case TY_INT:
    case TY_CHAR:
    case TY_STRING:
      break;
    case TY_LIST:
      assert( pty_cons->attrs.list.pty_elem );
      ty_curve( pty_cons->attrs.list.pty_elem, pos );
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

static struct {
  TYPE_CONS_PTR pavail;
  TYPE_CONS_PTR palive;
} type_cons_manage;
TYPE_CONS_PTR alloc_type_cons ( SRC_POS_C pos ) {
  TYPE_CONS_PTR ptycons = NULL;
  
  ptycons = (TYPE_CONS_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.pavail,
				       (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.palive,
				       sizeof(TYPE_CONS), NUM_TYCONS_PER_ALLOC, pos );
  if( ptycons )
    bzero( &ptycons->pos, (sizeof(TYPE_CONS) - sizeof(ALLOC_NODE_LINKS)) );
  return ptycons;
}

void free_type_cons ( TYPE_CONS_PTR ptycons ) {
  if( ptycons ) {
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_cons_manage.palive,
		(ALLOC_NODE_LINKS_PTR)ptycons );
  }
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

int enum_gentyvars ( TYPE_CONS_PTR *ppgen_tyvs, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  int ngvs = 0;
  assert( ppgen_tyvs );
  assert( pty );
  
  *ppgen_tyvs = NULL;
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
	  *ppgen_tyvs = pnote;
	ptail = pnote;
	ngvs++;
      } else {
	*ppgen_tyvs = NULL;
	ath_abort( pos, ABORT_MEMLACK );
      }
      pgv = pgv->type.tyvars.var.pnext;
    } while( pgv );
  }
  return ngvs;
}

TYPE_CONS_PTR gen_tyvars ( TYPE_CONS_PTR pty, TYPE_CONS_PTR pgen_tyvs, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_gen = NULL;
  assert( pty );
  assert( pgen_tyvs );
  
  pty_gen = dup_tydesc( pty, pos );
  if( pty_gen ) {
    TYPE_CONS_PTR ptvs = pgen_tyvs;
    assert( ptvs );
    do {
      BOOL found = FALSE;
      TYPE_CONS_PTR pgvs = pty_gen->type.tyvars.pgenvars;
      assert( ptvs->type.ty == TY_OTHERS );
      assert( ptvs->type.tyvars.var.ident );
      while( pgvs ) {
	assert( pgvs->type.tyvars.var.ident );
	if( strcmp( pgvs->type.tyvars.var.ident, ptvs->type.tyvars.var.ident ) == 0 ) {
	  found = TRUE;
	  break;
	}
	pgvs = pgvs->type.tyvars.var.pnext;
      }
      if( !found ) {
	TYPE_CONS_PTR pnew = NULL;
	assert( !pgvs );
	pnew = alloc_type_cons( pos );
	if( pnew ) {
	  pnew->pos = pos;
	  pnew->type.ty = TY_GEN;
	  pnew->type.tyvars.pgenvars = NULL;
	  pnew->type.pstuck = NULL;
	  pnew->type.tyvars.var.ident = ptvs->type.tyvars.var.ident;
	  pnew->type.tyvars.var.pnext = pty_gen->type.tyvars.pgenvars;
	  pty_gen->type.tyvars.pgenvars = pnew;
	} else {
	  pty_gen = NULL;
	  goto failed_memalloc;
	}
	assert( !pgvs );
      } else {
	assert( pgvs );
	assert( strcmp( pgvs->type.tyvars.var.ident, ptvs->type.tyvars.var.ident ) == 0 );
      }
      ptvs = ptvs->type.tyvars.var.pnext;
    } while( ptvs );
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
  if( ptymap )
    bzero( &ptymap->ident, (sizeof(TYPE_MAPSTO) - sizeof(ALLOC_NODE_LINKS)) );
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
  if( ptysubst )
    bzero( &ptysubst->pcomposit, (sizeof(TYPE_SUBST) - sizeof(ALLOC_NODE_LINKS)) );
  return ptysubst;
}

void free_type_subst ( TYPE_SUBST_PTR psubst ) {
  if( psubst ) {
    TYPE_MAPSTO_PTR ptymaps = NULL;
    if( psubst->pcomposit )
      free_type_subst( psubst->pcomposit );
    ptymaps = psubst->pmappings;
    while( ptymaps ) {
      TYPE_MAPSTO_PTR pn = NULL;
      pn = ptymaps->pnext;
      free_type_mapping( ptymaps );
      ptymaps = pn;
    }
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.palive,
		(ALLOC_NODE_LINKS_PTR)psubst );
  }
}

TYPE_CONS_PTR subst_map ( TYPE_SUBST_PTR psubst, const char *tyv_ident, SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  TYPE_MAPSTO_PTR ps_elem = NULL;
  assert( psubst );
  assert( tyv_ident );
  
  ps_elem = psubst->pmappings;
  while( ps_elem ) {
    assert( ps_elem->ident );
    assert( ps_elem->ptype );
    if( strcmp( ps_elem->ident, tyv_ident ) == 0 ) {
      r = ps_elem->ptype;
      break;
    }
    ps_elem = ps_elem->pnext;
  }
  return r;
}

TYPE_SUBST_PTR subst_add ( TYPE_SUBST_PTR psubst, const char *tyv_ident, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_MAPSTO_PTR pe = NULL;
  assert( psubst );
  assert( tyv_ident );
  assert( pty );
  
  pe = alloc_type_mapping( pos );
  if( pe ) {
    pe->ident = tyv_ident;
    pe->ptype = pty;
    pe->pnext = psubst->pmappings;
    psubst->pmappings = pe;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return psubst;
}

TYPE_SUBST_PTR dup_subst ( TYPE_SUBST_PTR psubst_org, SRC_POS_C pos ) {
  TYPE_SUBST_PTR psubst_dup = NULL;
  assert( psubst_org );
  
  psubst_dup = alloc_type_subst( pos );
  if( psubst_dup ) {
    TYPE_SUBST_PTR pcomps_dup = NULL;
    TYPE_MAPSTO_PTR plast = NULL;
    TYPE_MAPSTO_PTR pmap = NULL;
    if( psubst_org->pcomposit ) {
      pcomps_dup = dup_subst( psubst_org->pcomposit, pos );
      assert( pcomps_dup );
    }
    pmap = psubst_org->pmappings;
    while( pmap ) {
      TYPE_MAPSTO_PTR pnew = NULL;
      assert( pmap->ident );
      assert( pmap->ptype );
      pnew = alloc_type_mapping( pos );
      if( pnew ) {
	pnew->ident = pmap->ident;
	pnew->ptype = pmap->ptype;
	pnew->pnext = NULL;
	if( plast )
	  plast->pnext = pnew;
	else
	  psubst_dup->pmappings = pnew;
	plast = pnew;
      } else {
	psubst_dup = NULL;
	goto failed_memalloc;
      }
      pmap = pmap->pnext;
    }
    psubst_dup->pcomposit = pcomps_dup;
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return psubst_dup;
}

TYPE_SUBST_PTR comp_subst ( TYPE_SUBST_PTR psubst_1, TYPE_SUBST_PTR psubst_2, SRC_POS_C pos ) {
  TYPE_SUBST_PTR psub1_new = NULL;
  TYPE_SUBST_PTR psub2_new = NULL;
  assert( psubst_1 );
  assert( psubst_2 );
  
  if( SUBST_EMPTY( psubst_1 ) ) {
    psub1_new = dup_subst( psubst_2, pos );
    if( !psub1_new )
      goto failed_memalloc;
  } else if( SUBST_EMPTY( psubst_2 ) ) {
    psub1_new = dup_subst( psubst_1, pos );
    if( !psub1_new )
      goto failed_memalloc;
  } else {
    psub1_new = dup_subst( psubst_1, pos );
    if( psub1_new ) {
      psub2_new = dup_subst( psubst_2, pos );
      if( psub2_new ) {
	psub1_new->pcomposit = psub2_new;
      } else {
	psub1_new = NULL;
	goto failed_memalloc;
      }
    } else
    failed_memalloc:
      ath_abort( pos, ABORT_MEMLACK );
  }
  return psub1_new;
}

static TYPE_SUBST_PTR elim_subst_elems ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR tyvs_omit, SRC_POS_C pos ) {
  TYPE_SUBST_PTR psub_elim = NULL;
  assert( psubst );
  assert( tyvs_omit );

  psub_elim = psubst;
  if ( psub_elim->pcomposit )
    psub_elim->pcomposit = elim_subst_elems( psub_elim->pcomposit, tyvs_omit, pos );
  {
    TYPE_CONS_PTR pm_elim = tyvs_omit;
    while( pm_elim ) {
      TYPE_MAPSTO_PTR *ppm = NULL;
      assert( pm_elim->type.ty == TY_OTHERS );
      ppm = &psub_elim->pmappings;
      while( *ppm ) {
	assert( pm_elim->type.tyvars.var.ident );
	if( strcmp( (*ppm)->ident, pm_elim->type.tyvars.var.ident ) == 0 ) {
	  *ppm = (*ppm)->pnext;
	  break;
	}
	ppm = &(*ppm)->pnext;
      }
      pm_elim = pm_elim->type.tyvars.var.pnext;
    }
  }
  return psub_elim;
}
TYPE_SUBST_PTR restr_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR tyvs_omit, SRC_POS_C pos ) {
  TYPE_SUBST_PTR psub_restr = NULL;
  assert( psubst );
  assert( tyvs_omit );
  
  {
    TYPE_SUBST_PTR psub_dup = NULL;
    psub_dup = dup_subst( psubst, pos );
    if( psub_dup ) {
      psub_restr = elim_subst_elems( psub_dup, tyvs_omit, pos );
      assert( psub_restr );
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  return psub_restr;
}

static TYPE_CONS_PTR tyvar_rewrt ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_subst = NULL;
  assert( psubst );
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.pgenvars );
    pty_subst = pty;
    break;
  case TY_LIST:
    assert( ! pty->type.tyvars.var.ident );
    assert( pty->attrs.list.pty_elem );
    pty_subst = dup_tydesc( pty, pos );
    if( pty_subst ) {
      assert( pty_subst->type.ty == TY_LIST );
      assert( pty_subst->attrs.list.pty_elem == pty->attrs.list.pty_elem );
      pty_subst->attrs.list.pty_elem = tyvar_rewrt( psubst, pty_subst->attrs.list.pty_elem, pos );
      assert( pty_subst->attrs.list.pty_elem );
      if( pty_subst->attrs.list.pty_elem == pty->attrs.list.pty_elem )
	pty_subst = pty;
    } else
      ath_abort( pos, ABORT_MEMLACK );
    break;
  case TY_POLY:      
    assert( pty->type.tyvars.var.ident );
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
  if( penv )
    bzero( &penv->kind, (sizeof(TYENV_ELEM) - sizeof(ALLOC_NODE_LINKS)) );
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
  if( penv )
    bzero( &penv->pmappings, (sizeof(TYPE_ENV) - sizeof(ALLOC_NODE_LINKS)) );
  return penv;
}

void free_type_env ( TYPE_ENV_PTR penv ) {
  if( penv ) {
    TYENV_ELEM_PTR pelem = penv->pmappings;
    while( pelem ) {
      TYENV_ELEM_PTR pn = pelem->pnext;
      free_tyenv_elems( pelem );
      pelem = pn;
    }    
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.pavail,
		(ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.palive,
		(ALLOC_NODE_LINKS_PTR)penv );    
  }
}

TYPE_ENV_PTR env_link ( TYPE_ENV_PTR penv_pred, TYPE_ENV_PTR penv ) {
  assert( penv_pred );
  assert( penv );
  penv->uplink = penv_pred;
  penv_pred->dnlink = penv;
  return penv->uplink;
}

TYPE_ENV_PTR env_rid ( TYPE_ENV_PTR penv, const char *var_ident ) {
  BOOL found = FALSE;
  TYENV_ELEM_PTR *ppe = NULL;
  assert( penv );
  assert( var_ident );
  
  ppe = &penv->pmappings;
  while( *ppe ) {
    assert( (*ppe)->decl.var.v.ident );
    assert( (*ppe)->decl.var.v.ptype );
    if( strcmp( (*ppe)->decl.var.v.ident, var_ident ) == 0 ) {
      found = TRUE;
      *ppe = (*ppe)->pnext;
      break;
    }
    ppe = &(*ppe)->pnext;
  }
  if( !found ) {
    assert( ! *ppe );
    if( penv->uplink )
      penv = env_rid( penv->uplink, var_ident );
    else
      penv = NULL;
  }
  return penv;
}

#if 0 // *****
TYPE_ENV_PTR env_add ( TYPE_ENV_PTR penv, const char *var_ident, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( var_ident );
  assert( pty );
  
  pe = alloc_tyenv_elem( pos );
  if( pe ) {
    pe->decl.var.v.ident = var_ident;
    pe->decl.var.v.ptype = pty;
    pe->pnext = penv->pmappings;
    penv->pmappings = pe;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return penv;
}
#else
TYPE_ENV_PTR env_add1 ( TYPE_ENV_PTR penv, DECLARATION_PTR pdecl, SRC_POS_C pos ) {
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( pdecl );
  assert( pdecl->ident );
  
  pe = alloc_tyenv_elem( pos );
  if( pe ) {
    pe->decl.var.v.ident = pdecl->ident;
    assert( pdecl->u.variable.pvar );
    assert( (pdecl->u.variable.pvar)->ptype );
    pe->decl.var.v.ptype = (pdecl->u.variable.pvar)->ptype;
    pe->decl.var.plnk_symtbl = pdecl->u.variable.pvar;
    pe->pnext = penv->pmappings;
    penv->pmappings = pe;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return penv;
}
#endif

static TYENV_ELEM_PTR env_search ( TYPE_ENV_PTR penv, const char *var_ident, BOOL dir ) {
  BOOL found = FALSE;
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( var_ident );
  
  pe = penv->pmappings;
  while( pe ) {
    assert( pe->decl.var.v.ident );
    assert( pe->decl.var.v.ptype );
    if( strcmp( pe->decl.var.v.ident, var_ident ) == 0 ) {
      found = TRUE;
      break;
    }
    pe = pe->pnext;
  }
  if( !found ) {
    TYPE_ENV_PTR penv_succ = NULL;
    assert( !pe );
    if( dir ) // upward
      penv_succ = penv->uplink;
    else // downward
      penv_succ = penv->dnlink;
    if( penv_succ )
      pe = env_search( penv_succ, var_ident, dir );
  } else
    assert( pe );
  return pe;
}
TYENV_ELEM_PTR env_lkup ( TYPE_ENV_PTR penv, const char *var_ident ) {
  TYENV_ELEM_PTR pe_found = NULL;
  assert( penv );
  assert( var_ident );
  
  pe_found = env_search( penv, var_ident, TRUE );
  return pe_found;
}

TYPE_ENV_PTR dup_env ( TYPE_ENV_PTR penv_org, SRC_POS_C pos ) {
  TYPE_ENV_PTR penv = NULL;
  assert( penv_org );
  
  penv = alloc_type_env( pos );
  if( penv ) {
    TYPE_ENV_PTR penv_up = NULL;
    TYENV_ELEM_PTR pprev = NULL;
    TYENV_ELEM_PTR pmap = NULL;
    if( penv_org->uplink ) {
      penv_up = dup_env( penv_org->uplink, pos );
      assert( penv_up );
    }
    penv->uplink = NULL;
    penv->dnlink = NULL;
    penv->pmappings = NULL;    
    pmap = penv_org->pmappings;
    while( pmap ) {
      TYENV_ELEM_PTR pnew = NULL;
      pnew = alloc_tyenv_elem( pos );
      if( pnew ) {
	assert( pmap->decl.var.v.ident );
	assert( pmap->decl.var.v.ptype );
	pnew->decl.var = pmap->decl.var;
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
    if( penv_up )
      env_link( penv_up, penv );
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return penv;
}

static TYPE_ENV_PTR env_subst_elem ( TYPE_ENV_PTR penv, TYPE_SUBST_PTR psubst, SRC_POS_C pos ) {
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( psubst );
  
  pe = penv->pmappings;
  while( pe ) {
    TYPE_CONS_PTR pty_s = NULL;
    assert( pe->decl.var.v.ident );
    assert( pe->decl.var.v.ptype );
    if( penv->dnlink ) {
      if( env_search( penv->dnlink, pe->decl.var.v.ident, 0 ) ) {
	pe = pe->pnext;
	continue;
      }
    }
    pty_s = ty_subst( psubst, pe->decl.var.v.ptype, pos );
    assert( pty_s );
    pe->decl.var.v.ptype = pty_s;
    pe = pe->pnext;
  }
  if( penv->uplink ) {
    TYPE_ENV_PTR penv_u = NULL;
    penv_u = env_subst_elem( penv->uplink, psubst, pos );
    assert( penv_u );
    assert( penv_u->dnlink == penv );
    assert( penv->uplink == penv_u );
  }
  return penv;
}
TYPE_ENV_PTR env_subst ( TYPE_ENV_PTR penv, TYPE_SUBST_PTR psubst, SRC_POS_C pos ) {
  TYPE_ENV_PTR penv_s = NULL;
  assert( penv );
  assert( psubst );
  
  penv_s = dup_env( penv, pos );
  if( penv_s )
    env_subst_elem( penv_s, psubst, pos );
  else
    ath_abort( pos, ABORT_MEMLACK );
  return penv_s;
}

char *print_type ( char *sbuf, TYPE_CONS_PTR_C pty_desc ) {
  //SRC_POS pos;
  char *ps = NULL;  
  assert( sbuf );
  assert( pty_desc );
  
  ps = sbuf;
  //pos = pty_desc->pos;
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
      ps = print_type( ps, pty_desc->attrs.list.pty_elem );
    } else {
      strcpy( ps, "UNKNOWN_TYPE" );
      ps += strlen( ps );
    }
    assert( *ps == 0 );
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
