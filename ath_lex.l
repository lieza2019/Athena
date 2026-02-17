%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

#include "y.tab.h"

#define MAX_KEYWORD_LEN 16
#define MAX_IDENT_LEN 256

int yywrap( void ) {
  return 1;
}
%}
%%
";" {
  yylloc.first_column += strlen( ";" );
  return TK_SMCL;
}
"\"" {
  yylloc.first_column += strlen( "\"" );
  return TK_DQUOT;
}
"[" {
  yylloc.first_column += strlen( "[" );
  return TK_LSQBL;
}
"]" {
  yylloc.first_column += strlen( "]" );
  return TK_RSQBL;
}
":=" {
  yylloc.first_column += strlen( ":=" );
  return TK_ASGN;
}

"as" {
  yylloc.first_column += strlen( yytext );
  return TK_AS;
}
"int" {
  yylloc.first_column += strlen( yytext );
  return TK_INT;
}
"string" {
  yylloc.first_column += strlen( yytext );
  return TK_STRING;
}
"poly" {
  yylloc.first_column += strlen( yytext );
  return TK_POLY;
}

"-"*[0-9]+ {
  yylloc.first_column += strlen( yytext );
  yylval.nat = atoi( yytext );
  return TK_INT_LITERAL;
}

([a-zA-Z]|"_")([a-zA-Z0-9]|"_")* {
  yylloc.first_column += strlen( yytext );
  yylval.str = find_literal( yytext );
  return TK_IDENT;
}

\".+\" {
  yylloc.first_column += strlen( yytext );
  yylval.str = find_literal( yytext );
  printf( "TK_STR_LITERAL: %s\n", yylval.str );
  return TK_STR_LITERAL;
}

[ \t] { yylloc.first_column++; }
"\n" { yylloc.first_line++; yylloc.first_column = 1; }

. {
  yylloc.first_column++;
  return yytext[0];
}
%%
