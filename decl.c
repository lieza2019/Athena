#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

void err_redef ( DECL_ATTRIB_PTR pdecl_attr ) {
  int row;
  int col;
  assert( pdecl_attr );
  row = pdecl_attr->u.var.pos.row;
  col = pdecl_attr->u.var.pos.col;
  assert( row > 1 );
  assert( col > 1 );
  printf( "(%d, %d): symbol %s redefinition previous at (%d, %d).\n", row, col, pdecl_attr->ident, pdecl_attr->u.var.pos.row, pdecl_attr->u.var.pos.col );  
}

BOOL decl_var ( DECL_ATTRIB_PTR *ppdecl_attr, VAR_DECL_PTR pvar_attr ) {
  SYM_ENTITY_PTR pentry =  NULL;
  BOOL redef = FALSE;
  assert( ppdecl_attr );
  assert( pvar_attr );
  
  *ppdecl_attr = NULL;
  pentry = find_crnt_scope( symtbl.pcrnt_scope, pvar_attr->ident );
  if( pentry ) {
    *ppdecl_attr = &pentry->u.decl;
    redef = TRUE;
  } else {
    SYMTBL_ENTRY_PTR psym = NULL;
    psym = new_memarea( sizeof(SYMTBL_ENTRY) );
    if( psym ) {
      psym->ident = pvar_attr->ident;
      psym->entity.kind = SYM_DECL;
      psym->entity.u.decl.ident = pvar_attr->ident;
      psym->entity.u.decl.kind = DECL_VAR;
      psym->entity.u.decl.u.var = *pvar_attr;
      psym->passoc = NULL;
      psym->pnext = NULL;
      {
	SYMTBL_ENTRY_PTR ps = NULL;
	ps = reg_symbol( psym );
	assert( ps );
	assert( strcmp( ps->ident, pvar_attr->ident) == 0 );
	assert( ps->entity.kind == SYM_DECL );
	assert( ps->entity.u.decl.kind == DECL_VAR );
	*ppdecl_attr = &ps->entity.u.decl;
      }
    } else
      ath_abort( ABORT_MEMLACK, pvar_attr->pos );
  }
  if( *ppdecl_attr )
    assert( strcmp( (*ppdecl_attr)->ident, pvar_attr->ident ) == 0 );
  return redef;
}
