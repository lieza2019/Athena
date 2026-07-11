#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

static void poly_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, EXPR_CONS_PTR pinit, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    TYPE_CONS_PTR pty_ply = NULL;
    pvar_attr->pos = pos;
    pvar_attr->ident = pident;
    if( !pinit ) {      
      pty_ply = alloc_type_cons( pos );
      if( pty_ply ) {
	pty_ply->pos = pos;
	pty_ply->type.ty = TY_POLY;
      } else
	ath_abort( pos, ABORT_MEMLACK );
    } else
      assert( pinit->ptype );
    pvar_attr->pinit = pinit;
    pvar_attr->ptype = (pinit ? pinit->ptype : pty_ply);
    
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void int_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, EXPR_CONS_PTR pn_init, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    pvar_attr->pos = pos;
    pvar_attr->ident = pident;
    if( !pn_init ) {
      TYPE_CONS_PTR pty_int = NULL;
      pty_int = alloc_type_cons( pos );
      if( pty_int ) {
	pty_int->pos = pos;
	pty_int->type.ty = TY_INT;
#if 0 // *****
	pty_int->attrs.literal.integer.n = 0;
#endif
      } else
	goto failed_memalloc;
      pn_init = alloc_expr_cons( pos );
      if( pn_init ) {
	pn_init->pos = pos;
	pn_init->mnemonic = MNC_CONST;
	pn_init->kids.body.literal.integer.n = 0;
	pn_init->ptype = pty_int;
      } else
      failed_memalloc:
	ath_abort( pos, ABORT_MEMLACK );
    }
    assert( pn_init );
    assert( pn_init->ptype );
    assert( (pn_init->ptype)->type.ty == TY_INT );
    pvar_attr->pinit = pn_init;
    pvar_attr->ptype = pn_init->ptype;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void string_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, EXPR_CONS_PTR ps_init, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    pvar_attr->pos = pos;
    pvar_attr->ident = pident;
    if( !ps_init ) {
      TYPE_CONS_PTR pty_str = NULL;
      char *e = NULL;
      e = new_memarea( 1 );
      if( e )
	*e = 0;
      else
	goto failed_memalloc;
      pty_str = alloc_type_cons( pos );
      if( pty_str ) {
	pty_str->pos = pos;
	pty_str->type.ty = TY_STRING;	
      } else
	goto failed_memalloc;
      ps_init = alloc_expr_cons( pos );
      if( ps_init ) {
	ps_init->pos = pos;
	ps_init->mnemonic = MNC_CONST;
	ps_init->kids.body.literal.string.s = e;
	ps_init->ptype = pty_str;
      } else
      failed_memalloc:	
	ath_abort( pos, ABORT_MEMLACK );
    }
    assert( ps_init );
    assert( ps_init->ptype );
    assert( (ps_init->ptype)->type.ty == TY_STRING );
    pvar_attr->pinit = ps_init;
    pvar_attr->ptype = ps_init->ptype;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void list_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CONS_PTR pty_list, EXPR_CONS_PTR pinit, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  assert( pty_list );
  assert( pty_list->type.ty == TY_LIST );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    pvar_attr->pos = pos;
    pvar_attr->ident = pident;
    pvar_attr->ptype = pty_list;
    if( !pinit ) {
      pinit = alloc_expr_cons( pos );
      if( pinit ) {
	pinit->pos = pos;
	pinit->mnemonic = MNC_LIST;
	pinit->kids.body.list.car = NULL;
	pinit->kids.body.list.cdr = NULL;
	pinit->kids.body.list.plast = pinit;
	pinit->ptype = pty_list;
      } else
	goto failed_memalloc;
    }
    assert( pinit );
    assert( pinit->ptype );
    assert( (pinit->ptype)->type.ty == TY_LIST );
    pvar_attr->pinit = pinit;
  } else
    failed_memalloc:
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

VAR_ATTRIB_PTR decl_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CODE var_type, TYPE_CONS_PTR type_arg, EXPR_CONS_PTR pinit, SRC_POS_C pos ) {
  assert( pvar_attr );
  assert( pvar_name );
  switch( var_type ) {
  case TY_INT:
    assert( !type_arg );
    int_var_attrib( pvar_attr, pvar_name, pinit, pos );
    break;
  case TY_CHAR:
    assert( !type_arg );
    break;
  case TY_STRING:
    assert( !type_arg );
    string_var_attrib( pvar_attr, pvar_name, pinit, pos );
    break;
  case TY_LIST:
    assert( type_arg );
    list_var_attrib( pvar_attr, pvar_name, type_arg, pinit, pos );
    break;
  case TY_POLY:
    assert( !type_arg );
    poly_var_attrib( pvar_attr, pvar_name, pinit, pos );
    break;
  case TY_GEN:
    /* fall thru. */
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  }
  return pvar_attr;
}

