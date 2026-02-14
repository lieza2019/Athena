typedef enum type_code {
  TY_INT = 1,
  TY_STRING,
  TY_LIST,
  TY_POLY,
  END_OF_TYPE_CODE
} TYPE_CODE;
extern char *ath_type_name[];

typedef struct type_cons {
  SRC_POS pos;
  TYPE_CODE type;
  union {
    struct {
      struct type_cons *pty_elem;
    } list;
  } u;
} TYPE_CONS, *TYPE_CONS_PTR;
typedef const struct type_cons TYPE_CONS_C;
typedef struct type_cons const *TYPE_CONS_PTR_C;
