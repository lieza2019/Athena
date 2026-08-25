#define ERRMSG_BUF_CHRTYPE char
#define ERRMSG_BUF_NAME msgbuf
#define ERRMSG_BUF_SIZE 256
#define ERRMSG_BUF_DECL ERRMSG_BUF_CHRTYPE ERRMSG_BUF_NAME[ERRMSG_BUF_SIZE]

#define ERRMSG_TYCON_MISMATCH(reason, pexpr, pos) {			\
    ERRMSG_BUF_DECL;							\
    print_tycon_mismatch_reason((ERRMSG_BUF_NAME), (reason), (pexpr));	\
    err_print((pos), (ERRMSG_BUF_NAME));				\
  }
