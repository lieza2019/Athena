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
  ALLOC_NODE_LINKS_PTR r = NULL;
  assert( ppavail );
  assert( ppalive );
  assert( nnodes_alloc > 0 );
  
  if( ! *ppavail ) {
    *ppavail = new_memarea( node_size * nnodes_alloc );
    if( *ppavail ) {
      ALLOC_NODE_LINKS_PTR pc = *ppavail;
      while( pc < (ALLOC_NODE_LINKS_PTR)((unsigned char *)*ppavail + (node_size * (nnodes_alloc - 1))) ) {
	pc->pprev = NULL;
	pc->pnext = (ALLOC_NODE_LINKS_PTR)((unsigned char *)pc + node_size);
	pc = (ALLOC_NODE_LINKS_PTR)((unsigned char*)pc + node_size);
	assert( !pc->pnext );
      }
    } else
      ath_abort( pos, ABORT_MEMLACK );
  }
  assert( *ppavail );
  r = *ppavail;
  *ppavail = r->pnext;
  bzero( (unsigned char *)r, node_size );
  if( *ppalive ) {
    (*ppalive)->pprev = r;
    r->pnext = *ppalive;
  }
  *ppalive = r;
  return r;
}

void free_node ( ALLOC_NODE_LINKS_PTR *ppavail, ALLOC_NODE_LINKS_PTR *ppalive, ALLOC_NODE_LINKS_PTR pnode_freed ) {
  ALLOC_NODE_LINKS_PTR pp = NULL;
  assert( ppavail );
  assert( ppalive );
  assert( pnode_freed );
  
  pp = pnode_freed->pprev;
  if( pp ) {
    ALLOC_NODE_LINKS_PTR pn = pnode_freed->pnext;
    assert( pnode_freed != *ppalive );
    pp->pnext = pn;
    if( pn )
      pn->pprev = pp;
    pnode_freed->pprev = NULL;
  } else {
    assert( pnode_freed == *ppalive );
    *ppalive = pnode_freed->pnext;
    if( *ppalive )
      (*ppalive)->pprev = NULL;
  }
  assert( ! pnode_freed->pprev );
  pnode_freed->pnext = *ppavail;
  *ppavail = pnode_freed;
}
