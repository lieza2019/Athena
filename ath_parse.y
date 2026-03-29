%{
  int yylex();
  int yyerror ( const char *s );
%}
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

#define YYDEBUG 1
%}
%union {
  char tk_chr;
  char *tk_keyword;
  int nat;
  char *str;
  TYPE_CONS_PTR ptype_cons;
  LIST_CELL_PTR plist_init;
  VAR_DECL var_decl;
  LIST_CELL_PTR plcell;
  STATEMENT stmt;
}
%token <tk_chr> TK_SMCL
%token TK_COMMA
%token TK_DQUOT
%token TK_LSQBL TK_RSQBL
%token TK_ASGN
%token TK_KEYWORD_AS
%token TK_KEYWORD_INT
%token TK_KEYWORD_STRING
%token TK_POLY
%token <nat> TK_INT_LITERAL
%token <str> TK_IDENT
%token <str> TK_STR_LITERAL
%type <var_decl> decl_var_poly
%type <var_decl> decl_var_string
%type <str> decl_string_init
%type <var_decl> decl_var_int
%type <nat> decl_int_init
%type <var_decl> decl_var_list
%type <ptype_cons> list_elem_type
%type <plist_init> decl_list_init decl_list_init_elems decl_list_init_elems_tail
%type <stmt> statement
%start statement
%%
statement : decl_var_poly {
  STATEMENT_PTR pstmt = NULL;
  stmt_decl_var( &pstmt, &$1 );
  assert( pstmt );
  $$ = *pstmt;
 }
| decl_var_string {
  STATEMENT_PTR pstmt = NULL;
  stmt_decl_var( &pstmt, &$1 );
  assert( pstmt );
  $$ = *pstmt;
 }
| decl_var_int {
  STATEMENT_PTR pstmt = NULL;
  stmt_decl_var( &pstmt, &$1 );
  assert( pstmt );
  $$ = *pstmt;
 };
| decl_var_list {
  STATEMENT_PTR pstmt = NULL;
  {
    printf( "NULL list: %s.", (list_is_nil( $1.u.var_list.pty ) ? "TRUE" : "FALSE") );
  }
  stmt_decl_var( &pstmt, &$1 );
  assert( pstmt );
  $$ = *pstmt;
 };

decl_var_poly : TK_IDENT TK_KEYWORD_AS TK_POLY TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  poly_var_attrib( &$$, $1, pos );
 }
decl_var_int : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT decl_int_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  int_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_POLY decl_int_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  int_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  int_var_attrib( &$$, $1, 0, pos );
 }
decl_int_init : TK_ASGN TK_INT_LITERAL TK_SMCL {
  $$ = $2;
 };

decl_var_string : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING decl_string_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  string_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_POLY decl_string_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  string_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  string_var_attrib( &$$, $1, NULL, pos );
 };
decl_string_init : TK_ASGN TK_STR_LITERAL TK_SMCL {
  $$ = $2;
 };

decl_var_list : TK_IDENT TK_KEYWORD_AS list_elem_type TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  char *pident = NULL;
  assert( $3 );
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.pos = pos;
    $$.ident = pident;
    $$.type = TY_LIST;
    $$.u.var_list.pty = $3;
    $$.u.var_list.init_l = NULL;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
 }
| TK_IDENT TK_KEYWORD_AS list_elem_type decl_list_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  char *pident = NULL;
  assert( $3 );
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.pos = pos;
    $$.ident = pident;
    $$.type = TY_LIST;
    $$.u.var_list.pty = $3;
    $$.u.var_list.init_l = $4;
  } else
    ath_abort( pos, ABORT_CANNOT_REG_SYNBOL );
 };

list_elem_type : TK_LSQBL TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  TYPE_CONS_PTR pty_desc = NULL;
  pty_desc = alloc_tycons_node( pos );
  if( pty_desc ) {
    pty_desc->pos = pos;
    pty_desc->type = TY_POLY;
    $$ = (TYPE_CONS_PTR)list_creat_nil( pty_desc, pos );
    assert( $$ );
    assert( ($$)->type == TY_LIST );
    assert( ($$)->u.list.pty_elem == pty_desc );
    assert( ! ($$)->u.list.car );
    assert( ! ($$)->u.list.cdr );
    assert( ! ($$)->u.list.plast );    
  } else
    ath_abort( pos, ABORT_CANNOT_CREAT_OBJ );
 }
