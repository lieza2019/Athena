#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

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

#define NUM_TYELEMS_PER_ALLOC 256

typedef struct type_env_elem {
  VAR_DECL var;
  TYPE_CONS type;
  struct type_env_elem *pnext;
  struct {
    struct type_env_elem *pnext;
    struct type_env_elem *pprev;
  } alloc;
} TYPE_ENV_ELEM, *TYPE_ENV_ELEM_PTR;

struct {
  LIST_CELL_PTR pavail;
  LIST_CELL_PTR palive;
} cells_manage;

LIST_CELL_PTR alloc_tyenv_elem ( SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  if( !cells_manage.pavail ) {
    cells_manage.pavail = new_memarea( sizeof(LIST_CELL) * NUM_TYELEMS_PER_ALLOC );
    if( ! cells_manage.pavail )
      ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
    assert( cells_manage.pavail );
    {
      LIST_CELL_PTR pc = cells_manage.pavail;
      while( pc < (cells_manage.pavail + (NUM_TYELEMS_PER_ALLOC - 1)) ) {
	pc->alloc.pprev = NULL;
	pc->alloc.pnext = (pc + 1);
	pc++;
	assert( !pc->alloc.pnext );
      }
    }
  }
  assert( cells_manage.pavail );
  r = cells_manage.pavail;
  cells_manage.pavail = r->alloc.pnext;
  r->alloc.pnext = NULL;
  r->alloc.pprev = NULL;
  if( cells_manage.palive ) {
    (cells_manage.palive)->alloc.pprev = r;
    r->alloc.pnext = cells_manage.palive;
  }
  cells_manage.palive = r;
  if( r ) {
    r->u.list.pty_elem = NULL;
    r->u.list.car = NULL;
    r->u.list.cdr = NULL;
    r->u.list.plast = NULL;
  }
  return r;
}
