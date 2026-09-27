#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"
#include "y.tab.h"

STATEMENT_PTR expr2_stmt ( EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  STATEMENT_PTR pstmt = NULL;
  assert( pexpr );
  stmt_expr( &pstmt, pexpr, pos );
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
    pattr->pinit = pvar_attr->pinit;
    stmt_decl_var( &pstmt, pattr, pos );
    assert( pstmt );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  tychk_decl_var( pstmt, pos );
  return pstmt;
}
