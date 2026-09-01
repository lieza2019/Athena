typedef enum type_code {
  TY_INT = 1,
  TY_CHAR,
  TY_STRING,
  TY_LIST,
  TY_POLY,
  TY_GEN,
  TY_OTHERS,
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
	const char *ident;
	struct type_cons *pnext;
      } var;
      struct type_cons *pgenvars;
    } tyvars;
    struct type_cons *pstuck;
  } type;
  union {
    struct {
      struct type_cons *pty_elem;
    } list;
  } attrs;
#if 0 // *****
  struct expr_cons *pexpr;
#endif
} TYPE_CONS, *TYPE_CONS_PTR;
typedef const struct type_cons TYPE_CONS_C;
typedef struct type_cons const *TYPE_CONS_PTR_C;

typedef TYPE_CONS LIST_CELL;
typedef LIST_CELL *LIST_CELL_PTR;
typedef const LIST_CELL LIST_CELL_C;
typedef LIST_CELL const *LIST_CELL_PTR_C;

typedef enum tycon_mismatch_reason {
  TYCON_WELLTYPED = 1,
  TYCON_ASGN_TYPEMISMATCH,
  TYCON_UNAEXPR_ILLOPERAND,
  END_OF_TYCON_MISMATCH_REASON
} TYCON_MISMATCH_REASON;

#define TYCHK_RESULT_WELLTYPED(res) ((res).reason == TYCON_WELLTYPED)

#define NUM_TYMAPS_PER_ALLOC 256
typedef struct type_mapsto {
  ALLOC_NODE_LINKS alloc;
  const char *ident;
  TYPE_CONS_PTR ptype;
  struct type_mapsto *pnext;
} TYPE_MAPSTO, *TYPE_MAPSTO_PTR;
#define NUM_TYSUBSTS_PER_ALLOC 256
typedef struct type_subst {
  ALLOC_NODE_LINKS alloc;
  struct type_subst *pcomposit;
  TYPE_MAPSTO_PTR pmappings;
} TYPE_SUBST, *TYPE_SUBST_PTR;
#define SUBST_EMPTY(s) (!((s)->pmappings))
