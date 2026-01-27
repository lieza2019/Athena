#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

SYMTBL symtbl = { NULL, NULL, NULL };

static SYMTBL_ENTRY_PTR new_entry ( char *pname ) {
  SYMTBL_ENTRY_PTR pr = NULL;
  assert( pname );
  pr = new_memarea( sizeof(SYMTBL_ENTRY) );
  if( pr ) {
#if 0
    const int len = strlen(pname);
    char *ps = NULL;
    ps = new_memarea( len + 1 );
    ps = strncpy( ps, pname, len );
    if( ps ) {
      ps[len] = 0;
      pr->ident = ps;
      pr->passoc = NULL;
      pr->pnext = NULL;
    } else
      pr = NULL;
#else
    pr->ident = pname;
    pr->passoc = NULL;
    pr->pnext = NULL;
#endif
  }
  return pr;
}
static SYMTBL_SCOPE_PTR new_scope ( void ) {
  SYMTBL_SCOPE_PTR pr = NULL;
  pr = new_memarea( sizeof(SYMTBL_SCOPE) );
  if( pr ) {
    pr->pparent = NULL;
    pr->decend = NULL;
    pr->attic.past = NULL;
    pr->attic.pkids = NULL;
  }
  return pr;
}

SYMTBL_SCOPE_PTR enter_scope ( void ) {
  SYMTBL_SCOPE_PTR pr = NULL;
  if( symtbl.pcrnt_scope ) {
    SYMTBL_SCOPE_PTR pnew = NULL;
    assert( symtbl.pgnd_scope );
    pnew = new_scope();
    if( pnew ) {
      symtbl.pcrnt_scope->decend = pnew;
      pnew->pparent = symtbl.pcrnt_scope;
      pr = pnew;
    }
  } else {    
    assert( !symtbl.pgnd_scope );
    pr = new_scope();
    symtbl.pgnd_scope = pr;
  }
  if( pr )
    symtbl.pcrnt_scope = pr;
  return pr;
} 

SYMTBL_SCOPE_PTR leave_scope ( void ) {
  SYMTBL_SCOPE_PTR pr = NULL;
  assert( symtbl.pcrnt_scope );
  if( symtbl.pcrnt_scope ) {
    SYMTBL_SCOPE_PTR puplink = symtbl.pcrnt_scope->pparent;
    assert( symtbl.pgnd_scope );
    if( puplink ) {
      symtbl.pcrnt_scope->attic.pkids = puplink->attic.past;
      puplink->attic.past = symtbl.pcrnt_scope;
    }
    pr = puplink;
    symtbl.pcrnt_scope = pr;
    if( !symtbl.pcrnt_scope )
      symtbl.pgnd_scope = NULL;
  }
  return pr;
}

#define IDENT_HASHSEED_LEN 8
static int symreg_hash ( const char *ident ) {
  const int n = 5;  
  assert( (n > 0) && (n <= IDENT_HASHSEED_LEN) );
  assert( ident );
  
  int h = 0;
  {
    int i;
    for( i = 0; (i < n) && ident[i]; i++ ) {
      h = 13 * h + ident[i];
      h = (h < 0) ? ((h * -1) % SYMTBL_HASHTBL_ENTRIES) : h;
    }
  }
  return ( h % SYMTBL_HASHTBL_ENTRIES );
}

SYMTBL_ENTRY_PTR reg_symbol ( SYMTBL_ENTRY_PTR psym ) {
  int h = -1;
  assert( psym );
  assert( psym->ident );
  
  h = symreg_hash( psym->ident );
  assert( (h > -1) && (h < SYMTBL_HASHTBL_ENTRIES) );
  {
    SYMTBL_ENTRY_PTR *pbuck = NULL;
    assert( symtbl.pcrnt_scope );
    pbuck = &symtbl.pcrnt_scope->index[h];
    assert( pbuck );
    while( *pbuck )
      pbuck = &(*pbuck)->pnext;
    assert( pbuck );
    assert( ! *pbuck );
    *pbuck = psym;
    psym->pnext = NULL;
  }
  return psym;
}

SYM_ENTITY_PTR find_crnt_scope ( SYMTBL_SCOPE_PTR psco, const char *ident ) {
  SYM_ENTITY_PTR pentry = NULL;
  SYMTBL_ENTRY_PTR psym = NULL;
  int h = -1;
  assert( psco );
  assert( ident );
  
  h = symreg_hash( ident );
  assert( (h > -1) && (h < SYMTBL_HASHTBL_ENTRIES) );  
  psym = psco->index[h];
  while( psym ) {
    assert( psym );
    if( strcmp( psym->ident, ident ) == 0 )
      break;
    psym = psym->pnext;    
  }
  if( psym )
    pentry = &psym->entity;
  return pentry;
}

SYM_ENTITY_PTR find_symbol ( const char *ident ) {
  SYM_ENTITY_PTR pentry = NULL;
  SYMTBL_SCOPE_PTR psco = NULL;  
  
  psco = symtbl.pcrnt_scope;
  assert( psco );
  while( psco ) {
    assert( psco );
    pentry = find_crnt_scope( psco, ident );
    if( pentry )
      break;
    psco = psco->decend;
  }
  if( !psco )
    assert( !pentry );
  return pentry;
}

static SYMTBL_ENTRY_PTR reg_literal ( char *pname ) {
  SYMTBL_ENTRY_PTR pliter = NULL;
  int h = -1;
  assert( pname );
  
  h = symreg_hash( pname );
  assert( (h > -1) && (h < SYMTBL_HASHTBL_ENTRIES) );
  {
    SYMTBL_ENTRY_PTR *pbuck = NULL;
    if( !symtbl.pliteral )
      symtbl.pliteral = new_scope();
    assert( symtbl.pliteral );
    pbuck = &symtbl.pliteral->index[h];
    assert( pbuck );
    while( *pbuck )
      pbuck = &(*pbuck)->pnext;
    assert( pbuck );
    assert( ! *pbuck );
    pliter = new_entry( pname );
    if( pliter ) {
      pliter->entity.kind = SYM_CONST;
      pliter->entity.u.constant.kind = CONST_STR;
      pliter->entity.u.constant.pstr = pname;
      *pbuck = pliter;
      pliter->pnext = NULL;
    }
  }
  return pliter;
}

char *find_literal ( char *pname ) {
  char *pstr = NULL;
  SYMTBL_ENTRY_PTR psym = NULL;
  assert( pname );
  
  if( !symtbl.pliteral )
    goto reg;
  {
    int h = -1;
    h = symreg_hash( pname );
    assert( (h > -1) && (h < SYMTBL_HASHTBL_ENTRIES) );
    psym = symtbl.pliteral->index[h];
    while( psym ) {
      assert( psym );
      if( strcmp( psym->ident, pname ) == 0 )
	break;
      psym = psym->pnext;
    }
    if( !psym )
    reg:
      psym = reg_literal( pname );
  }
  if( psym ) {
    assert( psym->entity.kind == SYM_CONST );
    assert( psym->entity.u.constant.kind == CONST_STR );
    pstr = psym->entity.u.constant.pstr;
  }
  return pstr;
}
