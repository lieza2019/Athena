#define NUM_VARATTR_VAR_PAR_ALLOC 256
typedef struct var_attrib {
  ALLOC_NODE_LINKS alloc;
  SRC_POS pos;
  const char *ident;
  TYPE_CONS_PTR ptype;
#if 1 // *****
  union {
    struct {
      struct expr_cons *pdecl_init;
    } var_decl;
    struct {
      struct var_attrib *parg_head;
      struct var_attrib *parg_next;
    } proc_args;
  } opts;
#else
  struct {
    struct {
      struct expr_cons *pdecl_init;
    } var_decl;
    struct {
      struct var_attrib *parg_head;
      struct var_attrib *parg_next;
    } proc_args;
  } opts;
#endif
} VAR_ATTRIB, *VAR_ATTRIB_PTR;
typedef const struct var_attrib VAR_ATTRIB_C;
typedef struct var_attrib const *VAR_ATTRIB_PTR_C;

#define NUM_PROCATTR_VAR_PAR_ALLOC 256
typedef struct proc_attrib {
  ALLOC_NODE_LINKS alloc;
  SRC_POS pos;
  const char *ident;
  TYPE_CONS_PTR ptype;
  VAR_ATTRIB_PTR pargs;
  struct statement *pstmts;
} PROC_ATTRIB, *PROC_ATTRIB_PTR;

#define NUM_TYELEMS_PER_ALLOC 256
typedef struct type_env_elem {
  ALLOC_NODE_LINKS alloc;
  union {
    struct {
      VAR_ATTRIB v;
      VAR_ATTRIB_PTR plnk_symtbl;
    } var;
  } decl;
  struct type_env_elem *pnext;
} TYENV_ELEM, *TYENV_ELEM_PTR;
#define NUM_TYENVS_PER_ALLOC 256
typedef struct type_env {
  ALLOC_NODE_LINKS alloc;
  TYENV_ELEM_PTR pmappings;
  struct type_env *uplink;
  struct type_env *dnlink;
} TYPE_ENV, *TYPE_ENV_PTR;

#define NUM_DECLATTR_PAR_ALLOC 256
typedef enum decl_sort {
  DECL_PROC = 1,
  DECL_VAR,
  END_OF_DECL_KIND
} DECL_SORT;
typedef struct declaration {
  SRC_POS pos;
  const char *ident;
  DECL_SORT kind;
  union {
    struct {
      VAR_ATTRIB_PTR pvar;
    } variable;
    struct {
      PROC_ATTRIB_PTR pproc;
    } procedure;
  } u;
  struct declaration *pnext;
} DECLARATION, *DECLARATION_PTR;
typedef const struct declaration DECL_ATTRIB_C;
typedef struct declaration const *DECL_ATTRIB_PTR_C;

typedef struct declarations {
  DECLARATION_PTR phead;
  DECLARATION_PTR plast;
} DECLARATIONS, *DECLARATIONS_PTR;
extern DECLARATIONS declarations;
