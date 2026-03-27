#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

#define NUM_CELLS_PER_ALLOC 256

struct {
  LIST_CELL_PTR pavail;
  LIST_CELL_PTR palive;
} cells_manage;

LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  if( !cells_manage.pavail ) {
    cells_manage.pavail = new_memarea( sizeof(LIST_CELL) * NUM_CELLS_PER_ALLOC );
    if( ! cells_manage.pavail )
      ath_abort( ABORT_CANNOT_REG_SYNBOL, pos );
    assert( cells_manage.pavail );
    {
      LIST_CELL_PTR pc = cells_manage.pavail;
      while( pc < (cells_manage.pavail + (NUM_CELLS_PER_ALLOC - 1)) ) {
	pc->pprev = NULL;
	pc->pnext = (pc + 1);	
	pc++;
	assert( !pc->pnext );
      }
    }
  }
  assert( cells_manage.pavail );
  r = cells_manage.pavail;
  cells_manage.pavail = r->pnext;  
  r->pnext = NULL;
  r->pprev = NULL;
  if( cells_manage.palive ) {
    (cells_manage.palive)->pprev = r;
    r->pnext = cells_manage.palive;
  }
  cells_manage.palive = r;
  return r;
}

void free_list_cell ( LIST_CELL_PTR pcell ) {
  assert( pcell );
  if( !pcell->pprev ) {
    assert( pcell == cells_manage.palive );
    cells_manage.palive = pcell->pnext;
    if( cells_manage.palive )
      cells_manage.palive->pprev = NULL;
  } else {
    assert( pcell != cells_manage.palive );
    LIST_CELL_PTR pn = pcell->pnext;
    LIST_CELL_PTR pp = pcell->pprev;
    assert( pp );
    pp->pnext = pn;
    if( pn )
      pn->pprev = pp;
    pcell->pprev = NULL;
  }
  assert( !pcell->pprev );
  pcell->pnext = cells_manage.pavail;
  cells_manage.pavail = pcell;  
}

BOOL list_is_nil ( TYPE_CONS_PTR pcons_list ) {
  BOOL r = FALSE;
  
  assert( pcons_list );
  if( pcons_list->type == TY_LIST ) {
    r = !(pcons_list->u.list.cdr) && !(pcons_list->u.list.car);
  }
  return r;
}

LIST_CELL_PTR creat_nil_list ( SRC_POS_C pos ) {
  LIST_CELL_PTR pl_nil = NULL;
  pl_nil = alloc_list_cell( pos );
  if( pl_nil ) {
    pl_nil->pos = pos;
    pl_nil->type = TY_LIST;
    pl_nil->u.list.car = NULL;
    pl_nil->u.list.cdr = NULL;
    pl_nil->u.list.plast = NULL;
  }
  return pl_nil;
}

LIST_CELL_PTR cons_list_elem_basetype ( LIST_CELL_PTR plist, TYPE_CODE ty, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  LIST_CELL_PTR pelem = NULL;
  
  assert( plist );
  assert( plist->type == TY_LIST );
  assert( ! plist->pnext );
  assert( ! plist->pprev );
  pelem = alloc_list_cell( pos );
  if( pelem ) {
    pelem->pos = pos;
    pelem->type = ty;    
    if( plist->u.list.car ) {
      LIST_CELL_PTR pnode = NULL;
      assert( plist->u.list.plast );
      pnode = alloc_list_cell( pos );
      if( pnode ) {
	pnode->pos = pos;
	pnode->type = TY_LIST;
	pnode->u.list.car = pelem;
	pnode->u.list.cdr = plist;
	pnode->u.list.plast = plist->u.list.plast;
	r = pnode;
      } else
	goto err_creat_objs;
    } else {
      assert( ! plist->u.list.cdr );
      assert( ! plist->u.list.plast );
      plist->u.list.car = pelem;
      plist->u.list.plast = plist;
      r = plist;
    }
  } else
  err_creat_objs:
    ath_abort( ABORT_CANNOT_CREATE_OBJ, pos );
  return r;
}
