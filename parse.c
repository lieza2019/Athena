#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

void poly_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, SRC_POS_C pos ) {
  assert( pvar_attr );
  assert( pvar_name );
  assert( strlen(pvar_name) >= 1 );
  char *pident = NULL;
  
  pident = find_literal( pvar_name );
  if( pident ) {
    assert( strcmp( pident, pvar_name ) == 0 );
    pvar_attr->ident = pident;
    pvar_attr->type = TY_POLY;
    pvar_attr->pos = pos;
  } else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, pos );
}

void int_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, const int n_init, SRC_POS_C pos ) {
  assert( pvar_attr );
  assert( pvar_name );
  assert( strlen(pvar_name) >= 1 );
  char *pident = NULL;
  
  pident = find_literal( pvar_name );
  if( pident ) {
    assert( strcmp( pident, pvar_name ) == 0 );
    pvar_attr->ident = pident;
    pvar_attr->type = TY_INT;
    pvar_attr->u.var_int.init_n = n_init;
    pvar_attr->pos = pos;
  } else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, pos );
}

TYPE_CONS_PTR cons_list_elem_basetype ( TYPE_CODE ty, SRC_POS_C pos ) {
  assert( (ty > 0) && (ty < END_OF_TYPE_CODE) );
  TYPE_CONS_PTR r = NULL;
  TYPE_CONS_PTR pty_cons = NULL;
  pty_cons = new_memarea( sizeof(TYPE_CONS) );
  if( pty_cons ) {
    TYPE_CONS_PTR pty_elem = NULL;
    pty_elem = new_memarea( sizeof(TYPE_CONS) );
    if( pty_elem ) {
      assert( pty_cons );
      pty_elem->type = ty;
      pty_cons->type = TY_LIST;
      pty_cons->u.list.pty_elem = pty_elem;
      pty_cons->pos.row = pos.row;
      pty_cons->pos.col = pos.col;
      r = pty_cons;
    } else
      goto err;
  } else
  err:
    ath_abort( ABORT_MEMLACK, pos );
  return r;
}
