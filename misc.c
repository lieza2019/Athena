#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "athena.h"

static char *ath_abort_str[] = {
  "UNASSIGNED",
  "ABORT_MEMLACK",
  "ABORT_CANNOT_CREATE_SCOPE",
  "ABORT_CANNOT_REG_SYNBOL",
  "INTERNALERR_TYPE_CONS",
  "END_OF_ATH_ABORT"
};
void ath_abort ( SRC_POS_C pos, const ATH_ABORT reason ) {
  const char *pmsg = ath_abort_str[reason];
  assert( (pos.row > 0) && (pos.col > 0) );
  assert( reason < END_OF_ATH_ABORT );
  printf( "(%d, %d): aborted for %s.\n", pos.row, pos.col, pmsg );
  exit( ABORT_CODE );
}

char *print_value_type ( char *sbuf, TYPE_CONS_PTR_C pvar_tydesc ) {
  SRC_POS pos;
  char *ps = NULL;
  assert( sbuf );
  assert( pvar_tydesc );
  
  pos = pvar_tydesc->pos;
  ps = sbuf;
  switch( pvar_tydesc->type.ty ) {
  case TY_INT:
    sprintf( ps, "%d", pvar_tydesc->attrs.literal.integer.n );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps, ":" );
    ps++;
    ps = print_var_type( ps, pvar_tydesc );
    assert( *ps == 0 );
    break;
  case TY_CHAR:
    sprintf( ps, "%d", pvar_tydesc->attrs.literal.character.c );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps, ":" );
    ps++;
    ps = print_var_type( ps, pvar_tydesc );
    assert( *ps == 0 );
    break;
  case TY_STRING:
    assert( pvar_tydesc->attrs.literal.string.ps );
    strcpy( ps,  "\"" );
    ps++;
    strcpy( ps, pvar_tydesc->attrs.literal.string.ps );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps,  "\"" );
    ps++;
    strcpy( ps, ":" );
    ps++;
    ps = print_var_type( ps, pvar_tydesc );
    assert( *ps == 0 );
    break;
  case TY_LIST:
    if( pvar_tydesc->attrs.list.car ) {
      LIST_CELL_PTR_C pcell = pvar_tydesc;
      strcpy( ps, "[" );
      ps++;
      do {
	if( pcell != pvar_tydesc ) {
	  sprintf( ps, "%s", ", " );
	  ps += strlen( ps );
	  assert( *ps == 0 );
	}
	assert( pcell->type.ty == TY_LIST );
	ps = print_value_type( ps, pcell->attrs.list.car );
	assert( *ps == 0 );
	pcell = pcell->attrs.list.cdr;
      } while( pcell );
      strcpy( ps, "]" );
      ps++;
    } else {
      assert( ! pvar_tydesc->attrs.list.cdr );
      assert( ! pvar_tydesc->attrs.list.plast );
      strcpy( ps, "[]" );
      ps += 2;
    }
    assert( *ps == 0 );    
    strcpy( ps, ":" );
    ps++;
    strcpy( ps, "[" );
    ps++;
    ps = print_var_type( ps, pvar_tydesc->attrs.list.pty_elem );
    strcpy( ps, "]" );
    ps++;
    assert( *ps == 0 );
    break;
  case TY_POLY:
    strcpy( ps, "UNKNOWN_VALUE" );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps, ":" );
    ps++;
    ps = print_var_type( ps, pvar_tydesc );
    assert( *ps == 0 );
    break;
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ps;
}

char *show_var_decl ( char *sbuf, VAR_ATTRIB_PTR pvar_attr ) {
  SRC_POS pos;
  char *ps = NULL;  
  assert( sbuf );
  assert( pvar_attr );
  
  pos = pvar_attr->pos;
  ps = sbuf;
  strcpy( ps, pvar_attr->ident );
  ps += strlen( ps );
  assert( *ps == 0 );
  strcpy( ps, " := " );
  ps += strlen( ps );
  switch( pvar_attr->type ) {
  case TY_INT:
    assert( pvar_attr->u.var_int.init_n );
    assert( (pvar_attr->u.var_int.init_n)->type.ty == TY_INT );
    sprintf( ps, "%d", (pvar_attr->u.var_int.init_n)->attrs.literal.integer.n );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps, ":int" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_STRING:
    assert( pvar_attr->u.var_str.init_s );
    assert( (pvar_attr->u.var_str.init_s)->type.ty == TY_STRING );
    assert( (pvar_attr->u.var_str.init_s)->attrs.literal.string.ps );
    strcpy( ps,  "\"" );
    ps++;    
    sprintf( ps, "%s", (pvar_attr->u.var_str.init_s)->attrs.literal.string.ps );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps,  "\"" );
    ps++;
    strcpy( ps, ":string" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_LIST:
    if( pvar_attr->u.var_list.pty )
      ps = print_value_type( ps, pvar_attr->u.var_list.init_l );
    else
      strcpy( ps, "UNKNOWN_VALUE:[UNKNOWN_TYPE]" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_POLY:
    strcpy( ps, "UNKNOWN_VALUE:poly" );
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
