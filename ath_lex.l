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
  return SMCL;
}

"as" {
  yylloc.first_column += strlen( yytext );
  return TK_AS;
}

"int" {
  yylloc.first_column += strlen( yytext );
  return TK_INT;
}

([a-zA-Z]|"_")([a-zA-Z0-9]|"_")* {  
  yylloc.first_column += strlen( yytext );
  //strncpy( yylval.str, yytext, MAX_IDENT_LEN );
  yylval.str = yytext;
  return TK_IDENT;
}

[ \t] { yylloc.first_column++; }
"\n" { yylloc.first_line++; yylloc.first_column = 1; }

. {
  yylloc.first_column++;
  return yytext[0];
}
%%
