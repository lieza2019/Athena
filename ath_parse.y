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
  TYPE_CONS_PTR pty_obj;
  TYPE_CONS_PTR pty_list_elem;
  EXPR_CONS_PTR pvar_init;
  VAR_ATTRIB_PTR pvar_attr;
  PROC_ATTRIB_PTR pproc_attr;
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

%type <pty_obj> obj_type
%type <pty_list_elem> list_elem_type
%type <pvar_init> expression unary_expr primary_expr binary_expr
%type <pvar_init> const_int const_str
%type <pvar_init> const_list decl_list_init_elems decl_list_init_elems_tail
%type <pvar_attr> var_decl var_decl_list proc_args
%type <pproc_attr> decl_proc
%type <pstmt_last> statement statements
%type <pdecl_last> declaration declarations

%start declarations
%%
declarations : declarations declaration {
  assert( $1 );
  assert( $2 );
  assert( declarations.phead );
  assert( declarations.plast == $1 );
  (declarations.plast)->pnext = $2;
  $2->pnext = NULL;
  declarations.plast = $2;
  $$ = declarations.plast;
 }
| declaration {
  assert( $1 );
  if( ! declarations.phead ) {
    assert( ! declarations.plast );
    declarations.phead = $1;
    declarations.plast = $1;
  }
  $1->pnext = NULL;
  $$ = $1;
 };

declaration : decl_proc {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  DECLARATION_PTR pdecl = NULL;
  assert( $1 );
  pdecl = alloc_decl_attr( pos );
  if( pdecl ) {
    pdecl->pos = pos;
    pdecl->ident = $1->ident;
    pdecl->kind = DECL_PROC;
    pdecl->u.procedure.pproc = $1;
  } else
    ath_abort( pos, ABORT_MEMLACK );
  $$ = pdecl;
 };

decl_proc : TK_KEYWORD_PROC obj_type TK_IDENT TK_LPAR proc_args TK_RPAR TK_LBRA statements TK_RBRA {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  /* e.g. proc int foo ( n as int, s as string, b as bool ) { statements } */
  PROC_ATTRIB_PTR pproc = NULL;
  assert( $3 );
  /* staffs as follows, may have no-value. in such cases,
     the DEFAULT value designated for each, would be applied.
     obj_type
     proc_args
     statements
  */
  pproc = decl_proc( $3, $2, $5, &statements, pos );
  $$ = pproc;
};

proc_args : proc_args TK_COMMA var_decl {
  SRC_POS_C pos = { @3.first_line, @3.first_column };
  assert( $1 );
  assert( $3 );
  assert( ! $3->opts.proc_args.parg_next );
  if( $3->opts.var_decl.with_init )
    err_print( pos, "procedure arguments have no initialization.\n" );
  $3->opts.proc_args.parg_next = NULL;
  if( $1->opts.proc_args.parg_head )
    $3->opts.proc_args.parg_head = $1->opts.proc_args.parg_head;
  else
    $3->opts.proc_args.parg_head = $1;
  $1->opts.proc_args.parg_next = $3;
  $$ = $3;
 }
| var_decl {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( $1 );
  if( $1->opts.var_decl.with_init )
    err_print( pos, "procedure arguments have no initialization.\n" );
  $1->opts.proc_args.parg_head = NULL;
  $1->opts.proc_args.parg_next = NULL;
  $$ = $1;
 };

statements : statements statement {
  assert( $1 );
  assert( $2 );
  assert( statements.phead );
  assert( statements.plast == $1 );
  (statements.plast)->psucc = $2;
  $2->psucc = NULL;
  statements.plast = $2;
  $$ = statements.plast;
 }
| statement {
  assert( $1 );
  if( ! statements.phead ) {
    assert( ! statements.plast );
    statements.phead = $1;
    statements.plast = $1;
  }
  $1->psucc = NULL;
  $$ = $1;
 };

statement : var_decl TK_SMCL {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  STATEMENT_PTR pstmt = NULL;
  pstmt = vardecl2_stmt( $1, pos );
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

var_decl : TK_IDENT TK_KEYWORD_AS obj_type {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( $3 );
  VAR_ATTRIB_PTR pattr = NULL;
  pattr = alloc_var_attr( pos );
  if( pattr ) {
    pattr->opts.var_decl.with_init = FALSE;
    decl_var_attrib( pattr, $1, $3->type.ty, NULL, NULL, pos );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  $$ = pattr;
 }
| TK_IDENT TK_KEYWORD_AS obj_type TK_ASGN expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  assert( $3 );
  VAR_ATTRIB_PTR pattr = NULL;
  pattr = alloc_var_attr( pos );
  if( pattr ) {
    pattr->opts.var_decl.with_init = TRUE;
    decl_var_attrib( pattr, $1, $3->type.ty, NULL, $5, pos );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  $$ = pattr;
 }
| var_decl_list {
  $$ = $1;
 };

var_decl_list : TK_IDENT TK_KEYWORD_AS list_elem_type {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  VAR_ATTRIB_PTR pattr = NULL;
  pattr = alloc_var_attr( pos );
  if( pattr )
    decl_var_attrib( pattr, $1, TY_LIST, $3, NULL, pos );
  else
    ath_abort( pos, ABORT_MEMLACK );
  $$ = pattr;
 }
| TK_IDENT TK_KEYWORD_AS list_elem_type TK_ASGN expression {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  VAR_ATTRIB_PTR pattr = NULL;
  pattr = alloc_var_attr( pos );
  if( pattr ) {
    decl_var_attrib( pattr, $1, TY_LIST, $3, $5, pos );
  } else
    ath_abort( pos, ABORT_MEMLACK );
  $$ = pattr;
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

obj_type : TK_KEYWORD_POLY {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  TYPE_CONS_PTR pty_poly = NULL;
  pty_poly = alloc_type_cons( pos );
  if( pty_poly ) {
    pty_poly->type.ty = TY_POLY;
    $$ = pty_poly;
  } else
    ath_abort( pos, ABORT_MEMLACK );
 }
| TK_KEYWORD_INT {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  TYPE_CONS_PTR pty_int = NULL;
  pty_int = alloc_type_cons( pos );
  if( pty_int ) {
    pty_int->type.ty = TY_INT;
    $$ = pty_int;
  } else
    ath_abort( pos, ABORT_MEMLACK );
 }
| TK_KEYWORD_STRING {
  SRC_POS_C pos = { @1.first_line, @1.first_column };
  TYPE_CONS_PTR pty_string = NULL;
  pty_string = alloc_type_cons( pos );
  if( pty_string ) {
    pty_string->type.ty = TY_STRING;
    $$ = pty_string;
  } else
    ath_abort( pos, ABORT_MEMLACK );
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
