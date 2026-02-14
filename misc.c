#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "athena.h"

char *ath_abort_str[] = {
  "UNASSIGNED",
  "ABORT_MEMLACK",
  "ABORT_CANNOT_CREATE_SCOPE",
  "ABORT_CANNOT_REG_SYNBOL",
  "END_OF_ATH_ABORT"
};

void ath_abort ( const ATH_ABORT reason, SRC_POS_C pos ) {
  const char *pmsg = ath_abort_str[reason];
  assert( (pos.row > 0) && (pos.col > 0) );
  assert( reason < END_OF_ATH_ABORT );
  printf( "(%d, %d): aborted for %s.\n", pos.row, pos.col, pmsg );
  exit( ABORT_CODE );
}
