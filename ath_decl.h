typedef struct pvar_attrib {
  SRC_POS pos;
  char *ident;
  TYPE_CODE type;
  union {
    struct {
      TYPE_CONS_PTR init_n;
    } var_int;
    struct {
      TYPE_CONS_PTR init_s;
    } var_str;
    struct {
      TYPE_CONS_PTR init_l;
      TYPE_CONS_PTR pty;
    } var_list;
  } u;
} VAR_ATTRIB, *VAR_ATTRIB_PTR;
typedef const struct var_attrib VAR_ATTRIB_C;
typedef struct var_attrib const *VAR_ATTRIB_PTR_C;

#define NUM_TYELEMS_PER_ALLOC 256
typedef struct type_env_elem {
  ALLOC_NODE_LINKS alloc;
  VAR_ATTRIB_PTR pvar;
  TYPE_CONS_PTR ptype;
  struct type_env_elem *pnext;
} TYENV_ELEM, *TYENV_ELEM_PTR;
#define NUM_TYENVS_PER_ALLOC 256
typedef struct type_env {
  ALLOC_NODE_LINKS alloc;
  TYENV_ELEM_PTR pmappings;
} TYPE_ENV, *TYPE_ENV_PTR;

typedef enum decl_sort {
  DECL_FUN = 1,
  DECL_VAR,
  END_OF_DECL_KIND
} DECL_SORT;

typedef struct declaration {
  char *ident;
  DECL_SORT kind;
  union {
    struct {
#if 1
      VAR_ATTRIB var;
#else
      VAR_ATTRIB_PTR pvar;
#endif
    } variable;
  } u;
  EXPR_CONS_PTR pinit;
} DECLARATION, *DECLARATION_PTR;
typedef const struct declaration DECL_ATTRIB_C;
typedef struct declaration const *DECL_ATTRIB_PTR_C;
