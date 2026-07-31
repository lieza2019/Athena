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

static char *print_value_type ( char *sbuf, EXPR_CONS_PTR_C pval ) {
  //SRC_POS pos;
  char *ps = NULL;
  assert( sbuf );
  assert( pval );
  assert( pval->ptype );
  
  //pos = pval->pos;
  ps = sbuf;
  switch( (pval->ptype)->type.ty ) {
  case TY_INT:
    sprintf( ps, "%d", pval->kids.body.literal.integer.n );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps, ":" );
    ps++;
    ps = print_type( ps, pval->ptype );
    assert( *ps == 0 );
    break;
  case TY_CHAR:
    sprintf( ps, "%d", pval->kids.body.literal.character.c );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps, ":" );
    ps++;
    ps = print_type( ps, pval->ptype );
    assert( *ps == 0 );
    break;
  case TY_STRING:
    strcpy( ps,  "\"" );
    ps++;
    if( pval->kids.body.literal.string.s ) {
      strcpy( ps, pval->kids.body.literal.string.s );
      ps += strlen( ps );
    }
    assert( *ps == 0 );
    strcpy( ps,  "\"" );
    ps++;
    strcpy( ps, ":" );
    ps++;
    ps = print_type( ps, pval->ptype );
    assert( *ps == 0 );
    break;
  case TY_LIST:
    if( pval->kids.body.list.car ) {
      EXPR_CONS_PTR_C pcell = pval;
      strcpy( ps, "[" );
      ps++;
      do {
	assert( pcell->ptype );
	if( pcell != pval ) {
	  sprintf( ps, "%s", ", " );
	  ps += strlen( ps );
	  assert( *ps == 0 );
	}
	assert( (pcell->ptype)->type.ty == TY_LIST );
	ps = print_value_type( ps, pcell->kids.body.list.car );
	assert( *ps == 0 );
	pcell = pcell->kids.body.list.cdr;
      } while( pcell );
      strcpy( ps, "]" );
      ps++;
    } else {
      assert( ! pval->kids.body.list.cdr );
      strcpy( ps, "[]" );
      ps += 2;
    }
    assert( *ps == 0 );    
    strcpy( ps, ":" );
    ps++;
    strcpy( ps, "[" );
    ps++;
    ps = print_type( ps, (pval->ptype)->attrs.list.pty_elem );
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
    ps = print_type( ps, pval->ptype );
    assert( *ps == 0 );
    break;
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ps;
}

char *show_var_decl ( char *sbuf, VAR_ATTRIB_PTR pvar_attr ) {
  //SRC_POS pos;
  char *ps = NULL;
  assert( sbuf );
  assert( pvar_attr );
  assert( pvar_attr->ptype );
  
  //pos = pvar_attr->pos;
  ps = sbuf;
  strcpy( ps, pvar_attr->ident );
  ps += strlen( ps );
  assert( *ps == 0 );
  strcpy( ps, " := " );
  ps += strlen( ps );
  switch( (pvar_attr->ptype)->type.ty ) {
  case TY_INT:
    if( pvar_attr->pinit ) {
#if 0 // *****
      assert( (pvar_attr->pinit)->ptype );
      assert( ((pvar_attr->pinit)->ptype)->type.ty == TY_INT );
      sprintf( ps, "%d", (pvar_attr->pinit)->kids.body.literal.integer.n );
#else
      EXPR_CONS_PTR pini = pvar_attr->pinit;
      assert( (pini->mnemonic == MNC_RVALUE) || (pini->mnemonic == MNC_CONST) );
      ;
#endif
    } else
      strcpy( ps, "NO_DECL_INIVAL" );
    ps += strlen( ps );
    assert( *ps == 0 );
    strcpy( ps, ":int" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_STRING:
    if( pvar_attr->pinit ) {
      assert( (pvar_attr->pinit)->ptype );
      assert( ((pvar_attr->pinit)->ptype)->type.ty == TY_STRING );
      strcpy( ps,  "\"" );
      ps++;
      if( (pvar_attr->pinit)->kids.body.literal.string.s ) {
	sprintf( ps, "%s", (pvar_attr->pinit)->kids.body.literal.string.s );
	ps += strlen( ps );
      }
      assert( *ps == 0 );
      strcpy( ps,  "\"" );
      ps++;
    } else {
      strcpy( ps, "NO_DECL_INIVAL" );
      ps += strlen( ps );
      assert( *ps == 0 );
    }
    strcpy( ps, ":string" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_LIST:
    if( pvar_attr->pinit ) {
      assert( (pvar_attr->pinit)->ptype );
      assert( ((pvar_attr->pinit)->ptype)->type.ty == TY_LIST );
      ps = print_value_type( ps, pvar_attr->pinit );
      ps += strlen( ps );
    } else {
      strcpy( ps, "NO_DECL_INIVAL:" );
      ps += strlen( ps );
      assert( *ps == 0 );
      ps = print_type( ps, pvar_attr->ptype );
    }
    assert( *ps == 0 );
    break;
  case TY_POLY:
    strcpy( ps, "UNKNOWN_VALUE:poly" );
    ps += strlen( ps );
    assert( *ps == 0 );
    break;
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return ps;
}
