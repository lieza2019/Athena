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
