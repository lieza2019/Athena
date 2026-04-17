#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

STATEMENTS statements = { 0, {} };

STATEMENT_PTR new_stmt ( void ) {
  STATEMENT_PTR pr = NULL;
  if( statements.nstmts < MAX_STATEMENTS ) {
    pr = &statements.stmts_buf[statements.nstmts];
    statements.nstmts++;
  }
  return pr;
}

BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_ATTRIB_PTR pvar_attr ) {
  BOOL redef = FALSE;  
  assert( ppstmt );
  assert( pvar_attr );
  
  *ppstmt = NULL;
  *ppstmt = new_stmt();
  if( *ppstmt ) {
    DECLARATION_PTR pdecl = NULL;
    redef = decl_var( &pdecl, pvar_attr );
    assert( pdecl );
    if( redef )
      err_redef( pdecl );
    (*ppstmt)->pos = pdecl->u.variable.var.pos;
    (*ppstmt)->sort = STMT_DECL;
    (*ppstmt)->u.pdecl = pdecl;
  } else
    ath_abort( pvar_attr->pos, ABORT_MEMLACK );
  return redef;
}
