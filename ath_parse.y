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
  /* %type <plcell> decl_list_init_elems decl_list_init_elems_closing decl_list_init */
  /* %type <TYPE_CONS_PTR> decl_list_init decl_list_init_elems */
%}
%union {
  int nat;
  char *str;
  TYPE_CONS_PTR ptype_cons;
  TYPE_CONS_PTR plist_init;
  VAR_DECL var_decl;
  LIST_CELL_PTR plcell;
  STATEMENT stmt;
}
%token TK_SMCL
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
%type <plist_init> decl_list_init
%type <stmt> statement
%start statement
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
 }
| decl_var_list {
  STATEMENT_PTR pstmt = NULL;
  {
    printf( "NULL list: %s.", (list_is_nill( $1.u.var_list.pty ) ? "TRUE" : "FALSE") );
  }
  stmt_decl_var( &pstmt, &$1 );
  assert( pstmt );
  $$ = *pstmt;
 };

decl_var_poly : TK_IDENT TK_KEYWORD_AS TK_POLY TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  poly_var_attrib( &$$, $1, pos );
 };

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

decl_var_list : TK_IDENT TK_KEYWORD_AS list_elem_type decl_list_init {
//decl_var_list : TK_IDENT TK_KEYWORD_AS list_elem_type {
  assert( FALSE );
  char *pident = NULL;
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.type = TY_LIST;
    $$.u.var_list.pty = $3;
    $$.u.var_list.pty = $4;
    $$.pos.row = @1.first_line;
    $$.pos.col = @1.first_column;
  } else {
    SRC_POS_C pos = { @1.first_line, @1.first_column };
    ath_abort( ABORT_CANNOT_REG_SYNBOL, pos );
  }
 };
list_elem_type : TK_LSQBL TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = cons_list_elem_basetype( TY_POLY, pos );;
 }
| TK_LSQBL TK_KEYWORD_INT TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = cons_list_elem_basetype( TY_INT, pos );
 }
| TK_LSQBL TK_KEYWORD_STRING TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = cons_list_elem_basetype( TY_STRING, pos );
 }
| TK_LSQBL list_elem_type TK_RSQBL {
  TYPE_CONS_PTR pty_cons = NULL;
  pty_cons = new_memarea( sizeof(TYPE_CONS) );
  if( pty_cons ) {
    pty_cons->type = TY_LIST;
    pty_cons->u.list.cdr = $2;
    pty_cons->pos.row = @1.first_line;
    pty_cons->pos.col = @1.first_column;
    $$ = pty_cons;
  } else {
    SRC_POS_C pos = { @1.first_line, @1.first_column };
    ath_abort( ABORT_MEMLACK, pos );
  }
 };
/*
decl_list_init : TK_LSQBL decl_list_init_elems TK_RSQBL {
  $$ =$2
 };
*/
decl_list_init : TK_LSQBL TK_RSQBL TK_SMCL {
  $$ = NULL;
 };
 /*
decl_list_init_elems : TK_INT_LITERAL TK_COMMA decl_list_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  LIST_CELL_PTR pnew = NULL;
  pnew = alloc_list_cell( pos );
  assert( $3 );
  if( pnew ) {
    pnew->type = TY_INT;
    pnew->u.integer.n = $1;
    pnew->u.list.car = NULL;
    pnew->u.list.cdr = $3;
    $$ = pnew;
  } else
    ath_abort( ABORT_MEMLACK, pos );
 }
| TK_STR_LITERAL TK_COMMA decl_list_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  LIST_CELL_PTR pnew = NULL;
  assert( $3 );
  pnew = alloc_list_cell( pos );
  if( pnew ) {
    pnew->type = TY_STRING;
    pnew->u.string.pstr = $1;
    pnew->u.list.car = NULL;
    pnew->u.list.cdr = $3;
  } else
    ath_abort( ABORT_MEMLACK, pos );
 }
decl_list_init_elems_closing : TK_COMMA TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  LIST_CELL_PTR pnew = NULL;
  pnew = alloc_list_cell( pos );
  if( pnew ) {
    pnew->type= TY_LIST;
    pnew->u.list.car = NULL;
    pnew->u.list.car = NULL;
  } else
    ath_abort( ABORT_MEMLACK, pos );
 }
| TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  LIST_CELL_PTR pnew = NULL;
  pnew = alloc_list_cell( pos );
  if( pnew ) {
    pnew->type= TY_LIST;
    pnew->u.list.car = NULL;
    pnew->u.list.car = NULL;
  } else
    ath_abort( ABORT_MEMLACK, pos );
 };
*/
%%
int yyerror ( const char *s ) {
  return 1;
}
