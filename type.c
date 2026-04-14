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
  switch( pty_desc->type ) {
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
  if( pty1->type == TY_LIST ) {
    if( pty2->type == TY_LIST ) {
      assert( pty1->u.list.pty_elem );
      assert( pty2->u.list.pty_elem );
      r = typecheck( pty1->u.list.pty_elem, pty2->u.list.pty_elem );
    }
  } else {
    assert( pty1->type != TY_LIST );
    switch( pty1->type ) {
    case TY_INT:
    case TY_STRING:
      r = (pty1->type == pty2->type);
      break;
    default:
      assert( FALSE );
    }
  }
  return r;
}

typedef struct type_env_elem {
  VAR_DECL var;
  TYPE_CONS type;
  struct type_env_elem *pnext;
} TYPE_ENV_ELEM, *TYPE_ENV_ELEM_PTR;
