#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

#if 0
static struct {
  TYPE_ENV_ELEM_PTR pavail;
  TYPE_ENV_ELEM_PTR palive;
} tyenv_elems_manage;
TYPE_ENV_ELEM_PTR alloc_tyenv_elem ( SRC_POS_C pos ) {
  TYPE_ENV_ELEM_PTR r = NULL;
  
  if( !tyenv_elems_manage.pavail ) {
    tyenv_elems_manage.pavail = new_memarea( sizeof(TYPE_ENV_ELEM) * NUM_TYELEMS_PER_ALLOC );
    if( tyenv_elems_manage.pavail ) {
      TYPE_ENV_ELEM_PTR pe = tyenv_elems_manage.pavail;
      while( pe < (tyenv_elems_manage.pavail + (NUM_TYELEMS_PER_ALLOC - 1)) ) {
	pe->alloc.pprev = NULL;
	pe->alloc.pnext = (pe + 1);
	pe++;
	assert( !pe->alloc.pnext );
      }
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  assert( tyenv_elems_manage.pavail );
  r = tyenv_elems_manage.pavail;
  tyenv_elems_manage.pavail = r->alloc.pnext;
  bzero( r, sizeof(TYPE_ENV_ELEM) );
  if( tyenv_elems_manage.palive ) {
    assert( ! (tyenv_elems_manage.palive)->alloc.pprev );
    (tyenv_elems_manage.palive)->alloc.pprev = r;
    r->alloc.pnext = tyenv_elems_manage.palive;
  }
  tyenv_elems_manage.palive = r;
  return r;
}

void free_tyenv_elems ( TYPE_ENV_ELEM_PTR pelem ) {  
  TYPE_ENV_ELEM_PTR pp = pelem->alloc.pprev;
  assert( pelem );
  
  if( pp ) {
    TYPE_ENV_ELEM_PTR pn = pelem->alloc.pnext;
    assert( pelem != tyenv_elems_manage.palive );
    pp->alloc.pnext = pn;
    if( pn )
      pn->alloc.pprev = pp;
    pelem->alloc.pprev = NULL;
  } else {
    assert( pelem == tyenv_elems_manage.palive );
    tyenv_elems_manage.palive = pelem->alloc.pnext;
    if( tyenv_elems_manage.palive )
      tyenv_elems_manage.palive->alloc.pprev = NULL;
  }
  assert( !pelem->alloc.pprev );
  pelem->alloc.pnext = tyenv_elems_manage.pavail;
  tyenv_elems_manage.pavail = pelem;  
}
#else
static struct {
  TYPE_ENV_ELEM_PTR pavail;
  TYPE_ENV_ELEM_PTR palive;
} tyenv_elems_manage;
TYPE_ENV_ELEM_PTR alloc_tyenv_elem ( SRC_POS_C pos ) {
  TYPE_ENV_ELEM_PTR penv = NULL;
  penv = (TYPE_ENV_ELEM_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&tyenv_elems_manage.pavail, (ALLOC_NODE_LINKS_PTR *)&tyenv_elems_manage.palive, sizeof(TYPE_ENV_ELEM), NUM_TYELEMS_PER_ALLOC, pos );
  return penv;
}

void free_tyenv_elems ( TYPE_ENV_ELEM_PTR pelem ) {
}
#endif

#define TYVER_SEQDIGITS_MAXLEN 8
struct {
  int seq;
} tyver_ctrl;
char *fresh_tyver ( SRC_POS_C pos ) {
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

static TYPE_CONS_PTR infer ( TYPE_ENV_ELEM_PTR penv, EXPR_CONS_PTR pexpr ) {
  TYPE_CONS_PTR pty_expr = NULL;
  assert( penv );
  assert( pexpr );
  switch( pexpr->mnemonic ) {
  case MNC_CALL:
    break;
  case MNC_ASGN:
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
TYPE_CONS_PTR typematch ( TYPE_ENV_ELEM_PTR penv, STATEMENT_PTR pstmt ) {
  TYPE_CONS_PTR pty_stmt = NULL;
  assert( penv );
  assert( pstmt );
  switch( pstmt->sort ) {
  case STMT_DECL:
    {
      EXPR_CONS asgn;
      EXPR_CONS l, r;
      l.pos = pstmt->pos;
      l.mnemonic = MNC_LVALUE;
      l.kids.pdaugh = &(pstmt->u.pdecl)->u.variable.var;
      r.pos = pstmt->pos;
      r.mnemonic = MNC_RVALUE;
      r.kids.pdaugh = &(pstmt->u.pdecl)->u.variable.var.u.var_int;
      asgn.pos = pstmt->pos;
      asgn.mnemonic = MNC_ASGN;
      asgn.kids.pleft = &l;
      asgn.kids.pright = &r;
      pty_stmt = infer( penv, &asgn );
    }
    break;
  case STMT_EXPR:
    infer( penv, pstmt->u.pexpr );
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
