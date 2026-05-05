#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos ) {
  LIST_CELL_PTR pcell = NULL;
  pcell = alloc_type_cons( pos );
  return pcell;
}

void free_list_cell ( LIST_CELL_PTR pcell ) {
  free_type_cons( pcell );
}

void destroy_list ( LIST_CELL_PTR plist ) {
  assert( plist );
  free_list_cell( plist );
}

TYPE_CONS_PTR list_dup ( TYPE_CONS_PTR *ppdup, TYPE_CONS_PTR porg, SRC_POS_C pos ) {
  assert( ppdup );
  assert( porg );
  *ppdup = alloc_list_cell( pos );
  if( *ppdup ) {
    (*ppdup)->pos = pos;
    (*ppdup)->type = porg->type;
    if( (*ppdup)->type.ty == TY_LIST ) {
      list_dup( &(*ppdup)->attrs.list.pty_elem, porg->attrs.list.pty_elem, pos );
      if( (*ppdup)->attrs.list.pty_elem ) {
	TYPE_CONS_PTR pcrnt = *ppdup;
	if( porg->attrs.list.car ) {
	  TYPE_CONS_PTR pnext = porg->attrs.list.cdr;
	  list_dup( &pcrnt->attrs.list.car, porg->attrs.list.car, pos );
	  if( pcrnt->attrs.list.car ) {
	    while( pnext ) {
	      list_dup( &pcrnt->attrs.list.cdr, pnext, pos );
	      if( pcrnt->attrs.list.cdr ) {
		pcrnt = pcrnt->attrs.list.cdr;
		pcrnt->attrs.list.cdr = NULL;
		pnext = pnext->attrs.list.cdr;
	      } else
		ath_abort( pos, ABORT_MEMLACK );
	    }
	    (*ppdup)->attrs.list.plast = pcrnt;
	  } else
	    ath_abort( pos, ABORT_MEMLACK );
	}
      } else
	ath_abort( pos, ABORT_MEMLACK );
#ifdef RUNTIME_CONSITENCY_CHECK
      assert( ppdup );
      assert( *ppdup );
      if( (*ppdup)->attrs.list.car ) {
	TYPE_CONS_PTR *ppcell = ppdup;
	while( *ppcell ) {
	  if( ! (*ppcell)->attrs.list.cdr )
	    break;
	  ppcell = &(*ppcell)->attrs.list.cdr;
	}
	assert( (*ppdup)->attrs.list.plast == *ppcell );
      } else {
	assert( (*ppdup)->attrs.list.cdr == NULL );
	assert( (*ppdup)->attrs.list.plast == NULL );
      }
#endif // RUNTIME_CONSITENCY_CHECK
    } else {
      assert( (*ppdup)->type.ty != TY_LIST );
      switch( (*ppdup)->type.ty ) {
      case TY_INT:
	(*ppdup)->attrs.literal.integer.n = porg->attrs.literal.integer.n;
	break;
      case TY_CHAR:
	(*ppdup)->attrs.literal.character.c = porg->attrs.literal.character.c;
	break;
      case TY_STRING:
	(*ppdup)->attrs.literal.string.ps = porg->attrs.literal.string.ps;
	break;
      default:
	assert( FALSE );
      }
    }
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return *ppdup;
}

BOOL list_is_nil ( TYPE_CONS_PTR_C pcons_list ) {
  BOOL r = FALSE;
  
  assert( pcons_list );
  if( pcons_list->type.ty == TY_LIST ) {
    r = (pcons_list->attrs.list.car == NULL);
    assert( r ? ((! pcons_list->attrs.list.cdr) && (! pcons_list->attrs.list.plast)) : (pcons_list->attrs.list.plast != NULL) );
  }
  return r;
}

LIST_CELL_PTR list_creat_nil( TYPE_CONS_PTR pty, SRC_POS_C pos ) {
  TYPE_CONS_PTR pl_nil = NULL;
  assert( pty );
  pl_nil = alloc_list_cell( pos );
  if( pl_nil ) {
    pl_nil->pos = pos;
    pl_nil->type.ty = TY_LIST;
    pl_nil->attrs.list.pty_elem = pty;
    assert( pl_nil->attrs.list.car == NULL );
    assert( pl_nil->attrs.list.cdr == NULL );
    assert( pl_nil->attrs.list.plast == NULL );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return pl_nil;
}

LIST_CELL_PTR cons_list ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  
  assert( plist );
  assert( plist->type.ty == TY_LIST );
  assert( pcons_ty );
  if( typecheck( (TYPE_CONS_PTR)plist->attrs.list.pty_elem, pcons_ty ) ) {
    LIST_CELL_PTR pcons_cell = NULL;
    if( list_is_nil( plist ) )
      pcons_cell = plist;
    else
      pcons_cell = alloc_list_cell( pos );
    if( pcons_cell ) {
      pcons_cell->pos = pos;
      pcons_cell->type.ty = TY_LIST;
      pcons_cell->attrs.list.pty_elem = plist->attrs.list.pty_elem;
      pcons_cell->attrs.list.car = pcons_ty;
      if( pcons_cell != plist ) {
	pcons_cell->attrs.list.cdr = plist;
	pcons_cell->attrs.list.plast = plist->attrs.list.plast;
      } else {
	pcons_cell->attrs.list.cdr = NULL;
	pcons_cell->attrs.list.plast = plist;
      }
      r = pcons_cell;
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  return r;
}
