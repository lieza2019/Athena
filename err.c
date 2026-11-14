#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "athena.h"

#if 0 // *****
static char *ath_abort_str[] = {
  "UNASSIGNED",
  "COMP_ERROR_WARN1",
  "COMP_ERROR_WARN2",
  "COMP_ERROR_FATAL",
  "COMP_ERROR_INTERNAL",
  "END_OF_COMPILE_ERROR_FATALITY"
};
#else
char *ath_abort_str[] = {
  "UNASSIGNED",
  "COMP_ERROR_WARN1",
  "COMP_ERROR_WARN2",
  "COMP_ERROR_FATAL",
  "COMP_ERROR_INTERNAL",
  "END_OF_COMPILE_ERROR_FATALITY"
};
#endif

ERRMSG_BUF_CHRTYPE ERRMSG_BUF_NAME[ERRMSG_BUF_SIZE];

#if 1 // *****
void ath_abort ( SRC_POS_C pos, const ATH_ABORT reason ) {
  const char *pmsg = ath_abort_str[reason];
  assert( (pos.row > 0) && (pos.col > 0) );
  assert( reason < END_OF_ATH_ABORT );
  printf( "(%d, %d): aborted for %s.\n", pos.row, pos.col, pmsg );
  exit( ABORT_CODE );
}
#endif

#if 0 // *****
void err_print( SRC_POS_C pos, const char *perr_msg ) {
  assert( perr_msg );
  printf( "(%d, %d): ", SRCPOS_ROW(pos), SRCPOS_COL(pos) );
  printf( "%s\n", perr_msg );
}
void err_print ( SRC_POS_C pos, COMPILE_ERROR_FATALITY err_lv, const char *perr_msg ) {
  assert( perr_msg );
  printf( "(%d, %d): ", SRCPOS_ROW(pos), SRCPOS_COL(pos) );
  printf( "[%s] ", ath_abort_str[err_lv] );
  printf( "%s\n", perr_msg );
}
#endif
