#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

static TYPE_CONS_PTR infer ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, EXPR_CONS_PTR pexpr, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_expr = NULL;
  assert( penv );
  assert( pexpr );
  switch( pexpr->mnemonic ) {
  case MNC_CALL:
    break;
  case MNC_ASGN:
    {
      TYPE_SUBST_PTR pS_l = NULL;
      TYPE_CONS_PTR pty_l = NULL;
      TYPE_SUBST_PTR pS_r = NULL;
      TYPE_CONS_PTR pty_r = NULL;
      pty_l = infer( &pS_l, penv, pexpr->kids.pleft, pos );
      {
	TYPE_ENV_PTR penv_r = NULL;
	penv_r = dup_env( penv, pos );
	if( penv_r ) {
	  pty_r = infer( &pS_r, penv_r, pexpr->kids.pright, pos );
	} else
	  ;
      }
      ;
    }
    break;
  case MNC_ARITH:
    break;
  case MNC_LVALUE:
    break;
  case MNC_RVALUE:
    break;
  case END_OF_MNEMONIC_CODE:
    /* fall thru. */
  default:
    break;
  }
  return pty_expr;
}

TYPE_CONS_PTR typematch ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, STATEMENT_PTR pstmt, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_stmt = NULL;
  assert( penv );
  assert( pstmt );
  switch( pstmt->sort ) {
  case STMT_DECL:
    assert( pstmt->u.pdecl );
    assert( (pstmt->u.pdecl)->pinit );
    infer( NULL, penv, (pstmt->u.pdecl)->pinit, pos );
    break;
  case STMT_EXPR:
    infer( NULL, penv, pstmt->u.pexpr, pos );
    break;
  case END_OF_STMT_SORT:
    /* fall thru. */
  default:
    assert( FALSE );    
  }
  return pty_stmt;
}

BOOL typecheck ( TYPE_CONS_PTR_C pty1, TYPE_CONS_PTR_C pty2 ) {
  BOOL r = FALSE;
  
  assert( pty1 );
  assert( pty2 );
  if( pty1->type.ty == TY_LIST ) {
    if( pty2->type.ty == TY_LIST ) {
      assert( pty1->attrs.list.pty_elem );
      assert( pty2->attrs.list.pty_elem );
      r = typecheck( pty1->attrs.list.pty_elem, pty2->attrs.list.pty_elem );
    }
  } else {
    assert( pty1->type.ty != TY_LIST );
    switch( pty1->type.ty ) {
    case TY_INT:
    case TY_STRING:
      r = (pty1->type.ty == pty2->type.ty);
      break;
    default:
      assert( FALSE );
    }
  }
  return r;
}
