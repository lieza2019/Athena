#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ath_mem.h"

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
  }
  return r;
}
