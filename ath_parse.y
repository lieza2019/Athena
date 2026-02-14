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
static TYPE_CONS_PTR cons_list_elem_basetype ( TYPE_CODE ty, const int row, const int col ) {
  assert( (ty > 0) && (ty < END_OF_TYPE_CODE) );
  TYPE_CONS_PTR r = NULL;
  TYPE_CONS_PTR pty_cons = NULL;
  pty_cons = new_memarea( sizeof(TYPE_CONS) );
  if( pty_cons ) {
    TYPE_CONS_PTR pty_elem = NULL;
    pty_elem = new_memarea( sizeof(TYPE_CONS) );
    if( pty_elem ) {
      assert( pty_cons );
      pty_elem->type = ty;
      pty_cons->type = TY_LIST;
      pty_cons->u.list.pty_elem = pty_elem;
      r = pty_cons;
    } else
      goto err;
  } else
  err:
    ath_abort( ABORT_MEMLACK, row, col );
  return r;
}
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
%token <nat> TK_INT_LITERAL
%token <str> TK_IDENT
%token <str> TK_STR_LITERAL
%type <ptype_cons> list_elem_type
%type <var_decl> decl_var_int
%type <nat> decl_int_init
%type <var_decl> decl_var_string
%type <str> decl_string_init
%type <var_decl> decl_var_list
%type <stmt> statement
%start statement
%%
statement : decl_var_string {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  STATEMENT_PTR pstmt = NULL;
  stmt_decl_var( &pstmt, &$1, &pos );
  assert( pstmt );
  $$ = *pstmt;
 }
| decl_var_int {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  STATEMENT_PTR pstmt = NULL;
  stmt_decl_var( &pstmt, &$1, &pos );
  assert( pstmt );
  $$ = *pstmt;
 }
| decl_var_list {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  STATEMENT_PTR pstmt = NULL;
  stmt_decl_var( &pstmt, &$1, &pos );
  assert( pstmt );
  $$ = *pstmt;
 };

decl_var_int : TK_IDENT TK_AS TK_INT decl_int_init {
  char *pident = NULL;
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.type = TY_INT;
    $$.u.var_int.init_n = $4;
    $$.pos.row = @1.first_line;
    $$.pos.col = @1.first_column;
  } else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, @1.first_line, @1.first_column );
 };
decl_int_init : TK_ASGN TK_INT_LITERAL TK_SMCL {
  $$ = (int)$2;
 };
| TK_SMCL {
  $$ = 0;
  };

decl_var_string : TK_IDENT TK_AS TK_STRING decl_string_init {
  char *pident = NULL;
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.type = TY_STRING;
    $$.u.var_str.init_s = $4;
    $$.pos.row = @1.first_line;
    $$.pos.col = @1.first_column;
  } else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, @1.first_line, @1.first_column );
 };
decl_string_init : TK_ASGN TK_STR_LITERAL TK_SMCL {
  char *pident = NULL;
  pident = find_literal( $2 );
  if( pident )
    $$ = pident;
  else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, @2.first_line, @2.first_column );
  $$ = $2;
 }
| TK_SMCL {
  $$ = "";
 };

decl_var_list : TK_IDENT TK_AS list_elem_type {
  char *pident = NULL;
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.type = TY_POLY;
    $$.u.var_list.pty = $3;
    $$.pos.row = @1.first_line;
    $$.pos.col = @1.first_column;
  } else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, @1.first_line, @1.first_column );
 };
list_elem_type : TK_LSQBL TK_RSQBL {
  $$ = cons_list_elem_basetype( TY_POLY, @1.first_line, @1.first_column );
 }
| TK_LSQBL TK_INT TK_RSQBL {
  $$ = cons_list_elem_basetype( TK_INT, @1.first_line, @1.first_column );
 }
| TK_LSQBL TK_STRING TK_RSQBL {
  $$ = cons_list_elem_basetype( TK_STRING, @1.first_line, @1.first_column );
 }
| TK_LSQBL list_elem_type TK_RSQBL {
  TYPE_CONS_PTR pty_cons = NULL;
  pty_cons = new_memarea( sizeof(TYPE_CONS) );
  if( pty_cons ) {
    pty_cons->type = TY_LIST;
    pty_cons->u.list.pty_elem = $2;
    $$ = pty_cons;
  } else
    ath_abort( ABORT_MEMLACK, @1.first_line, @1.first_column );
 };
%%
int yyerror ( const char *s ) {
  return 1;
}
