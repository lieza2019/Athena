#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

#if 0
static struct {
  LIST_CELL_PTR pavail;
  LIST_CELL_PTR palive;
} cells_manage;
LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  
  if( !cells_manage.pavail ) {
    cells_manage.pavail = new_memarea( sizeof(LIST_CELL) * NUM_CELLS_PER_ALLOC );
    if( cells_manage.pavail ) {
      LIST_CELL_PTR pc = cells_manage.pavail;
      while( pc < (cells_manage.pavail + (NUM_CELLS_PER_ALLOC - 1)) ) {
	pc->alloc.pprev = NULL;
	pc->alloc.pnext = (pc + 1);
	pc++;
	assert( !pc->alloc.pnext );
      }
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  assert( cells_manage.pavail );
  r = cells_manage.pavail;
  cells_manage.pavail = r->alloc.pnext;
  bzero( r, sizeof(TYPE_CONS) );
  if( cells_manage.palive ) {
    (cells_manage.palive)->alloc.pprev = r;
    r->alloc.pnext = cells_manage.palive;
  }
  cells_manage.palive = r;
  return r;
}

void free_list_cell ( LIST_CELL_PTR pcell ) {
  LIST_CELL_PTR pp = pcell->alloc.pprev;
  assert( pcell );
  
  if( pp ) {
    LIST_CELL_PTR pn = pcell->alloc.pnext;
    assert( pcell != cells_manage.palive );    
    pp->alloc.pnext = pn;
    if( pn )
      pn->alloc.pprev = pp;
    pcell->alloc.pprev = NULL;
  } else {
    assert( pcell == cells_manage.palive );
    cells_manage.palive = pcell->alloc.pnext;
    if( cells_manage.palive )
      cells_manage.palive->alloc.pprev = NULL;
  }
  assert( !pcell->alloc.pprev );
  pcell->alloc.pnext = cells_manage.pavail;
  cells_manage.pavail = pcell;  
}
#else
static struct {
  LIST_CELL_PTR pavail;
  LIST_CELL_PTR palive;
} list_cells_manage;
LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos ) {
  LIST_CELL_PTR pcell = NULL;
  pcell = (LIST_CELL_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&list_cells_manage.pavail, (ALLOC_NODE_LINKS_PTR *)&list_cells_manage.palive, sizeof(TYPE_CONS), NUM_CELLS_PER_ALLOC, pos );
  return pcell;
}

void free_list_cell ( LIST_CELL_PTR pcell ) {
  free_node ( (ALLOC_NODE_LINKS_PTR *)&list_cells_manage.pavail, (ALLOC_NODE_LINKS_PTR *)&list_cells_manage.palive, (ALLOC_NODE_LINKS_PTR)pcell );
}
#endif

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
      list_dup( &(*ppdup)->u.list.pty_elem, porg->u.list.pty_elem, pos );
      if( (*ppdup)->u.list.pty_elem ) {
	TYPE_CONS_PTR pcrnt = *ppdup;
	if( porg->u.list.car ) {
	  TYPE_CONS_PTR pnext = porg->u.list.cdr;
	  list_dup( &pcrnt->u.list.car, porg->u.list.car, pos );
	  if( pcrnt->u.list.car ) {
	    while( pnext ) {
	      list_dup( &pcrnt->u.list.cdr, pnext, pos );
	      if( pcrnt->u.list.cdr ) {
		pcrnt = pcrnt->u.list.cdr;
		pcrnt->u.list.cdr = NULL;
		pnext = pnext->u.list.cdr;
	      } else
		ath_abort( pos, ABORT_MEMLACK );
	    }
	    (*ppdup)->u.list.plast = pcrnt;
	  } else
	    ath_abort( pos, ABORT_MEMLACK );
	}
      } else
	ath_abort( pos, ABORT_MEMLACK );
#ifdef RUNTIME_CONSITENCY_CHECK
      assert( ppdup );
      assert( *ppdup );
      if( (*ppdup)->u.list.car ) {
	TYPE_CONS_PTR *ppcell = ppdup;
	while( *ppcell ) {
	  if( ! (*ppcell)->u.list.cdr )
	    break;
	  ppcell = &(*ppcell)->u.list.cdr;	  
	}
	assert( (*ppdup)->u.list.plast == *ppcell );
      } else {
	assert( (*ppdup)->u.list.cdr == NULL );
	assert( (*ppdup)->u.list.plast == NULL );
      }
#endif // RUNTIME_CONSITENCY_CHECK
    } else {
      assert( (*ppdup)->type.ty != TY_LIST );
      switch( (*ppdup)->type.ty ) {
      case TY_INT:
	(*ppdup)->u.integer.n = porg->u.integer.n;
	break;
      case TY_CHAR:
	(*ppdup)->u.character.c = porg->u.character.c;
	break;
      case TY_STRING:
	(*ppdup)->u.string.ps = porg->u.string.ps;
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
    r = (pcons_list->u.list.car == NULL);
    assert( r ? ((! pcons_list->u.list.cdr) && (! pcons_list->u.list.plast)) : (pcons_list->u.list.plast != NULL) );
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
    pl_nil->u.list.pty_elem = pty;
    assert( pl_nil->u.list.car == NULL );
    assert( pl_nil->u.list.cdr == NULL );
    assert( pl_nil->u.list.plast == NULL );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return pl_nil;
}

LIST_CELL_PTR cons_list ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  
  assert( plist );
  assert( plist->type.ty == TY_LIST );
  assert( pcons_ty );
  if( typecheck( (TYPE_CONS_PTR)plist->u.list.pty_elem, pcons_ty ) ) {
    LIST_CELL_PTR pcons_cell = NULL;
    if( list_is_nil( plist ) )
      pcons_cell = plist;
    else
      pcons_cell = alloc_list_cell( pos );
    if( pcons_cell ) {
      pcons_cell->pos = pos;
      pcons_cell->type.ty = TY_LIST;
      pcons_cell->u.list.pty_elem = plist->u.list.pty_elem;
      pcons_cell->u.list.car = pcons_ty;
      if( pcons_cell != plist ) {
	pcons_cell->u.list.cdr = plist;
	pcons_cell->u.list.plast = plist->u.list.plast;
      } else {
	pcons_cell->u.list.cdr = NULL;
	pcons_cell->u.list.plast = plist;
      }
      r = pcons_cell;
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  return r;
}
