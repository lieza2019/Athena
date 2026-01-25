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
%token SMCL
%token TK_AS
%token TK_INT
%token <str> TK_IDENT
%type <var_decl> decl_var
%type <stmt> statement
%start statement
%%
statement : decl_var SMCL {
  BOOL redef = FALSE;
  STATEMENT_PTR pstmt = NULL;
  
  pstmt = new_stmt();
  if( pstmt ) {
    DECL_ATTRIB_PTR pattr = NULL;
    VAR_DECL var;
    pstmt->pos.row = @1.first_line;
    pstmt->pos.col = @1.first_column;
    var.ident = $1.ident;
    var.ty = $1.ty;
    switch( var.ty ) {
    case TY_INT:
      var.u.var_int.nat = 0;
      break;
    default:
      assert( FALSE );
    }
    redef = ! decl_var( &pattr, &var );
    if( !redef ) {
      pstmt->kind = STMT_DECL;
      pstmt->u.pdecl = pattr;
    } else
      ;
  } else
    ;
 };
decl_var : TK_IDENT TK_AS TK_INT {
  char *pident = NULL;;
  assert( strlen($1) > 1 );
  pident = find_literal( $1 );
  if( pident ) {
    $$.ident = pident;
    $$.ty = TY_INT;
    $$.u.var_int.nat = 0;
    $$.pos.row = @1.first_line;
    $$.pos.col = @1.first_column;
  } else
    ;
};
%%
int yyerror ( const char *s ) {
  return 1;
}
