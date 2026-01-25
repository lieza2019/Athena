%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

"AS" {
  yylloc.first_column += strlen( yytext );
  return TK_AS;
}

"INT" {
  yylloc.first_column += strlen( yytext );
  return TK_INT;
}

([alpha]|'_')([alnum]|'_')* {
  yylloc.first_column += strlen( yytext );
  //strncpy( yylval.str, yytext, MAX_IDENT_LEN );
  yylval.str = yytext;
  return TK_IDENT;
}
. {
  yylloc.first_column++;
  return yytext[0];
}
%%
