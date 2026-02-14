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

BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_DECL_PTR pvar_decl, SRC_POS_C pos ) {
  BOOL redef = FALSE;  
  assert( ppstmt );
  assert( pvar_decl );
  
  *ppstmt = NULL;
  *ppstmt = new_stmt();
  if( *ppstmt ) {
    DECL_ATTRIB_PTR pattr = NULL;
    redef = decl_var( &pattr, pvar_decl, pos );
    assert( pattr );
    if( redef )
      err_redef( pattr, pos );
    (*ppstmt)->pos = pattr->u.var.pos;
    (*ppstmt)->kind = STMT_DECL;
    (*ppstmt)->u.pdecl = pattr;
  } else
    ath_abort( ABORT_MEMLACK, pos );
  return redef;
}
