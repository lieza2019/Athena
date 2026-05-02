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
      assert( strcmp( ps->ident, pvar_attr->ident ) == 0 );
      if( ps == psym ) {
	assert( ps->entity.kind == SYM_DECL );
	assert( ps->entity.u.decl.kind == DECL_VAR );
	
#if 1 /* assuming the specific case of "int a = n", in below code. */
	if( pvar_attr->type == TY_INT ) {
	  assert( pvar_attr->u.var_int.init_n );
	  SRC_POS pos_ini = pvar_attr->u.var_int.init_n->pos;
	  EXPR_CONS_PTR pasgn = alloc_expr_cons( pos_ini );
	  pasgn->pos = pvar_attr->pos;
	  pasgn->mnemonic = MNC_ASGN;
	  if( pasgn ) {
	    EXPR_CONS_PTR pl = alloc_expr_cons( pos_ini );
	    EXPR_CONS_PTR pr = alloc_expr_cons( pos_ini );
	    if( pl && pr ) {
	      pl->pos = pvar_attr->pos;
	      pl->mnemonic = MNC_LVALUE;
	      pl->kids.pdaugh = pvar_attr;	  
	      pr->pos = pos_ini;
	      pr->mnemonic = MNC_RVALUE;
	      assert( pvar_attr->u.var_int.init_n );
	      pr->kids.pdaugh = pvar_attr->u.var_int.init_n;
	      pasgn->kids.pleft = pl;
	      pasgn->kids.pright = pr;
	      ps->entity.u.decl.u.variable.pinival = pasgn;
	    } else
	      goto failed_memalloc;
	  } else
	  failed_memalloc:
	    ath_abort( pos_ini, ABORT_MEMLACK );
	}
#endif
	
      } else {
	redef = TRUE;
	/* and release unused psym, here. */
      }
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