| TK_LSQBL TK_KEYWORD_INT TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  TYPE_CONS_PTR pty_desc = NULL;
  pty_desc = alloc_tycons_node( pos );
  if( pty_desc ) {
    pty_desc->pos = pos;
    pty_desc->type = TY_INT;
    $$ = (TYPE_CONS_PTR)list_creat_nil( pty_desc, pos );
    assert( $$ );    
    assert( ($$)->type == TY_LIST );
    assert( ($$)->u.list.pty_elem == pty_desc );
    assert( ! ($$)->u.list.car );
    assert( ! ($$)->u.list.cdr );
    assert( ! ($$)->u.list.plast );    
  } else
    ath_abort( pos, ABORT_CANNOT_CREAT_OBJ );
 }
| TK_LSQBL TK_KEYWORD_STRING TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  TYPE_CONS_PTR pty_desc = NULL;
  pty_desc = alloc_tycons_node( pos );
  if( pty_desc ) {
    pty_desc->pos = pos;
    pty_desc->type = TY_STRING;
    $$ = list_creat_nil( pty_desc, pos );
    assert( $$ );
    assert( ($$)->type == TY_LIST );
    assert( ($$)->u.list.pty_elem == pty_desc );
    assert( ! ($$)->u.list.car );
    assert( ! ($$)->u.list.cdr );
    assert( ! ($$)->u.list.plast );
  } else
    ath_abort( pos, ABORT_CANNOT_CREAT_OBJ );
 }
| TK_LSQBL list_elem_type TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( $2 );
  $$ = list_creat_nil( $2, pos );
  assert( $$ );
  assert( ($$)->type == TY_LIST );
  assert( ($$)->u.list.pty_elem == $2 );
  assert( ! ($$)->u.list.car );
  assert( ! ($$)->u.list.cdr );
  assert( ! ($$)->u.list.plast );  
 };

decl_list_init : TK_ASGN TK_LSQBL decl_list_init_elems TK_SMCL {
  $$ = $3;
 };

decl_list_init_elems : TK_INT_LITERAL decl_list_init_elems_tail {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  LIST_CELL_PTR pcell = NULL;
  pcell = alloc_list_cell( pos );
  if( pcell ) {
    LIST_CELL_PTR pelem = NULL;   
    pelem = alloc_list_cell( pos );
    pcell->pos = pos;
    pcell->type = TY_LIST;
    pcell->u.list.plast = NULL;
    if( pelem ) {
      TYPE_CONS_PTR pty_desc = NULL;
      pelem->type = TY_INT;
      pelem->u.integer.n = $1;
      pty_desc = alloc_tycons_node( pos );
      ;
      pcell->u.list.car = pelem;
      pcell->u.list.cdr = $2;
      if( $2 )
	pcell->u.list.plast = $2->u.list.plast;
      else
	pcell->u.list.plast = pcell;
      //pcell->u.list.pty_elem = ;
    } else
      ath_abort( pos, ABORT_MEMLACK );
  } else
    ath_abort( pos, ABORT_MEMLACK );
 }
| TK_STR_LITERAL decl_list_init_elems_tail {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  LIST_CELL_PTR pnew = NULL;
  assert( $2 );
  pnew = alloc_list_cell( pos );
  if( pnew ) {
    pnew->type = TY_STRING;
    pnew->u.string.ps = $1;
    pnew->u.list.car = NULL;
    pnew->u.list.cdr = $2;
    $$ = pnew;
  } else
    ath_abort( pos, ABORT_MEMLACK );
 };
decl_list_init_elems_tail : TK_COMMA decl_list_init_elems {
  $$ = $2;
}
| TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  LIST_CELL_PTR pnew = NULL;
  pnew = alloc_list_cell( pos );
  if( pnew ) {
    pnew->type = TY_STRING;
    pnew->u.list.car = NULL;
    pnew->u.list.cdr = NULL;
    $$ = pnew;
  } else
    ath_abort( pos, ABORT_MEMLACK );
 };
%%
int yyerror ( const char *s ) {
  return 1;
}
