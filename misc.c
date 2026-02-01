#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "athena.h"

static char *ath_abort_str[] = {  
  "ABORT_MEMLACK",
  "ABORT_CANNOT_REG_SYNBOL",
  "END_OF_ATH_ABORT"
};
void ath_abort ( const ATH_ABORT reason, const int row, const int col ) {
  const char *pmsg = ath_abort_str[reason];
  assert( (row > 0) && (col > 0) );
  assert( reason < END_OF_ATH_ABORT );
  printf( "(%d, %d): aborted for %s.\n", row, col, pmsg );
  exit( ABORT_CODE );
}
