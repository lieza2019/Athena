#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

TYPE_CONS_PTR alloc_tycons_node ( SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  r = (TYPE_CONS_PTR)alloc_list_cell( pos );
  return r;
}

static void poly_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );  
  assert( (pvar_attr->ptype)->type.ty == TY_POLY );
  assert( pvar_name );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    pvar_attr->ident = pident;
    pvar_attr->pos = pos;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void int_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CONS_PTR pn_init, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    if( !pn_init ) {
      pn_init = alloc_type_cons( pos );
      if( pn_init ) {
	pn_init->pos = pos;
	pn_init->type.ty = TY_INT;
	pn_init->attrs.literal.integer.n = 0;
      } else
	ath_abort( pos, ABORT_MEMLACK );
    } else
      assert( pn_init->type.ty == TY_INT );
    assert( pn_init );
    pvar_attr->ident = pident;
    pvar_attr->pos = pos;
    pvar_attr->ptype = pn_init;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void string_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CONS_PTR ps_init, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    if( !ps_init ) {
      ps_init = alloc_type_cons( pos );
      if( ps_init ) {
	char *e = NULL;
	e = new_memarea( 1 );
	if( e )
	  *e = 0;
	else
	  goto failed_memalloc;
	ps_init->pos = pos;
	ps_init->type.ty = TY_STRING;
	ps_init->attrs.literal.string.ps = e;
      } else
      failed_memalloc:
	ath_abort( pos, ABORT_MEMLACK );
    } else
      assert( ps_init->type.ty == TY_STRING );
    assert( ps_init );
    pvar_attr->ident = pident;
    pvar_attr->pos = pos;
    pvar_attr->ptype = ps_init;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void list_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CONS_PTR pty_list, TYPE_CONS_PTR pinit, SRC_POS_C pos ) {
  const char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  assert( pinit );
  assert( pinit->type.ty == TY_LIST );
  
  pident = find_literal( pvar_name, pos );
  if( pident ) {
    pvar_attr->pos = pos;
    pvar_attr->ident = pident;
    pvar_attr->ptype = pty_list;
    if( pinit )
      pvar_attr->pinit = pinit;
    else
      pvar_attr->pinit = pty_list;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

VAR_ATTRIB_PTR decl_attrib_var ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CODE var_type, TYPE_CONS_PTR type_arg, void *pinit, SRC_POS_C pos ) {
  assert( pvar_attr );
  assert( pvar_name );
  switch( var_type ) {
  case TY_EXPR:
    goto illegal_var_type;
  case TY_INT:
    assert( !type_arg );
    int_var_attrib( pvar_attr, pvar_name, (TYPE_CONS_PTR)pinit, pos );
    break;
  case TY_CHAR:
    assert( !type_arg );
    break;
  case TY_STRING:
    assert( !type_arg );
    string_var_attrib( pvar_attr, pvar_name, (TYPE_CONS_PTR)pinit, pos );
    break;
  case TY_LIST:
    assert( type_arg );
    list_var_attrib( pvar_attr, pvar_name, type_arg, (TYPE_CONS_PTR)pinit, pos );
    break;
  case TY_POLY:
    poly_var_attrib( pvar_attr, pvar_name, pos );
  case TY_GEN:
    /* fall thru. */
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
  illegal_var_type:
    assert( FALSE );
  }
  return pvar_attr;
}

TYPE_CONS_PTR var_list_type ( TYPE_CONS_PTR pl_ty, TYPE_CODE elem_ty, SRC_POS_C pos ) {
  TYPE_CONS_PTR r = NULL;
  if( elem_ty == TY_LIST ) {
    assert( pl_ty );
    r = (TYPE_CONS_PTR)list_creat_nil( pl_ty, pos );
    if( !r )
      goto failed_memalloc;
  } else {
    TYPE_CONS_PTR pty_desc = NULL;
    assert( !pl_ty );
    pty_desc = alloc_tycons_node( pos );
    if( pty_desc ) {
      pty_desc->pos = pos;
      pty_desc->type.ty = elem_ty;
      r = (TYPE_CONS_PTR)list_creat_nil( pty_desc, pos );
      if( !r )
	goto failed_memalloc;
      pl_ty = pty_desc;
    } else
    failed_memalloc:
      ath_abort( pos, ABORT_MEMLACK );
  }
  assert( r );
  assert( r->type.ty == TY_LIST );
  assert( r->attrs.list.pty_elem == pl_ty );
  assert( ! r->attrs.list.car );
  assert( ! r->attrs.list.cdr );
#if 0 // *****
  assert( ! r->attrs.list.plast );
#else
  r->attrs.list.plast = r;
#endif
  return r;
}

#if 0 // *****
LIST_CELL_PTR value_list ( LIST_CELL_PTR plist_elems, LIST_CELL_PTR psucc_ls, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  LIST_CELL_PTR pcons = NULL;
  assert( plist_elems );
  assert( plist_elems->type.ty == TY_LIST );
  
  pcons = alloc_list_cell( pos );
  if( pcons ) {
    TYPE_CONS_PTR pty_desc = NULL;
    pcons->pos = pos;
    pcons->type.ty = TY_LIST;
    pty_desc = alloc_tycons_node( pos );
    if( pty_desc ) {
      pty_desc->pos = pos;
      pty_desc->type.ty = TY_LIST;
      pty_desc->attrs.list.pty_elem = plist_elems->attrs.list.pty_elem;
      pcons->attrs.list.pty_elem = pty_desc;
      pcons->attrs.list.car = plist_elems;
      // and then, typecheck pcons with psucc_ls.
      ;
      pcons->attrs.list.cdr = psucc_ls;
      if( pcons->attrs.list.cdr )
	pcons->attrs.list.plast = (pcons->attrs.list.cdr)->attrs.list.plast;
      else
	pcons->attrs.list.plast = pcons;
      r = pcons;
    } else
      goto failed_memalloc;
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
#else
static TYPE_CONS_PTR retrive_type ( TYPE_CONS_PTR car, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  assert( car );
  
  switch( car->type.ty ) {
  case TY_EXPR:
    assert( car->attrs.expr.pexpr );
    r = (car->attrs.expr.pexpr)->ptype;
    break;
  case TY_INT:
  case TY_CHAR:
  case TY_STRING:
    r = car;
    break;
  case TY_LIST:
    assert( car->attrs.list.pty_elem );
    {
      LIST_CELL_PTR pcons = NULL;
      pcons = alloc_list_cell( pos );
      if( pcons ) {
	pcons->pos = pos;
	pcons->type.ty = TY_LIST;
	pcons->attrs.list.pty_elem = car->attrs.list.pty_elem;
      } else
	goto failed_memalloc;
      assert( ! pcons->attrs.list.car );
      assert( ! pcons->attrs.list.cdr );
      assert( ! pcons->attrs.list.plast );
      r = pcons;
    }    
    break;
  case TY_POLY:
    r = car;
    break;
  case TY_GEN:
    /* fall thru. */
  case TY_OTHERS:
    /* fall thru. */
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
    assert( FALSE );
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  }
  return r;
}
LIST_CELL_PTR value_list ( LIST_CELL_PTR plist_elems, LIST_CELL_PTR psucc_ls, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  LIST_CELL_PTR pcons = NULL;
  assert( plist_elems );
  assert( plist_elems->type.ty == TY_LIST );
  
  pcons = alloc_list_cell( pos );
  if( pcons ) {
    pcons->pos = pos;
    pcons->type.ty = TY_LIST;
    pcons->attrs.list.car = plist_elems;
    pcons->attrs.list.pty_elem = retrive_type( pcons->attrs.list.car, pos );
    pcons->attrs.list.cdr = psucc_ls;
    if( pcons->attrs.list.cdr )
      pcons->attrs.list.plast = (pcons->attrs.list.cdr)->attrs.list.plast;
    else
      pcons->attrs.list.plast = pcons;
    r = pcons;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
#endif

#if 0 // *****
LIST_CELL_PTR value_list_elem ( TYPE_CODE elem_ty, void *pelem_val, LIST_CELL_PTR psucc_es, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  LIST_CELL_PTR pcons = NULL;
  
  pcons = alloc_list_cell( pos );
  if( pcons ) {
    LIST_CELL_PTR pelem = NULL;
    pcons->pos = pos;
    pcons->type.ty = TY_LIST;
    pelem = alloc_list_cell( pos );
    if( pelem ) {
      TYPE_CONS_PTR pdesc = NULL;
      pdesc = alloc_tycons_node( pos );
      if( pdesc ) {
	TYPE_CONS_PTR pty_e = NULL;
	pdesc->pos = pos;
	switch( elem_ty ) {
	case TY_INT:
	  assert( pelem_val );
	  pelem->type.ty = TY_INT;
	  pelem->attrs.literal.integer.n = *(int *)pelem_val;
	  pdesc->type = pelem->type;
	  pty_e = pdesc;
	  break;
	case TY_CHAR:
	  break;
	case TY_STRING:
	  assert( pelem_val );
	  pelem->type.ty = TY_STRING;
	  pelem->attrs.literal.string.ps = (char *)pelem_val;
	  pdesc->type = pelem->type;
	  pty_e = pdesc;
	  break;
	case TY_LIST:
	  pelem->type.ty = TY_LIST;
	  pelem->attrs.list.pty_elem = pdesc;
	  if( pelem_val )
	    assert( FALSE );
	  else
	    pdesc->type.ty = TY_POLY;
	  pty_e = pelem;
	  break;
	case TY_POLY:
	  /* fall thru. */
	case TY_OTHERS:
	  /* fall thru. */
	case END_OF_TYPE_CODE:
	  /* fall thru. */
	default:
	  assert( FALSE );
	}
	pcons->attrs.list.pty_elem = pty_e;
	pcons->attrs.list.car = pelem;
	// and then, typecheck pcons with psucc_es.
	;
	pcons->attrs.list.cdr = psucc_es;
	if( pcons->attrs.list.cdr )
	  pcons->attrs.list.plast = (pcons->attrs.list.cdr)->attrs.list.plast;
	else
	  pcons->attrs.list.plast = pcons;
	r = pcons;
      } else
	goto failed_memalloc;
    } else
      goto failed_memalloc;
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
#else
LIST_CELL_PTR value_list_elem ( TYPE_CODE elem_ty, void *pelem_val, LIST_CELL_PTR psucc_es, SRC_POS_C pos ) {
  LIST_CELL_PTR r = NULL;
  LIST_CELL_PTR pcons = NULL;
  
  pcons = alloc_list_cell( pos );
  if( pcons ) {
    LIST_CELL_PTR pelem = NULL;
    pcons->pos = pos;
    pcons->type.ty = TY_LIST;
    pelem = alloc_list_cell( pos );
    if( pelem ) {
      TYPE_CONS_PTR pty_e = NULL;
      switch( elem_ty ) {
      case TY_EXPR:
	goto illegal_elem_type;
      case TY_INT:
	assert( pelem_val );
	pelem->type.ty = TY_INT;
	pelem->attrs.literal.integer.n = *(int *)pelem_val;
	pty_e = pelem;
	break;
      case TY_CHAR:
	break;
      case TY_STRING:
	assert( pelem_val );
	pelem->type.ty = TY_STRING;
	pelem->attrs.literal.string.ps = (char *)pelem_val;
	pty_e = pelem;
	break;
      case TY_LIST:
	pelem->type.ty = TY_LIST;
	if( pelem_val )
	  assert( FALSE );
	else {
	  TYPE_CONS_PTR pdesc = NULL;
	  pdesc = alloc_tycons_node( pos );
	  if( pdesc ) {
	    pdesc->type.ty = TY_POLY;
	    pelem->attrs.list.pty_elem = pdesc;
	  } else
	    goto failed_memalloc;
	}
	pty_e = pelem;
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
      illegal_elem_type:
	assert( FALSE );
      }
      pcons->attrs.list.pty_elem = pty_e;
      pcons->attrs.list.car = pelem;
      pcons->attrs.list.cdr = psucc_es;
      if( pcons->attrs.list.cdr )
	pcons->attrs.list.plast = (pcons->attrs.list.cdr)->attrs.list.plast;
      else
	pcons->attrs.list.plast = pcons;
      r = pcons;
    } else
      goto failed_memalloc;
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
#endif
