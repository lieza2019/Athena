typedef struct var_attrib {
  SRC_POS pos;
  char *ident;
  TYPE_CODE type;
  union {
    struct {
      int init_n;
    } var_int;
    struct {
      char const *init_s;
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
  VAR_ATTRIB_PTR pvar;
  TYPE_CONS_PTR ptype;
  struct type_env_elem *pnext;
  struct {
    struct type_env_elem *pnext;
    struct type_env_elem *pprev;
  } alloc;
} TYPE_ENV_ELEM, *TYPE_ENV_ELEM_PTR;
typedef TYPE_ENV_ELEM TYPE_ENV;
typedef TYPE_ENV_ELEM_PTR TYPE_ENV_PTR;

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
      EXPR_CONS_PTR pinival;
    } variable;
  } u;
} DECLARATION, *DECLARATION_PTR;
typedef const struct declaration DECL_ATTRIB_C;
typedef struct declaration const *DECL_ATTRIB_PTR_C;
