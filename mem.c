#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

static void *alloc_chunk ( int count, int size ) {
  void *r;
  r = calloc( count, size );
  return r;
}

static ATH_MEMCHUNK_PTR pmemavail = NULL;
void *new_memarea ( int size ) {
  void *r = NULL;
  ATH_MEMCHUNK_PTR pchunk = NULL;
  if( !pmemavail ) {  
    ATH_MEMCHUNK_PTR palloc = NULL;
  newchunk:
    palloc = alloc_chunk( INIT_ALLOC_MEMSIZ, 1 );
    if( palloc ) {      
      palloc->plim = palloc + INIT_ALLOC_MEMSIZ;
      palloc->parena = (void *)(palloc + 1);
      palloc->pwp = palloc->parena;
      palloc->pnext = NULL;
      if( !pmemavail ) {
	pmemavail = palloc;
	pchunk = pmemavail;
      } else {
	assert( pchunk );
	assert( !pchunk->pnext );
	pchunk = palloc;
      }
    }
  } else {
    ATH_MEMCHUNK_PTR pmnode = pmemavail;
    assert( pmemavail );
    do {
      assert( pmnode );
      pchunk = pmnode;
      if( size > (pmnode->plim - pmnode->pwp) )
	break;
      pmnode = pmnode->pnext;
    } while( pmnode );
    if( !pmnode ) {
      assert( pchunk );
      assert( !pchunk->pnext );
      goto newchunk;
    }
  }
  if( pchunk ) {
    r = pchunk->pwp;
    pchunk->pwp += size;
    assert( pchunk->plim > pchunk->pwp );
    bzero( r, size );
  }
  return r;
}

ALLOC_NODE_LINKS_PTR alloc_node ( ALLOC_NODE_LINKS_PTR *ppavail, ALLOC_NODE_LINKS_PTR *ppalive, const int node_size, const int nnodes_alloc, SRC_POS_C pos ) {
  ALLOC_NODE_LINKS_PTR r = NULL; // LIST_CELL_PTR r = NULL;
  assert( ppavail );
  assert( ppalive );
  assert( nnodes_alloc > 0 );
  
  if( ! *ppavail ) { // if( !list_cells_manage.pavail ) {
    *ppavail = new_memarea( node_size * nnodes_alloc ); // list_cells_manage.pavail = new_memarea( sizeof(LIST_CELL) * NUM_CELLS_PER_ALLOC );
    if( *ppavail ) { // if( list_cells_manage.pavail ) {
      ALLOC_NODE_LINKS_PTR pc = *ppavail; // LIST_CELL_PTR pc = list_cells_manage.pavail;
      while( pc < (ALLOC_NODE_LINKS_PTR)((unsigned char *)*ppavail + (node_size * (nnodes_alloc - 1))) ) { // while( pc < (list_cells_manage.pavail + (NUM_CELLS_PER_ALLOC - 1)) ) {
	pc->pprev = NULL; // pc->alloc.pprev = NULL;
	pc->pnext = (ALLOC_NODE_LINKS_PTR)((unsigned char *)pc + node_size); // tpc->alloc.pnext = (pc + 1);
	pc = (ALLOC_NODE_LINKS_PTR)((unsigned char*)pc + node_size); // pc++;
	assert( !pc->pnext );
      }
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  assert( *ppavail ); // assert( list_cells_manage.pavail );
  r = *ppavail; // r = list_cells_manage.pavail;
  *ppavail = r->pnext; // list_cells_manage.pavail = r->alloc.pnext;
  bzero( (unsigned char *)r, node_size ); // bzero( r, sizeof(TYPE_CONS) );
  if( *ppalive ) { // if( list_cells_manage.palive ) {
    (*ppalive)->pprev = r; // (list_cells_manage.palive)->alloc.pprev = r;
    r->pnext = *ppalive; // r->alloc.pnext = list_cells_manage.palive;
  }
  *ppalive = r; // list_cells_manage.palive = r;
  return r;
}

void free_node ( ALLOC_NODE_LINKS_PTR *ppavail, ALLOC_NODE_LINKS_PTR *ppalive, ALLOC_NODE_LINKS_PTR pnode_freed ) {
  ALLOC_NODE_LINKS_PTR pp = NULL;
  assert( ppavail );
  assert( ppalive );
  assert( pnode_freed ); // assert( pcell );
  
  pp = pnode_freed->pprev; // LIST_CELL_PTR pp = pcell->alloc.pprev;  
  if( pp ) {
    ALLOC_NODE_LINKS_PTR pn = pnode_freed->pnext; // LIST_CELL_PTR pn = pcell->alloc.pnext;
    assert( pnode_freed != *ppalive ); // assert( pcell != list_cells_manage.palive );
    pp->pnext = pn; // pp->alloc.pnext = pn;    
    if( pn )
      pn->pprev = pp; // pn->alloc.pprev = pp;
    pnode_freed->pprev = NULL; // pcell->alloc.pprev = NULL;
  } else {
    assert( pnode_freed == *ppalive ); // assert( pcell == list_cells_manage.palive );
    *ppalive = pnode_freed->pnext; // list_cells_manage.palive = pcell->alloc.pnext;
    if( *ppalive ) // if( list_cells_manage.palive )
      (*ppalive)->pprev = NULL; // list_cells_manage.palive->alloc.pprev = NULL;
  }
  assert( ! pnode_freed->pprev ); // assert( !pcell->alloc.pprev );
  pnode_freed->pnext = *ppavail; // pcell->alloc.pnext = list_cells_manage.pavail;
  *ppavail = pnode_freed; // list_cells_manage.pavail = pcell;
}
