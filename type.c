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
    asgn_tyvar( pty_cons->u.list.pty_elem, pos );
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

BOOL typecheck ( TYPE_CONS_PTR_C pty1, TYPE_CONS_PTR_C pty2 ) {
  BOOL r = FALSE;
  
  assert( pty1 );
  assert( pty2 );
  if( pty1->type.ty == TY_LIST ) {
    if( pty2->type.ty == TY_LIST ) {
      assert( pty1->u.list.pty_elem );
      assert( pty2->u.list.pty_elem );
      r = typecheck( pty1->u.list.pty_elem, pty2->u.list.pty_elem );
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
    *ptydesc = *ptydesc_org;
    ptydesc->type.pstuck = ptydesc_org;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return ptydesc;
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
    /* free pelem->pvar, then. */
    free_type_cons( pelem->ptype );
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
    free_type_cons( ptymap->pty );
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
    if( pty_desc->u.list.pty_elem ) {
      ps = print_var_type( ps, pty_desc->u.list.pty_elem );
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
