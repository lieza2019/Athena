/* purged, 2026/9/12 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

STATEMENTS statements = { 0, {} };
TYPE_ENV_PTR penv_last = NULL;

STATEMENT_PTR new_stmt ( void ) {
  STATEMENT_PTR pr = NULL;
  if( statements.nstmts < MAX_STATEMENTS ) {
    pr = &statements.stmts_buf[statements.nstmts];
    statements.nstmts++;
  }
  return pr;
}

BOOL stmt_expr ( STATEMENT_PTR *ppstmt, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  BOOL r = FALSE;
  assert( ppstmt );
  assert( pexpr );
  
  *ppstmt = NULL;
  *ppstmt = new_stmt();
  if( *ppstmt ) {
    (*ppstmt)->pos = pos;
    (*ppstmt)->sort = STMT_EXPR;
    (*ppstmt)->u.pexpr = pexpr;
    r = TRUE;
    if( !(pexpr->mnemonic == MNC_ASGN) ) {
      printf( "(%d, %d): statement has no effect.\n", pos.row, pos.col );
      r = FALSE;
    }
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}

BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_ATTRIB_PTR pvar_attr, SRC_POS_C pos ) {
  BOOL redef = FALSE;
  assert( ppstmt );
  assert( pvar_attr );
  
  *ppstmt = NULL;
  *ppstmt = new_stmt();
  if( *ppstmt ) {
    DECLARATION_PTR pdecl = NULL;
    redef = decl_var( &pdecl, pvar_attr, pos );
    assert( pdecl );
    if( redef )
      err_redef( pdecl, pvar_attr->pos );
    (*ppstmt)->pos = pos;
    (*ppstmt)->sort = STMT_DECL;
    (*ppstmt)->u.pdecl = pdecl;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return redef;
}
