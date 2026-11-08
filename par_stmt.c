#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"
#include "y.tab.h"

#if 0 // *****
STATEMENT_PTR expr2_stmt ( EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  STATEMENT_PTR pstmt = NULL;
  assert( pexpr );
  stmt_expr( &pstmt, pexpr, pos );
  return pstmt;
}
#else
STATEMENT_PTR expr2_stmt ( EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  STATEMENT_PTR pstmt = NULL;
  assert( pexpr );
  
  pstmt = alloc_stmt_attr( pos );
  if( pstmt ) {
    pstmt->pos = pos;
    pstmt->sort = STMT_EXPR;
    pstmt->u.pexpr = pexpr;
    if( (pstmt->u.pexpr)->mnemonic != MNC_ASGN )
      printf( "(%d, %d): statement has no effect.\n", pstmt->pos.row, pstmt->pos.col );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return pstmt;
}
#endif

STATEMENT_PTR stmt_return ( EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  STATEMENT_PTR pstmt = NULL;
  assert( pexpr );
  
  pstmt = alloc_stmt_attr( pos );
  if( pstmt ) {
    pstmt->pos = pos;
    pstmt->sort = STMT_RETURN;
    pstmt->u.pexpr = pexpr;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return pstmt;
}

STATEMENT_PTR vardecl2_stmt ( VAR_ATTRIB_PTR pvar_attr, SRC_POS_C pos ) {
  STATEMENT_PTR pstmt = NULL;
  VAR_ATTRIB_PTR pattr = NULL;
  assert( pvar_attr );
  
  pattr = alloc_var_attr( pos );
  if( pattr ) {
    pattr->pos = pos;
    pattr->ident = pvar_attr->ident;
    pattr->ptype = pvar_attr->ptype;
    pattr->opts.var_decl.pinitializer = pvar_attr->opts.var_decl.pinitializer;
    stmt_decl_var( &pstmt, pattr, pos );
    assert( pstmt );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  tychk_decl_var( pstmt, pos );
  return pstmt;
}
