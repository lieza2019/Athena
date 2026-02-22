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
      int n;
    } integer;
    struct {
      char c;
    } character;
    struct {
      char *pstr;
    } string;
    struct {
      struct type_cons *car;
      struct type_cons *cdr;
    } list;
  } u;
  struct type_cons *pnext;
  struct type_cons *pprev;
} TYPE_CONS, *TYPE_CONS_PTR;
typedef const struct type_cons TYPE_CONS_C;
typedef struct type_cons const *TYPE_CONS_PTR_C;

typedef TYPE_CONS LIST_CELL;
typedef LIST_CELL *LIST_CELL_PTR;
typedef const LIST_CELL TYPE_CONS_C;
typedef LIST_CELL const *LIST_CELL_PTR_C;
