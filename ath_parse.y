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
  int nat;
  char *str;
  TYPE_CONS_PTR ptype_cons;
  VAR_DECL var_decl;
  STATEMENT stmt;
}
%token TK_SMCL
%token TK_DQUOT
%token TK_LSQBL TK_RSQBL
%token TK_ASGN
%token TK_AS
%token TK_INT
%token TK_STRING
%token TK_POLY
%token <nat> TK_INT_LITERAL
%token <str> TK_IDENT
%token <str> TK_STR_LITERAL
%type <ptype_cons> list_elem_type
%type <var_decl> decl_var_poly
%type <var_decl> decl_var_int
%type <nat> decl_int_init
%type <var_decl> decl_var_string
%type <str> decl_string_init
%type <var_decl> decl_var_list
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
 }
| decl_var_list {
  STATEMENT_PTR pstmt = NULL;
  stmt_decl_var( &pstmt, &$1 );
  assert( pstmt );
  $$ = *pstmt;
 };

decl_var_poly : TK_IDENT TK_AS TK_POLY TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  poly_var_attrib( &$$, $1, pos );
 };

decl_var_int : TK_IDENT TK_AS TK_INT decl_int_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  int_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_AS TK_POLY decl_int_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  int_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_AS TK_INT TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  int_var_attrib( &$$, $1, 0, pos );
 }
decl_int_init : TK_ASGN TK_INT_LITERAL TK_SMCL {
  $$ = $2;
 };

decl_var_string : TK_IDENT TK_AS TK_STRING decl_string_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  string_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_AS TK_POLY decl_string_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  string_var_attrib( &$$, $1, $4, pos );
 }
| TK_IDENT TK_AS TK_STRING TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  string_var_attrib( &$$, $1, NULL, pos );
 };
decl_string_init : TK_ASGN TK_STR_LITERAL TK_SMCL {
  $$ = $2;
 };

decl_var_list : TK_IDENT TK_AS list_elem_type {
  char *pident = NULL;
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.type = TY_LIST;
    $$.u.var_list.pty = $3;
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
| TK_LSQBL TK_INT TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = cons_list_elem_basetype( TY_INT, pos );
 }
| TK_LSQBL TK_STRING TK_RSQBL {
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
%%
int yyerror ( const char *s ) {
  return 1;
}
