#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

char *ath_type_name[] = {
  "UNASSIGNED", 
  "int", // TY_INT = 1,
  "string", // TY_STRING
  "[]", // TY_LIST
  "poly", // TY_POLY
  "END_OF_TYPE_CODE" // END_OF_TYPE_CODE  
};

static char *show_list_type ( char *sbuf, TYPE_CONS_PTR_C pty_cons ) {
  SRC_POS pos;
  char *ps = NULL;
  assert( sbuf );
  assert( pty_cons );

  pos = pty_cons->pos;
  ps = sbuf;
  switch( pty_cons->type ) {
  case TY_INT: case TY_STRING: case TY_POLY:
    ps = strcat( ps, ath_type_name[pty_cons->type] );
    break;
  case TY_LIST:
    if( pty_cons->u.list.cdr ) {
      ps = strcat( ps, "[" );
      ps = show_list_type( ps, pty_cons->u.list.cdr );
      ps = strcat( ps, "]" );
    } else
      ath_abort( INTERNALERR_TYPE_CONS, pty_cons->pos );
    break;
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ps;
}
char *show_var_type ( char *sbuf, VAR_DECL_PTR pvar_decl ) {
  SRC_POS pos;
  char *ps = NULL;  
  assert( sbuf );
  assert( pvar_decl );

  pos = pvar_decl->pos;
  ps = sbuf;
  switch( pvar_decl->type ) {
  case TY_INT:
    ps = strcpy( ps, "int" );
    break;
  case TY_STRING:
    ps = strcpy( ps, "string" );
    break;
  case TY_LIST:
    if( pvar_decl->u.var_list.pty )
      ps = show_list_type( ps, pvar_decl->u.var_list.pty );
    else
      ath_abort( INTERNALERR_TYPE_CONS, pvar_decl->pos );
    break;
  case TY_POLY:
    ps = strcpy( ps, "poly" );
    break;
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ps;
}
