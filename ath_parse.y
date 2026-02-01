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
  VAR_DECL var_decl;
  STATEMENT stmt;
}
%token TK_SMCL
%token TK_ASGN
%token TK_DQUOT
%token TK_AS
%token TK_INT
%token TK_STRING
%token <nat> TK_INT_LITERAL
%token <str> TK_IDENT
%token <str> TK_STRING
%type <var_decl> decl_var_int
%type <nat> decl_int_init
%type <var_decl> decl_var_string
%type <str> string_const
%type <stmt> statement
%start statement
%%
statement : decl_var_int | decl_string_init {
  BOOL redef = FALSE;
  STATEMENT_PTR pstmt = NULL;
  pstmt = new_stmt();
  if( pstmt ) {
    DECL_ATTRIB_PTR pattr = NULL;
    VAR_DECL var;
    var = $1;
    redef = ! decl_var( &pattr, &var );
    if( !redef ) {
      pstmt->pos = pattr->u.var.pos;
      pstmt->kind = STMT_DECL;
      pstmt->u.pdecl = pattr;
    } else
      err_redef( pattr, @1.first_line, @1.first_column );
  } else
    ath_abort( ABORT_MEMLACK, @1.first_line, @1.first_column );
 };

decl_var_int : TK_IDENT TK_AS TK_INT decl_int_init {
  char *pident = NULL;
  assert( strlen($1) >= 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.ty = TY_INT;
    $$.u.var_int.nat = $4;
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
    $$.ty = TY_STRING;
    $$.u.var_int.nat = $4;
    $$.pos.row = @1.first_line;
    $$.pos.col = @1.first_column;
  } else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, @1.first_line, @1.first_column );
 };
decl_string_init : TK_ASGN string_const TK_SMCL {
  $$ = $2
 }
| TK_SMCL {
  $$ = "";
 };
string_const : TK_DQUOT TK_STRING TK_DQUOT {
  char *pident = NULL;;
  pident = find_literal( $2 );
  if( pident )
    $$ = pident;
  else
    ath_abort( ABORT_CANNOT_REG_SYNBOL, @2.first_line, @2.first_column );
 };
%%
int yyerror ( const char *s ) {
  return 1;
}
