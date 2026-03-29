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
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
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
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
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
	ath_abort( pos, ABORT_MEMLACK );
    }
    assert( s_init );
    pvar_attr->u.var_str.init_s = s_init;
    pvar_attr->pos = pos;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}
