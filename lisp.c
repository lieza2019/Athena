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
      ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
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

BOOL list_is_nil ( TYPE_CONS_PTR_C pcons_list ) {
  BOOL r = FALSE;
  
  assert( pcons_list );
  if( pcons_list->type == TY_LIST ) {
    r = !(pcons_list->u.list.car) && !(pcons_list->u.list.cdr);
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

LIST_CELL_PTR cons_list ( LIST_CELL_PTR plist, TYPE_CODE cons_ty, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  LIST_CELL_PTR pcons_elem = NULL;
  
  assert( plist );
  assert( plist->type == TY_LIST );
  pcons_elem = alloc_list_cell( pos );
  if( pcons_elem ) {
    pcons_elem->pos = pos;
    pcons_elem->type = cons_ty;
    if( plist->u.list.car ) {
      LIST_CELL_PTR pcons_node = NULL;
      assert( plist->u.list.plast );
      pcons_node = alloc_list_cell( pos );
      if( pcons_node ) {
	pcons_node->pos = pos;
	pcons_node->type = TY_LIST;
	pcons_node->u.list.car = pcons_elem;
	pcons_node->u.list.cdr = plist;
	pcons_node->u.list.plast = plist->u.list.plast;
	r = pcons_node;
      } else
	goto err_creat_objs;
    } else {
      assert( ! plist->u.list.cdr );
      assert( ! plist->u.list.plast );
      plist->u.list.car = pcons_elem;
      plist->u.list.plast = plist;
      r = plist;
    }
  } else
  err_creat_objs:
    ath_abort( pos, ABORT_CANNOT_CREAT_OBJ );
  return r;
}

BOOL typecheck ( TYPE_CONS_PTR_C pty1, TYPE_CONS_PTR_C pty2 ) {
  BOOL r = FALSE;
  
  assert( pty1 );
  assert( pty2 );
  if( pty1->type == TY_LIST ) {
    if( pty2->type == TY_LIST ) {
      if( list_is_nil( pty2 ) )
	r = TRUE;
      else {
	assert( pty2->u.list.car );
	assert( pty2->u.list.plast );
	if( ! list_is_nil( pty1 ) ) {
	  assert( pty1->u.list.car );
	  assert( pty1->u.list.plast );
	  r = typecheck( pty1->u.list.car, pty2->u.list.car );
	}
      }
    }
  } else {
    assert( pty1->type != TY_LIST );
    switch( pty1->type ) {
    case TY_INT: case TY_STRING:
      r = (pty1->type == pty2->type);
      break;
    default:
      assert( FALSE );
    }
  }
  return r;
}

LIST_CELL_PTR cons_list1 ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  LIST_CELL_PTR pcons_elem = NULL;
  
  assert( plist );
  assert( plist->type == TY_LIST );
  assert( pcons_ty );
  pcons_elem = alloc_list_cell( pos );
  if( pcons_elem ) {
    pcons_elem->pos = pos;
#if 0
    pcons_elem->type = cons_ty;
#endif
    if( list_is_nil( plist ) ) {
      assert( ! plist->u.list.car );
      assert( ! plist->u.list.cdr );
      assert( ! plist->u.list.plast );
      plist->u.list.car = pcons_elem;
      plist->u.list.plast = plist;
      r = plist;
    } else {      
      assert( plist->u.list.car );
      assert( plist->u.list.plast );
      if( typecheck( (TYPE_CONS_PTR)plist->u.list.car, pcons_ty ) ) {
	LIST_CELL_PTR pcons_node = NULL;
	pcons_node = alloc_list_cell( pos );
	if( pcons_node ) {
	  pcons_node->pos = pos;
	  pcons_node->type = TY_LIST;
	  pcons_node->u.list.car = pcons_elem;
	  pcons_node->u.list.cdr = plist;
	  pcons_node->u.list.plast = plist->u.list.plast;
	  r = pcons_node;
	} else
	  goto err_creat_objs;
      } else {
	ath_printf( pos, "type mismatched on list concatination, ignored." );
      }	
    }
  } else
  err_creat_objs:
    ath_abort( pos, ABORT_CANNOT_CREAT_OBJ );
  return r;
}

LIST_CELL_PTR cons_list2 ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  
  assert( plist );
  assert( plist->type == TY_LIST );
  assert( pcons_ty );
  pcons_ty->pos = pos;
  if( list_is_nil( plist ) ) {
    assert( ! plist->u.list.car );
    assert( ! plist->u.list.cdr );
    assert( ! plist->u.list.plast );
    plist->u.list.car = pcons_ty;
    plist->u.list.plast = plist;
    r = plist;
  } else {
    assert( plist->u.list.car );
    assert( plist->u.list.plast );
    if( typecheck( (TYPE_CONS_PTR)plist->u.list.car, pcons_ty ) ) {
      LIST_CELL_PTR pcons_node = NULL;
      pcons_node = alloc_list_cell( pos );
      if( pcons_node ) {
	pcons_node->pos = pos;
	pcons_node->type = TY_LIST;
	pcons_node->u.list.car = pcons_ty;
	pcons_node->u.list.cdr = plist;
	pcons_node->u.list.plast = plist->u.list.plast;
	r = pcons_node;
      } else
	ath_abort( pos, ABORT_CANNOT_CREAT_OBJ );
    } else
      ath_printf( pos, "type mismatched on list concatination, ignored." );
  }
  return r;
}
