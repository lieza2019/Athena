#define ERRMSG_BUF_CHRTYPE char
#define ERRMSG_BUF_NAME msgbuf
#define ERRMSG_BUF_SIZE 256
extern ERRMSG_BUF_CHRTYPE ERRMSG_BUF_NAME[ERRMSG_BUF_SIZE];

#if 0 // *****
#define ERRMSG_TYCON_MISMATCH(reason, pexpr, pos) {			\
    print_tycon_mismatch_reason((ERRMSG_BUF_NAME), (reason), (pexpr));	\
    if( strnlen( (ERRMSG_BUF_NAME), (ERRMSG_BUF_SIZE) ) > 0 ) {		\
      err_print((pos), (ERRMSG_BUF_NAME));				\
    }									\
  }
#else
#if 0 // *****
#define ERRMSG_TYCON_MISMATCH(pres, pos) {				\
    assert( (pres) );							\
    print_tycon_mismatch_reason( (ERRMSG_BUF_NAME), (pres)->reason, (pres)->pty_mismatch, (pres)->nargs ); \
    if( strnlen( (ERRMSG_BUF_NAME), (ERRMSG_BUF_SIZE) ) > 0 ) {		\
      err_print( (pos), (ERRMSG_BUF_NAME) );				\
    }									\
  }
#else
#define ERRMSG_TYCON_MISMATCH(pres, pos) {				\
    assert( (pres) );							\
    print_tycon_mismatch_reason( (ERRMSG_BUF_NAME), (pres)->reason, (pres)->pe_mismatch, (pres)->nargs ); \
    if( strnlen( (ERRMSG_BUF_NAME), (ERRMSG_BUF_SIZE) ) > 0 ) {		\
      err_print( (pos), (ERRMSG_BUF_NAME) );				\
    }									\
  }
#endif
#endif
