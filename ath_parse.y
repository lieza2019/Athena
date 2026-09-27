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
  PROC_ATTRIB proc_attr;
  STATEMENT_PTR pstmt_last;
  DECLARATION_PTR pdecl_last;
}
%token <tk_chr> TK_SMCL
%token TK_COMMA
%token TK_DQUOT
%token TK_LBRA TK_RBRA
%token TK_LPAR TK_RPAR
%token TK_LSQBL TK_RSQBL
%token TK_ASGN
%token TK_STAR
%token TK_SLASH
%token TK_DECR TK_PREDECR TK_PSTDECR
%token TK_MINUS
%token TK_INCR TK_PREINCR TK_PSTINCR
%token TK_CROSS
%token TK_KEYWORD_AS
%token TK_KEYWORD_INT
%token TK_KEYWORD_STRING
%token TK_KEYWORD_POLY
%token TK_KEYWORD_PROC
%token <nat> TK_INT_LITERAL
%token <str> TK_IDENT
%token <str> TK_STR_LITERAL

%right TK_ASGN
%left TK_CROSS TK_MINUS
%left TK_STAR TK_SLASH
%right TK_DECR TK_INCR

%type <pvar_init> expression unary_expr primary_expr binary_expr
%type <pvar_init> const_int const_str
%type <pvar_init> const_list decl_list_init_elems decl_list_init_elems_tail
%type <pty_list_elem> list_elem_type
%type <var_attr> decl_var decl_var_poly decl_var_int decl_var_string decl_var_list
 /* %type <proc_attr> decl_proc */
%type <pstmt_last> statement statements
%type <pdecl_last> declaration declarations
%start statements
 /* %start declarations */
%%
declarations : declarations declaration {
 }
| declaration : TK_KEYWORD_PROC TK_LPAR TK_RPAR TK_LBRA TK_RBRA {
 };
/*
decl_proc : {
  ;
};
*/

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

statement : decl_var TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  STATEMENT_PTR pstmt = NULL;
  pstmt = vardecl2_stmt( &$1, pos );
  assert( pstmt );
  $$ = pstmt;
 }
| expression TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  STATEMENT_PTR pstmt = NULL;
  pstmt = expr2_stmt( $1, pos );
  assert( pstmt );
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

decl_var_poly : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_POLY {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_POLY, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_POLY TK_ASGN expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_POLY, NULL, $5, pos );
 };

decl_var_int : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_INT, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_INT TK_ASGN expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_INT, NULL, $5, pos );
 };

decl_var_string : TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_STRING, NULL, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS TK_KEYWORD_STRING TK_ASGN expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_STRING, NULL, $5, pos );
  };

decl_var_list : TK_IDENT TK_KEYWORD_AS list_elem_type {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_LIST, $3, NULL, pos );
 }
| TK_IDENT TK_KEYWORD_AS list_elem_type TK_ASGN expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  decl_var_attrib( &$$, $1, TY_LIST, $3, $5, pos );
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

expression : binary_expr {
  $$ = $1;
 }
| unary_expr {
  $$ = $1;
 }
| primary_expr {
  $$ = $1;
 };

binary_expr : expression TK_STAR expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_binary_expr( $1, $3, TK_STAR, pos );
 }
| expression TK_SLASH expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_binary_expr( $1, $3, TK_SLASH, pos );
 }
| expression TK_CROSS expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_binary_expr( $1, $3, TK_CROSS, pos );
 }
| expression TK_MINUS expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_binary_expr( $1, $3, TK_MINUS, pos );
}
| expression TK_ASGN expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_binary_expr( $1, $3, TK_ASGN, pos );
 }

unary_expr : TK_MINUS expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_unary_expr( $2, TK_MINUS, pos );
 }
| TK_DECR expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_unary_expr( $2, TK_PREDECR, pos );
 }
| expression TK_DECR {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_unary_expr( $1, TK_PSTDECR, pos );
 }
| TK_INCR expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_unary_expr( $2, TK_PREINCR, pos );
 }
| expression TK_INCR {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_unary_expr( $1, TK_PSTINCR, pos );
 };

primary_expr : TK_IDENT {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  $$ = rval_primary_expr( $1, pos );
 }
| const_int {
  $$ = $1;
 }
| const_str {
  $$ = $1;
 }
| const_list {
  $$ = $1;
 };

const_int : TK_INT_LITERAL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };  
  EXPR_CONS_PTR pval_int = NULL;
  pval_int = alloc_expr_cons( pos );
  if( pval_int ) {
    TYPE_CONS_PTR pty_int = NULL;
    pval_int->pos = pos;
    pval_int->mnemonic = MNC_CONST;
    pval_int->kids.body.literal.integer.n = $1;
    pty_int = alloc_type_cons( pos );
    if( pty_int ) {
      pty_int->pos = pos;
      pty_int->type.ty = TY_INT;
    } else
      goto failed_memalloc_const_int;
    pval_int->ptype = pty_int;
  } else
  failed_memalloc_const_int:
    ath_abort( pos, ABORT_MEMLACK );
  $$ = pval_int;
 };

const_str : TK_STR_LITERAL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  EXPR_CONS_PTR pval_string = NULL;
  pval_string = alloc_expr_cons( pos );
  if( pval_string ) {
    TYPE_CONS_PTR pty_str = NULL;
    pval_string->pos = pos;
    pval_string->mnemonic = MNC_CONST;
    pval_string->kids.body.literal.string.s = find_literal( $1, pos );
    assert( pval_string->kids.body.literal.string.s );
    pty_str = alloc_type_cons( pos );
    if( pty_str ) {
      pty_str->pos = pos;
      pty_str->type.ty = TY_STRING;
    } else
      goto failed_memalloc_const_str;
    pval_string->ptype = pty_str;
  } else
  failed_memalloc_const_str:
    ath_abort( pos, ABORT_MEMLACK );
  $$ = pval_string;
 };

const_list : TK_LSQBL decl_list_init_elems {
  $$ = $2;
 }
| TK_RSQBL {
  $$ = NULL;
 }

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
  assert( ($2)->ptype );
  assert( (($2)->ptype)->type.ty == TY_LIST );
  $$ = value_list_elem( TY_LIST, $2, $3, pos );
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
