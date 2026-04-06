#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

TYPE_CONS_PTR alloc_tycons_node ( SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  r = (TYPE_CONS_PTR)alloc_list_cell( pos );
  return r;
}
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

static void int_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, SRC_POS_C pos ) {
  char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );

  assert( pvar_attr->type == TY_INT );
  pident = find_literal( pvar_name );
  if( pident ) {
    assert( strcmp( pident, pvar_name ) == 0 );
    pvar_attr->ident = pident;
    pvar_attr->pos = pos;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void string_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, const char *s_init, SRC_POS_C pos ) {
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

VAR_DECL_PTR decl_attrib_var ( VAR_DECL_PTR pvar_attr, char *pvar_name, const void *pinit, SRC_POS_C pos ) {
  assert( pvar_attr );
  assert( pvar_name );
  switch( pvar_attr->type ) {
  case TY_INT:
    int_var_attrib( pvar_attr, pvar_name, pos );
    break;
  case TY_CHAR:
    break;
  case TY_STRING:
    string_var_attrib( pvar_attr, pvar_name, (const char *)pinit, pos );
    break;
  case TY_LIST:
    break;
  case TY_POLY:
    break;
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return pvar_attr;
}
