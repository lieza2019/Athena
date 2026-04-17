#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

BOOL decl_var ( DECLARATION_PTR *pdecl, VAR_ATTRIB_PTR pvar_attr ) {
  BOOL redef = FALSE;
  SYMTBL_ENTRY_PTR psym = NULL;
  assert( pdecl );
  assert( pvar_attr );
  
  *pdecl = NULL;
  psym = new_memarea( sizeof(SYMTBL_ENTRY) );
  if( psym ) {
    psym->ident = pvar_attr->ident;
    psym->entity.kind = SYM_DECL;
    psym->entity.u.decl.ident = pvar_attr->ident;
    psym->entity.u.decl.kind = DECL_VAR;
    psym->entity.u.decl.u.variable.var = *pvar_attr;
    psym->passoc = NULL;
    psym->pnext = NULL;
    {
      SYMTBL_ENTRY_PTR ps = NULL;
      ps = reg_symbol( psym );
      assert( ps );
      assert( strcmp( ps->ident, pvar_attr->ident) == 0 );
      if( ps == psym ) {
	assert( ps->entity.kind == SYM_DECL );
	assert( ps->entity.u.decl.kind == DECL_VAR );	
      } else
	redef = TRUE;
      *pdecl = &ps->entity.u.decl;
    }
  } else
    ath_abort( pvar_attr->pos, ABORT_MEMLACK );
  if( *pdecl )
    assert( strcmp( (*pdecl)->ident, pvar_attr->ident ) == 0 );
  return redef;
}

void err_redef ( DECLARATION_PTR pdecl ) {
  int row;
  int col;
  assert( pdecl );
  row = pdecl->u.variable.var.pos.row;
  col = pdecl->u.variable.var.pos.col;
  assert( row > 1 );
  assert( col > 1 );
  printf( "(%d, %d): symbol %s redefinition previous at (%d, %d).\n", row, col, pdecl->ident, pdecl->u.variable.var.pos.row, pdecl->u.variable.var.pos.col );  
}
