#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

void err_redef ( DECLARATION_PTR pdecl ) {
  int row;
  int col;
  assert( pdecl );
  assert( pdecl->u.variable.pvar );
  
  row = (pdecl->u.variable.pvar)->pos.row;
  col = (pdecl->u.variable.pvar)->pos.col;
  assert( row > 1 );
  assert( col > 1 );
  printf( "(%d, %d): symbol %s redefinition previous at (%d, %d).\n", row, col, pdecl->ident, (pdecl->u.variable.pvar)->pos.row, (pdecl->u.variable.pvar)->pos.col );  
}

static struct {
  struct {
    VAR_ATTRIB_PTR pavail;
    VAR_ATTRIB_PTR palive;
  } var;
} decl_attr_manage;
VAR_ATTRIB_PTR alloc_var_attr ( SRC_POS_C pos ) {
  VAR_ATTRIB_PTR pvattr = NULL;
  
  pvattr = (VAR_ATTRIB_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&decl_attr_manage.var.pavail, (ALLOC_NODE_LINKS_PTR *)&decl_attr_manage.var.palive,
				       sizeof(VAR_ATTRIB), NUM_DECLATTR_VAR_PAR_ALLOC, pos );
  return pvattr;
}

void free_var_addr ( VAR_ATTRIB_PTR pvattr ) {
  if( pvattr ) {
    free_node ( (ALLOC_NODE_LINKS_PTR *)&decl_attr_manage.var.pavail, (ALLOC_NODE_LINKS_PTR *)&decl_attr_manage.var.palive, (ALLOC_NODE_LINKS_PTR)pvattr );
  }
}

BOOL decl_var ( DECLARATION_PTR *pdecl, VAR_ATTRIB_PTR pvar_attr ) {
  BOOL redef = FALSE;
  SYMTBL_ENTRY_PTR psym = NULL;
  assert( pdecl );
  assert( pvar_attr );
  assert( pvar_attr->ptype );
  
  *pdecl = NULL;
  psym = new_memarea( sizeof(SYMTBL_ENTRY) );
  if( psym ) {
    psym->ident = pvar_attr->ident;
    psym->entity.kind = SYM_DECL;
    psym->entity.u.decl.pos = pvar_attr->pos;
    psym->entity.u.decl.ident = pvar_attr->ident;
    psym->entity.u.decl.kind = DECL_VAR;
    psym->entity.u.decl.u.variable.pvar = pvar_attr;
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
	if( (pvar_attr->ptype)->type.ty == TY_INT ) {
	  SRC_POS pos_ini = (pvar_attr->ptype)->pos;
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
	      pr->mnemonic = MNC_VAL;
	      pr->kids.pdaugh = pvar_attr->ptype;
	      pasgn->kids.pleft = pl;
	      pasgn->kids.pright = pr;
	      ps->entity.u.decl.pinit = pasgn;
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