TYPE_CONS_PTR var_list_type ( TYPE_CONS_PTR pty_elem, TYPE_CODE elem_type, SRC_POS_C pos ) {
  TYPE_CONS_PTR pty_l = NULL;
  
  pty_l = alloc_type_cons( pos );
  if( pty_l ) {
    pty_l->pos = pos;
    pty_l->type.ty = TY_LIST;
    if( elem_type != TY_LIST ) {
      TYPE_CONS_PTR pty_desc = NULL;
      assert( !pty_elem );
      pty_desc = alloc_type_cons( pos );
      if( pty_desc ) {
	pty_desc->pos = pos;
	pty_desc->type.ty = elem_type;
	pty_elem = pty_desc;
      } else
	goto failed_memalloc;
    }
    assert( pty_elem );
    pty_l->attrs.list.pty_elem = pty_elem;
  } else
    failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pty_l;
}

EXPR_CONS_PTR value_list_elem ( TYPE_CODE elem_ty, void *pelem_val, EXPR_CONS_PTR psucc_cs, SRC_POS_C pos ) {
  EXPR_CONS_PTR pcons = NULL;
  
  pcons = alloc_expr_cons( pos );
  if( pcons ) {
    EXPR_CONS_PTR pelem = NULL;
    pcons->pos = pos;
    pcons->mnemonic = MNC_LIST;
    pelem = alloc_expr_cons( pos );
    if( pelem ) {
      TYPE_CONS_PTR pty_e = NULL;
      pelem->pos = pos;
      pelem->mnemonic = MNC_LIST;
      switch( elem_ty ) {
      case TY_INT:
	assert( pelem_val );
	pty_e = alloc_type_cons( pos );
	if( pty_e ) {
	  pty_e->pos = pos;
	  pty_e->type.ty = TY_INT;
	  pelem->ptype = pty_e;
	} else
	  goto failed_memalloc;	
	pelem->kids.body.literal.integer.n = *(int *)pelem_val;
	break;
      case TY_CHAR:
	assert( pelem_val );
	break;
      case TY_STRING:
	assert( pelem_val );
	pty_e = alloc_type_cons( pos );
	if( pty_e ) {
	  pty_e->pos = pos;
	  pty_e->type.ty = TY_STRING;
	  pelem->ptype = pty_e;
	} else
	  goto failed_memalloc;
	pelem->kids.body.literal.string.s = (char *)pelem_val;
	break;
      case TY_LIST:
	if( !pelem_val ) {
	  pty_e = alloc_type_cons( pos );
	  if( pty_e ) {
	    TYPE_CONS_PTR pty_ply = NULL;
	    pty_e->pos = pos;
	    pty_e->type.ty = TY_LIST;
	    pty_ply = alloc_type_cons( pos );
	    if( pty_ply ) {
	      pty_ply->pos = pos;
	      pty_ply->type.ty = TY_POLY;
	    } else
	      goto failed_memalloc;
	    pty_e->attrs.list.pty_elem = pty_ply;
	    pelem->ptype = pty_e;
	  } else
	    goto failed_memalloc;
	  pelem->kids.body.list.car = NULL;
	  pelem->kids.body.list.cdr = NULL;
	  pelem->kids.body.list.plast = pelem;
	} else {
	  assert( ((EXPR_CONS_PTR)pelem_val)->ptype );
	  assert( (((EXPR_CONS_PTR)pelem_val)->ptype)->type.ty == TY_LIST );
	  pty_e = ((EXPR_CONS_PTR)pelem_val)->ptype;
	  pelem = pelem_val;
	}
	break;
      case TY_POLY:
	/* fall thru. */
      case TY_GEN:
	/* fall thru. */
      case TY_OTHERS:
	/* fall thru. */
      case END_OF_TYPE_CODE:
	/* fall thru. */
      default:
	assert( FALSE );
      }
      pcons->kids.body.list.car = pelem;
      pcons->kids.body.list.cdr = psucc_cs;
      if( pcons->kids.body.list.cdr )
	pcons->kids.body.list.plast = (pcons->kids.body.list.cdr)->kids.body.list.plast;
      else
	pcons->kids.body.list.plast = pcons;
      {
	TYPE_CONS_PTR pty_l = NULL;
	pty_l = alloc_type_cons( pos );
	if( pty_l ) {
	  pty_l->pos = pos;
	  pty_l->type.ty = TY_LIST;
	  pty_l->attrs.list.pty_elem = pty_e;
	} else
	  goto failed_memalloc;
	pcons->ptype = pty_l;
      }
    } else
      goto failed_memalloc;
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return pcons;
}
