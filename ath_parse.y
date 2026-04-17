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
  VAR_ATTRIB var_attr;
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
%token TK_KEYWORD_POLY
%token <nat> TK_INT_LITERAL
%token <str> TK_IDENT
%token <str> TK_STR_LITERAL
%type <var_attr> decl_var_poly
%type <var_attr> decl_var_string
%type <str> decl_string_init
%type <var_attr> decl_var_int
%type <nat> decl_int_init
%type <var_attr> decl_var_list
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
  stmt_decl_var( &pstmt, &$1 );
  assert( pstmt );
  $$ = *pstmt;
 };

decl_var_poly : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_POLY TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( strlen($1) >= 1 );
  $$.type = TY_POLY;
  decl_attrib_var( &$$, $1, NULL, NULL, pos );
 }

decl_var_int : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( strlen($1) >= 1 );
  $$.type = TY_INT;
  $$.u.var_int.init_n = 0;
  decl_attrib_var( &$$, $1, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT decl_int_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( strlen($1) >= 1 );
  $$.type = TY_INT;
  $$.u.var_int.init_n = $4;
  decl_attrib_var( &$$, $1, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_POLY decl_int_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( strlen($1) >= 1 );
  $$.type = TY_INT;
  $$.u.var_int.init_n = $4;
  decl_attrib_var( &$$, $1, NULL, NULL, pos );
 };
decl_int_init : TK_ASGN TK_INT_LITERAL TK_SMCL {
  $$ = $2;
 };

decl_var_string : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( strlen($1) >= 1 );
  $$.type = TY_STRING;
  decl_attrib_var( &$$, $1, NULL, "", pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING decl_string_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( strlen($1) >= 1 );
  $$.type = TY_STRING;
  decl_attrib_var( &$$, $1, NULL, $4, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_POLY decl_string_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( strlen($1) >= 1 );
  $$.type = TY_STRING;
  decl_attrib_var( &$$, $1, NULL, $4, pos );
 }
decl_string_init : TK_ASGN TK_STR_LITERAL TK_SMCL {
  $$ = $2;
 };

decl_var_list : TK_IDENT TK_KEYWORD_AS list_elem_type TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( $3 );
  assert( strlen($1) >= 1 );
  $$.type = TY_LIST;
  decl_attrib_var( &$$, $1, $3, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS list_elem_type decl_list_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };  
  assert( $3 );
  assert( strlen($1) >= 1 );
  $$.type = TY_LIST;
  decl_attrib_var( &$$, $1, $3, $4, pos );
 };

list_elem_type : TK_LSQBL TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = var_list_type( NULL, TY_POLY, pos );
 }
| TK_LSQBL TK_KEYWORD_POLY TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = var_list_type( NULL, TY_POLY, pos );
 }
| TK_LSQBL TK_KEYWORD_INT TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = var_list_type( NULL, TY_INT, pos );
 }
| TK_LSQBL TK_KEYWORD_STRING TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = var_list_type( NULL, TY_STRING, pos );
 }
| TK_LSQBL list_elem_type TK_RSQBL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = var_list_type( $2, TY_LIST, pos );
 };

decl_list_init : TK_ASGN TK_LSQBL TK_RSQBL TK_SMCL {
  $$ = NULL;
 }
| TK_ASGN TK_LSQBL decl_list_init_elems TK_SMCL {
  $$ = $3;
 };

decl_list_init_elems : TK_INT_LITERAL decl_list_init_elems_tail {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = value_list_elem( TY_INT, &$1, $2, pos );
 }
| TK_STR_LITERAL decl_list_init_elems_tail {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = value_list_elem( TY_STRING, $1, $2, pos );
 }
| TK_LSQBL TK_RSQBL decl_list_init_elems_tail {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = value_list_elem( TY_LIST, NULL, $3, pos );
 }
| TK_LSQBL decl_list_init_elems decl_list_init_elems_tail {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( $2 );
  assert( ($2)->type.ty == TY_LIST );
  $$ = value_list( $2, $3, pos );
 };

decl_list_init_elems_tail : TK_COMMA decl_list_init_elems {
  $$ = $2;
}
| TK_RSQBL {
  $$ = NULL;
 };
%%
int yyerror ( const char *s ) {
  return 1;
}
