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

void string_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, const char *s_init, SRC_POS_C pos ) {
  assert( pvar_attr );
  assert( pvar_name );
  assert( strlen(pvar_name) >= 1 );
  char *pident = NULL;
  
  pident = find_literal( pvar_name );
  if( pident ) {
    pvar_attr->ident = pident;
    pvar_attr->type = TY_STRING;
    if( !s_init ) {
      char *pc = NULL;
      pc = new_memarea( sizeof(char) );
      if( pc ) {
	*pc = 0;
	s_init = pc;
      } else
	ath_abort( ABORT_MEMLACK, pos );
    }
    assert( s_init );
    pvar_attr->u.var_str.init_s = s_init;
    pvar_attr->pos = pos;
  } else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, pos );
}

#if 0
void string_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, const TYPE_CODE *ty_init, const int n_init_elems, SRC_POS_C pos ) {
decl_var_list : TK_IDENT TK_KEYWORD_AS list_elem_type decl_list_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  char *pident = NULL;
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.type = TY_LIST;
    $$.u.var_list.pty = $3;
    $$.u.var_list.init_l = $4;
    $$.pos.row = @1.first_line;
    $$.pos.col = @1.first_column;
  } else   
    ath_abort( ABORT_CANNOT_REG_SYNBOL, pos );
  assert( FALSE );
  }
}
#endif

TYPE_CONS_PTR list_var_attrib ( TYPE_CODE ty, SRC_POS_C pos ) {
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
      pty_cons->u.list.cdr = pty_elem;
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

#if 0
BOOL list_is_nill ( TYPE_CONS_PTR pcons_list ) {
  BOOL r = FALSE;
  
  assert( pcons_list );
  if( pcons_list->type == TY_LIST ) {
    r = !(pcons_list->u.list.cdr) && !(pcons_list->u.list.car);
  }
  return r;
}
#endif

