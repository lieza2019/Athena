#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

STATEMENTS statements = { 0, {} };

STATEMENT_PTR new_stmt ( void ) {
  STATEMENT_PTR pr = NULL;
  if( statements.nstmts < MAX_STATEMENTS ) {
    pr = &statements.stmts_buf[statements.nstmts];
    statements.nstmts++;
  }
  return pr;
}
