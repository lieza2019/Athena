#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

TYPE_ENV_PTR penv_last = NULL;

#if 0 // *****
STATEMENTS statements = { 0, {} };
STATEMENT_PTR new_stmt ( void ) {
  STATEMENT_PTR pr = NULL;
  if( statements.nstmts < MAX_STATEMENTS ) {
    pr = &statements.stmts_buf[statements.nstmts];
    statements.nstmts++;
  }
  return pr;
}
#else
STATEMENTS statements = { NULL, NULL };
#endif

BOOL stmt_expr ( STATEMENT_PTR *ppstmt, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  BOOL r = FALSE;
  assert( ppstmt );
  assert( pexpr );
  
  *ppstmt = NULL;
#if 0 // *****
  *ppstmt = new_stmt();
#else
  *ppstmt = alloc_stmt_attr( pos );
#endif
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
#if 0 // *****
  *ppstmt = new_stmt();
#else
  *ppstmt = alloc_stmt_attr( pos );
#endif
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

static struct {
  struct {
    PROC_ATTRIB_PTR pavail;
    PROC_ATTRIB_PTR palive;
  } proc;
} stmtattr_alloc_manage;
STATEMENT_PTR alloc_stmt_attr ( SRC_POS_C pos ) {
  STATEMENT_PTR pattr = NULL;
  pattr = (STATEMENT_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&stmtattr_alloc_manage.proc.pavail,
				     (ALLOC_NODE_LINKS_PTR *)&stmtattr_alloc_manage.proc.palive,
				     sizeof(STATEMENT), NUM_PROCATTR_VAR_PAR_ALLOC, pos );
  return pattr;
}
