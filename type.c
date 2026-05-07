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
  
  ident = (char *)new_memarea( strlen(prefix) + TYVER_SEQDIGITS_MAXLEN );
  if( ident ) {
    const int n = strlen( prefix );
    snprintf( &ident[n], TYVER_SEQDIGITS_MAXLEN, "%d", tyver_ctrl.seq );
    (&ident[n])[TYVER_SEQDIGITS_MAXLEN - 1] = 0;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return ident;
}

TYPE_CONS_PTR asgn_tyvar ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos ) {
  assert( pty_cons );
  switch( pty_cons->type.ty ) {
  case TY_INT: case TY_CHAR: case TY_STRING:
    break;  
  case TY_LIST:
    asgn_tyvar( pty_cons->attrs.list.pty_elem, pos );
    break;
  case TY_POLY:
    assert( ! pty_cons->type.tyvars.var.pnext );
    if( ! pty_cons->type.tyvars.var.ident ) {
      pty_cons->type.tyvars.var.ident = fresh_tyvar( pos );
      assert( pty_cons->type.tyvars.var.ident );
    }
    break;
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
  
  ptycons = (TYPE_CONS_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.pavail, (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.palive,
				       sizeof(TYPE_CONS), NUM_TYCONS_PER_ALLOC, pos );
  return ptycons;
}

void free_type_cons ( TYPE_CONS_PTR ptycons ) {
  if( ptycons ) {
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.pavail, (ALLOC_NODE_LINKS_PTR *)&type_cons_manage.palive, (ALLOC_NODE_LINKS_PTR)ptycons );
  }
}

TYPE_CONS_PTR dup_tydesc ( TYPE_CONS_PTR ptydesc_org, SRC_POS_C pos ) {
  TYPE_CONS_PTR ptydesc = NULL;
  
  ptydesc = alloc_type_cons( pos );
  if( ptydesc ) {
    assert( ptydesc_org );
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

int enum_gentyvers ( TYPE_CONS_PTR *ppgens_id, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  int ngentyvers = 0;
  assert( ppgens_id );
  assert( pty );
  
  *ppgens_id = NULL;
  if( pty->type.tyvars.pgenvars ) {
    TYPE_CONS_PTR ptail = NULL;
    TYPE_CONS_PTR pgv = pty->type.tyvars.pgenvars;
    do {
      TYPE_CONS_PTR pnote = NULL;
      assert( pgv->type.tyvars.var.ident );
      pnote = alloc_type_cons( pos );
      if( pnote ) {
	bzero( pnote, sizeof(TYPE_CONS) );
	pnote->type.ty = TY_GEN;
	pnote->type.tyvars.var.ident = pgv->type.tyvars.var.ident;
	pnote->type.tyvars.var.pnext = NULL;
	if( ptail )
	  ptail->type.tyvars.var.pnext = pnote;	
	else
	  *ppgens_id = pnote;
	ptail = pnote;
	ngentyvers++;
      } else {
	*ppgens_id = NULL;
	ath_abort( pos, ABORT_MEMLACK );
      }
      pgv = pgv->type.tyvars.var.pnext;
    } while( pgv );
  }
  return ngentyvers;
}

TYPE_CONS_PTR gen_tyvers ( TYPE_CONS_PTR pty, TYPE_CONS_PTR pgen_tyvers, SRC_POS_C pos ) {
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
  
  ptymap = (TYPE_MAPSTO_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.pavail, (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.palive,
					sizeof(TYPE_MAPSTO), NUM_TYMAPS_PER_ALLOC, pos );
  return ptymap;
}

void free_type_mapping ( TYPE_MAPSTO_PTR ptymap ) {
  if( ptymap ) {
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.pavail, (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.mapping.palive, (ALLOC_NODE_LINKS_PTR)ptymap );
  }
}

TYPE_SUBST_PTR alloc_type_subst ( SRC_POS_C pos ) {
  TYPE_SUBST_PTR ptysubst = NULL;
  
  ptysubst = (TYPE_SUBST_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.pavail, (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.palive,
					 sizeof(TYPE_SUBST), NUM_TYSUBSTS_PER_ALLOC, pos );
  return ptysubst;
}

void free_type_subst ( TYPE_SUBST_PTR ptysubst ) {
  if( ptysubst ) {
    TYPE_MAPSTO_PTR ptymap = NULL;
    if( ptysubst->pcomposit )
      free_type_subst( ptysubst->pcomposit );
    ptymap = ptysubst->pmappings;
    while( ptymap ) {
      TYPE_MAPSTO_PTR pnext = ptymap->pnext;
      free_type_mapping( ptymap );
      ptymap = pnext;
    }
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.pavail, (ALLOC_NODE_LINKS_PTR *)&type_subst_manage.subst.palive, (ALLOC_NODE_LINKS_PTR)ptysubst );
  }
}

TYPE_SUBST_PTR dup_subst ( TYPE_SUBST_PTR psub_org, SRC_POS_C pos ) {
  TYPE_SUBST_PTR psubst = NULL;
  
  psubst = alloc_type_subst( pos );
  if( psubst ) {
    TYPE_MAPSTO_PTR pprev = NULL;
    TYPE_MAPSTO_PTR pmap = NULL;
    assert( psub_org );
    if( psub_org->pcomposit )
      dup_subst( psub_org->pcomposit, pos );
    pmap = psub_org->pmappings;
    while( pmap ) {
      TYPE_MAPSTO_PTR pnew = NULL;
      pnew = alloc_type_mapping( pos );
      if( pnew ) {
	pnew->ident = pmap->ident;
	pnew->pty = dup_tydesc( pmap->pty, pos );
	assert( pnew->pty );
	pnew->pnext = NULL;
	if( pprev )
	  pprev->pnext = pnew;
	else
	  psubst->pmappings = pnew;
	pprev = pnew;
	pmap = pmap->pnext;
      } else {
	psubst = NULL;
	goto failed_memalloc;
      }
    }
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return psubst;
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
  assert( psubst );
  assert( tyvers_omit );
  if( psubst->pcomposit )
    psubst = elim_subst_elems( psubst->pcomposit, tyvers_omit, pos );
  {
    TYPE_CONS_PTR pgv = tyvers_omit->type.tyvars.pgenvars;
    while( pgv ) {
      TYPE_MAPSTO_PTR *ppm = NULL;      
      assert( psubst );
      ppm = &psubst->pmappings;      
      while( *ppm ) {
	assert( pgv->type.tyvars.var.ident );
	if( strcmp( (*ppm)->ident, pgv->type.tyvars.var.ident ) == 0 ) {
	  TYPE_MAPSTO_PTR pe = *ppm;
	  assert( pe );
	  *ppm = pe->pnext;
	  free_type_mapping( pe );
	  break;
	} else
	  ppm = &(*ppm)->pnext;
      }
      pgv = pgv->type.tyvars.var.pnext;
      assert( pgv ? (pgv->type.ty == TY_GEN) : TRUE );
    }
  }
  return psubst;
}
static TYPE_SUBST_PTR restrict_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR tyvers_omit, SRC_POS_C pos ) {
  TYPE_SUBST_PTR pr_subst = NULL;
  assert( psubst );
  assert( tyvers_omit );
  
  pr_subst = dup_subst( psubst, pos );
  if( pr_subst ) {
    pr_subst = elim_subst_elems( pr_subst, tyvers_omit, pos );
    assert( pr_subst );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return pr_subst;
}

static TYPE_CONS_PTR copy_gentyvers ( TYPE_CONS_PTR pty, TYPE_CONS_PTR pgen_from, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_gen = NULL;
  TYPE_CONS_PTR pgvs = NULL;
  int ngenvars = 0;
  
  ngenvars = enum_gentyvers( &pgvs, pgen_from, pos );
  if( ngenvars > 0 ) {
    assert( pgvs );
    pty_gen = gen_tyvers( pty, pgvs, pos );
    assert( pty_gen );
  }
  return pty_gen;
}

static TYPE_CONS_PTR tyvar_rewrit ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_rewr = NULL;
  TYPE_MAPSTO_PTR ps_elem = NULL;
  assert( psubst );
  assert( pty );
  assert( pty->type.ty == TY_POLY );
  
  ps_elem = psubst->pmappings;
  while( ps_elem ) {
    assert( ps_elem->ident );
#ifdef RUNTIME_CONSITENCY_CHECK
    {
      TYPE_CONS_PTR pgv = pty->type.tyvars.pgenvars;
      while( pgv ) {
	assert( pgv->type.ty == TY_GEN );
	assert( strcmp( ps_elem->ident, pgv->type.tyvars.var.ident ) != 0 );
	pgv = pgv->type.tyvars.var.pnext;
      }
    }
#endif // RUNTIME_CONSITENCY_CHECK
    assert( pty->type.tyvars.var.ident );
    if( strcmp( ps_elem->ident, pty->type.tyvars.var.ident ) == 0 ) {
      assert( ps_elem->pty );
      pty_rewr = dup_tydesc( ps_elem->pty, pos );
      if( !pty_rewr )
	goto failed_memalloc;
      break;
    }
    ps_elem = ps_elem->pnext;
  }
  if( !pty_rewr ) {
    pty_rewr = dup_tydesc( pty, pos );
    if( !pty_rewr )
    failed_memalloc:
      ath_abort( pos, ABORT_MEMLACK );
  }
  return pty_rewr;
}

TYPE_CONS_PTR ty_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_subst = NULL;
  assert( psubst );
  assert( pty );
  
  switch( pty->type.ty ) {
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    assert( ! pty->type.tyvars.pgenvars );
    pty_subst = dup_tydesc( pty, pos );
    assert( pty_subst );
    break;
  case TY_LIST:
    assert( ! pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    pty_subst = alloc_type_cons( pos );
    if( pty_subst ) {
      TYPE_SUBST_PTR ps_r = NULL;
      TYPE_CONS_PTR pty_s = NULL;
      TYPE_CONS_PTR pty_s_car = NULL;
      TYPE_CONS_PTR pty_s_cdr = NULL;
      pty_subst->type = pty->type;
      pty_subst->attrs = pty->attrs;
      pty_subst->type.pstuck = NULL;
      ps_r = restrict_subst( psubst, pty, pos );
      assert( ps_r );            
      assert( pty->attrs.list.pty_elem );
      pty_s = ty_subst( ps_r, pty->attrs.list.pty_elem, pos );
      assert( pty_s );      
      if( pty->attrs.list.car ) {
	pty_s_car = ty_subst( ps_r, pty->attrs.list.car, pos );
	assert( pty_s_car );
	if( pty->attrs.list.cdr ) {
	  pty_s_cdr = ty_subst( ps_r, pty->attrs.list.cdr, pos );
	  assert( pty_s_cdr );
	}
      } else
	assert( ! pty->attrs.list.cdr );
      pty_subst->attrs.list.pty_elem = pty_s;
      pty_subst->attrs.list.car = pty_s_car;
      pty_subst->attrs.list.cdr = pty_s_cdr;
      pty_subst->attrs.list.plast = pty_subst->attrs.list.car;
      if( pty_subst->attrs.list.cdr ) {
	TYPE_CONS_PTR pl = pty_subst->attrs.list.cdr;
	while( pl->attrs.list.cdr ) {
	  assert( pl->attrs.list.car );
	  pl = pl->attrs.list.cdr;
	}
	assert( pl );
	assert( pl->attrs.list.car );
	pty_subst->attrs.list.plast = pl;
      }
      {
	TYPE_CONS_PTR pty_s_gen = NULL;
	pty_s_gen = copy_gentyvers( pty_subst, pty, pos );
	if( pty_s_gen )
	  pty_subst = pty_s_gen;
      }
    }
    break;
  case TY_POLY:      
    assert( pty->type.tyvars.var.ident );
    assert( ! pty->type.tyvars.var.pnext );
    {
      TYPE_SUBST_PTR pr_subst = NULL;
      pr_subst = restrict_subst( psubst, pty, pos );
      assert( pr_subst );
      pty_subst = tyvar_rewrit( pr_subst, pty, pos );
      assert( pty_subst );
      free_type_subst( pr_subst );
    }
    break;
  case TY_GEN:
    assert( FALSE );
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return pty_subst;
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
  
  penv = (TYENV_ELEM_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.pavail, (ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.palive,
				     sizeof(TYENV_ELEM), NUM_TYELEMS_PER_ALLOC, pos );
  return penv;
}

void free_tyenv_elems ( TYENV_ELEM_PTR pelem ) {
  if( pelem ) {    
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.pavail, (ALLOC_NODE_LINKS_PTR *)&type_env_manage.mapping.palive, (ALLOC_NODE_LINKS_PTR)pelem );
  }
}

TYPE_ENV_PTR alloc_type_env ( SRC_POS_C pos ) {
  TYPE_ENV_PTR penv = NULL;
  
  penv = (TYPE_ENV_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.pavail, (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.palive,
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
    free_node ( (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.pavail, (ALLOC_NODE_LINKS_PTR *)&type_env_manage.env.palive, (ALLOC_NODE_LINKS_PTR)penv );    
  }
}

TYPE_ENV_PTR env_rid ( TYPE_ENV_PTR penv, const char *var_ident ) {
  TYENV_ELEM_PTR *ppe = NULL;
  assert( penv );
  assert( var_ident );
  
  ppe = &penv->pmappings;
  while( *ppe ) {
    assert( (*ppe)->ptype );
    assert( (*ppe)->pvar );
    assert( ((*ppe)->pvar)->ident );
    if( strcmp( ((*ppe)->pvar)->ident, var_ident ) == 0 ) {
      *ppe = (*ppe)->pnext;
      break;
    }
    ppe = &(*ppe)->pnext;
  }
  return penv;
}

TYPE_ENV_PTR env_add ( TYPE_ENV_PTR penv, TYENV_ELEM_PTR pelem ) {
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( pelem );
  
  pe = penv->pmappings;
  pelem->pnext = pe;
  penv->pmappings = pelem;
  return penv;
}

TYENV_ELEM_PTR env_lkup ( TYPE_ENV_PTR penv, const char *var_ident ) {
  TYENV_ELEM_PTR pe = NULL;
  assert( penv );
  assert( var_ident );
  
  pe = penv->pmappings;
  while( pe ) {
    assert( pe->ptype );
    assert( pe->pvar );
    assert( (pe->pvar)->ident );
    if( strcmp( (pe->pvar)->ident, var_ident ) == 0 )
      break;
    pe = pe->pnext;
  }
  return pe;
}

TYPE_ENV_PTR dup_env ( TYPE_ENV_PTR penv_org, SRC_POS_C pos ) {
  TYPE_ENV_PTR penv = NULL;
  
  penv = alloc_type_env( pos );
  if( penv ) {
    TYENV_ELEM_PTR pprev = NULL;
    TYENV_ELEM_PTR pmap = NULL;
    penv->pmappings = NULL;
    assert( penv_org );
    pmap = penv_org->pmappings;
    while( pmap ) {
      TYENV_ELEM_PTR pnew = NULL;
      pnew = alloc_tyenv_elem( pos );
      if( pnew ) {
	pnew->pvar = pmap->pvar;
	pnew->ptype = pmap->ptype;
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
    TYENV_ELEM_PTR pe = penv_s->pmappings;
    while( pe ) {
      TYPE_CONS_PTR pty_s = NULL;
      assert( pe->pvar );
      assert( pe->ptype );
      pty_s = ty_subst( psubst, pe->ptype, pos );
      assert( pty_s );
      pe->ptype = pty_s;
      pe = pe->pnext;
    }
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
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ps;
}
