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
  char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  
  assert( pvar_attr->type == TY_POLY );
  pident = find_literal( pvar_name );
  if( pident ) {
    assert( strcmp( pident, pvar_name ) == 0 );
    pvar_attr->ident = pident;
    pvar_attr->pos = pos;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void int_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, SRC_POS_C pos ) {
  char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );

  assert( pvar_attr->type == TY_INT );
  pident = find_literal( pvar_name );
  if( pident ) {
    assert( strcmp( pident, pvar_name ) == 0 );
    pvar_attr->ident = pident;
    pvar_attr->pos = pos;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void string_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, const char *s_init, SRC_POS_C pos ) {
  char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  assert( s_init );
  
  assert( pvar_attr->type == TY_STRING );
  pident = find_literal( pvar_name );
  if( pident ) {
    pvar_attr->ident = pident;
    if( !s_init ) {
      char *pc = NULL;
      pc = new_memarea( sizeof(char) );
      if( pc ) {
	*pc = 0;
	s_init = pc;
      } else
	ath_abort( pos, ABORT_MEMLACK );
    }
    assert( s_init );
    pvar_attr->u.var_str.init_s = s_init;
    pvar_attr->pos = pos;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

static void list_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CONS_PTR pl_ty, TYPE_CONS_PTR pl_init, SRC_POS_C pos ) {
  char *pident = NULL;
  assert( pvar_attr );
  assert( pvar_name );
  assert( pl_ty );
  
  assert( pvar_attr->type == TY_LIST );
  pident = find_literal( pvar_name );
  if( pident ) {
    pvar_attr->pos = pos;
    pvar_attr->ident = pident;
    pvar_attr->u.var_list.pty = pl_ty;
    if( pl_init )
      pvar_attr->u.var_list.init_l = pl_init;
    else
      pvar_attr->u.var_list.init_l = pl_ty;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
}

VAR_ATTRIB_PTR decl_attrib_var ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, void *pty_arg, void *pinit, SRC_POS_C pos ) {
  assert( pvar_attr );
  assert( pvar_name );
  switch( pvar_attr->type ) {
  case TY_INT:
    int_var_attrib( pvar_attr, pvar_name, pos );
    break;
  case TY_CHAR:
    break;
  case TY_STRING:
    string_var_attrib( pvar_attr, pvar_name, (const char *)pinit, pos );
    break;
  case TY_LIST:
    list_var_attrib( pvar_attr, pvar_name, (TYPE_CONS_PTR)pty_arg, (TYPE_CONS_PTR)pinit, pos );
    break;
  case TY_POLY:
    poly_var_attrib( pvar_attr, pvar_name, pos );
    break;
  case END_OF_TYPE_CODE:
    /* fall thru. */
  default:
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
  assert( r->u.list.pty_elem == pl_ty );
  assert( ! r->u.list.car );
  assert( ! r->u.list.cdr );
  assert( ! r->u.list.plast );
  return r;
}

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
	  pelem->u.literal.integer.n = *(int *)pelem_val;
	  pdesc->type = pelem->type;
	  pty_e = pdesc;
	  break;
	case TY_CHAR:
	  break;
	case TY_STRING:
	  assert( pelem_val );
	  pelem->type.ty = TY_STRING;
	  pelem->u.literal.string.ps = (char *)pelem_val;
	  pdesc->type = pelem->type;
	  pty_e = pdesc;
	  break;
	case TY_LIST:
	  pelem->type.ty = TY_LIST;
	  pelem->u.list.pty_elem = pdesc;
	  if( pelem_val )
	    assert( FALSE );
	  else
	    pdesc->type.ty = TY_POLY;
	  pty_e = pelem;
	  break;
	case TY_POLY:
	  /* fall thru. */
	case END_OF_TYPE_CODE:
	  /* fall thru. */
	default:
	  assert( FALSE );
	}
	pcons->u.list.pty_elem = pty_e;
	pcons->u.list.car = pelem;
	// and then, typecheck pcons with psucc_es.
	;
	pcons->u.list.cdr = psucc_es;
	if( pcons->u.list.cdr )
	  pcons->u.list.plast = (pcons->u.list.cdr)->u.list.plast;
	else
	  pcons->u.list.plast = pcons;
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
      pty_desc->u.list.pty_elem = plist_elems->u.list.pty_elem;
      pcons->u.list.pty_elem = pty_desc;
      pcons->u.list.car = plist_elems;
      // and then, typecheck pcons with psucc_ls.
      ;
      pcons->u.list.cdr = psucc_ls;
      if( pcons->u.list.cdr )
	pcons->u.list.plast = (pcons->u.list.cdr)->u.list.plast;
      else
	pcons->u.list.plast = pcons;
      r = pcons;
    } else
      goto failed_memalloc;
  } else
  failed_memalloc:
    ath_abort( pos, ABORT_MEMLACK );
  return r;
}
