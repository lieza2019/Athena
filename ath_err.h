#if 0 // *****
typedef enum complle_error_fatality {
  COMP_ERROR_WARN1 = 1,
  COMP_ERROR_WARN2,
  COMP_ERROR_FATAL,
  COMP_ERROR_INTERNAL,
  END_OF_COMPILE_ERROR_FATALITY
} COMPILE_ERROR_FATALITY;
#else
extern char *ath_abort_str[];
extern void err_print ( SRC_POS_C pos, COMPILE_ERROR_FATALITY err_lv, ... );
#define err_print(pos, err_lv, ...) {					\
    printf( "(%d, %d): ", SRCPOS_ROW( (pos) ), SRCPOS_COL( (pos) ) );	\
    printf( "[%s] ", ath_abort_str[(err_lv)] );				\
    printf( __VA_ARGS__ );						\
  }
#endif

#define ERRMSG_BUF_CHRTYPE char
#define ERRMSG_BUF_NAME msgbuf
#define ERRMSG_BUF_SIZE 256
extern ERRMSG_BUF_CHRTYPE ERRMSG_BUF_NAME[ERRMSG_BUF_SIZE];

extern void ERRMSG_SUPPRESS ( TYCHK_RESULT_DESC_PTR pres );
#define ERRMSG_SUPPRESS(pres) {			\
  assert( (pres) );				\
  if( (pres)->reason != TYCON_WELLTYPED )	\
    (pres)->suppress = TRUE;			\
  }

extern void ERRMSG_TYCON_MISMATCH ( TYCHK_RESULT_DESC_PTR pres, COMPILE_ERROR_FATALITY err_lv, SRC_POS_C pos );
#define ERRMSG_TYCON_MISMATCH(pres, err_lv, pos) {			\
    assert( (pres) );							\
    print_tycon_mismatch_reason( (ERRMSG_BUF_NAME), (pres) );		\
    if( (!(pres)->suppress) && strnlen( (ERRMSG_BUF_NAME), (ERRMSG_BUF_SIZE) ) > 0 ) { \
      err_print( (pos), (err_lv), (ERRMSG_BUF_NAME) );			\
    }									\
  }
