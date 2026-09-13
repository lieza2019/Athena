/* purged, 2026/9/12 */
typedef enum complle_error_fatality {
  COMP_ERROR_WARN1 = 1,
  COMP_ERROR_WARN2,
  COMP_ERROR_FATAL
} COMPILE_ERROR_FATALITY;

#define ERRMSG_BUF_CHRTYPE char
#define ERRMSG_BUF_NAME msgbuf
#define ERRMSG_BUF_SIZE 256
extern ERRMSG_BUF_CHRTYPE ERRMSG_BUF_NAME[ERRMSG_BUF_SIZE];

#define ERRMSG_SUPPRESS(pres) {			\
  assert( (pres) );				\
  if( (pres)->reason != TYCON_WELLTYPED )	\
    (pres)->suppress = TRUE;			\
  }
#if 0 // *****
#define ERRMSG_TYCON_MISMATCH(pres, pos) {				\
    assert( (pres) );							\
    print_tycon_mismatch_reason( (ERRMSG_BUF_NAME), (pres)->reason, (pres)->pe_mismatch, (pres)->nargs ); \
    if( strnlen( (ERRMSG_BUF_NAME), (ERRMSG_BUF_SIZE) ) > 0 ) {		\
      err_print( (pos), (ERRMSG_BUF_NAME) );				\
    }									\
  }
#else
#if 0
#define ERRMSG_TYCON_MISMATCH(pres, pos) {				\
    assert( (pres) );							\
    print_tycon_mismatch_reason( (ERRMSG_BUF_NAME), (pres) );		\
    if( strnlen( (ERRMSG_BUF_NAME), (ERRMSG_BUF_SIZE) ) > 0 ) {		\
      err_print( (pos), (ERRMSG_BUF_NAME) );				\
    }									\
  }
#else
#define ERRMSG_TYCON_MISMATCH(pres, pos) {				\
    assert( (pres) );							\
    print_tycon_mismatch_reason( (ERRMSG_BUF_NAME), (pres) );		\
    if( (!(pres)->suppress) && strnlen( (ERRMSG_BUF_NAME), (ERRMSG_BUF_SIZE) ) > 0 ) { \
      err_print( (pos), (ERRMSG_BUF_NAME) );				\
    }									\
  }
#endif
#endif
