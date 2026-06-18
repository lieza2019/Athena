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
  int nat;
  char *str;
  EXPR_CONS_PTR pvar_init;
  TYPE_CONS_PTR pty_list_elem;
  VAR_ATTRIB var_attr;
  STATEMENT_PTR pstmt_last;
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
%type <pvar_init> decl_int_init decl_string_init
%type <pvar_init> decl_list_init decl_list_init_elems decl_list_init_elems_tail
%type <pvar_init> decl_var_init
%type <pty_list_elem> list_elem_type
%type <var_attr> decl_var_poly decl_var_int decl_var_string decl_var_list
%type <var_attr> decl_var
%type <pstmt_last> statement statements
%start statements
%%
statements : statements statement {
  assert( statements.phead );
  assert( $1 );
  assert( statements.plast == $1 );
  assert( $2 );
  ($2)->psucc = NULL;
  (statements.plast)->psucc = $2;
  statements.plast = $2;
  $$ = statements.plast;
 }
| statement {
  assert( $1 );
  statements.phead = $1;
  statements.plast = statements.phead;
  $$ = statements.plast;
 };

statement : decl_var {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  STATEMENT_PTR pstmt = NULL;
  VAR_ATTRIB_PTR pvattr = NULL;
  assert( $1.ident );
  assert( $1.ptype );
#ifdef RUNTIME_CONSITENCY_CHECK
  exam_tycon( $1.ptype );
  if( $1.pinit )
    exam_tycon( $1.pinit );
#endif // RUNTIME_CONSITENCY_CHECK
  pvattr = alloc_var_attr( pos );
  if( pvattr ) {
    pvattr->pos = $1.pos;
    pvattr->ident = $1.ident;
    pvattr->ptype = $1.ptype;
    pvattr->pinit = $1.pinit;
    stmt_decl_var( &pstmt, pvattr, pos ); /* <- NOW HERE. */
    assert( pstmt );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  /* tychk_decl_var( pstmt, pos ); */
  $$ = pstmt;
 };

decl_var : decl_var_poly {
  $$ = $1;
 }
| decl_var_int {
  $$ = $1;
 }
| decl_var_string {
  $$ = $1;
 }
| decl_var_list {
  $$ = $1;
 };

decl_var_poly : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_POLY TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_POLY, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_POLY decl_var_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_POLY, NULL, $4, pos );
 };

decl_var_int : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_INT, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT decl_var_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_INT, NULL, $4, pos );
 };

decl_var_string : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_STRING, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING decl_var_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_STRING, NULL, $4, pos );
 };

decl_var_list : TK_IDENT TK_KEYWORD_AS list_elem_type TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_LIST, $3, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS list_elem_type decl_var_init {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_LIST, $3, $4, pos );
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
