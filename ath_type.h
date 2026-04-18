typedef enum type_code {
  TY_INT = 1,
  TY_CHAR,
  TY_STRING,
  TY_LIST,
  TY_POLY,
  END_OF_TYPE_CODE
} TYPE_CODE;
extern char *ath_type_name[];

#define NUM_TYCONS_PER_ALLOC 256
typedef struct type_cons {
  ALLOC_NODE_LINKS alloc;
  SRC_POS pos;
  struct {
    TYPE_CODE ty;
    struct {
      struct {
	char *ident;
	struct type_cons *pnext;
      } var;
      struct type_cons *bvars;
      struct type_cons *pbound;
    } tyvars;
    struct type_cons *pattic;
  } type;
  union {
    struct {
      struct {
	int n;
      } integer;
      struct {
	char c;
      } character;
      struct {
	char *ps;
      } string;
    } literal;
    struct {
      struct type_cons *pty_elem;
      struct type_cons *car;
      struct type_cons *cdr;
      struct type_cons *plast;
    } list;
  } u;
} TYPE_CONS, *TYPE_CONS_PTR;
typedef const struct type_cons TYPE_CONS_C;
typedef struct type_cons const *TYPE_CONS_PTR_C;

typedef TYPE_CONS LIST_CELL;
typedef LIST_CELL *LIST_CELL_PTR;
typedef const LIST_CELL LIST_CELL_C;
typedef LIST_CELL const *LIST_CELL_PTR_C;

typedef struct type_subst {
  char *ident;
  TYPE_CONS_PTR pty;
} TYPE_SUBST, *TYPE_SUBST_PTR;
