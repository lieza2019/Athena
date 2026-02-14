typedef enum decl_kind {
  DECL_FUN = 1,
  DECL_VAR,
  END_OF_DECL_KIND
} DECL_KIND;

typedef enum type_code {
  TY_INT = 1,
  TY_STRING,
  TY_LIST,
  TY_POLY,
  END_OF_TYPE_CODE
} TYPE_CODE;

typedef struct type_cons {
  TYPE_CODE type;
  union {
    struct {
      struct type_cons *pty_elem;
    } list;
  } u;
} TYPE_CONS, *TYPE_CONS_PTR;
typedef const struct type_cons TYPE_CONS_C;
typedef struct type_cons const *TYPE_CONS_PTR_C;

typedef struct var_decl {
  SRC_POS pos;
  char *ident;
  TYPE_CODE type;
  union {
    struct {
      int init_n;
    } var_int;
    struct {
      char *init_s;
    } var_str;
    struct {
      TYPE_CONS_PTR pty;
    } var_list;
  } u;
} VAR_DECL, *VAR_DECL_PTR;
typedef const struct var_decl VAR_DECL_C;
typedef struct var_decl const *VAR_DECL_PTR_C;

typedef struct decl_attrib {
  char *ident;
  DECL_KIND kind;
  union {
    VAR_DECL var;
  } u;
} DECL_ATTRIB, *DECL_ATTRIB_PTR;
typedef const struct decl_attrib DECL_ATTRIB_C;
typedef struct decl_attrib const *DECL_ATTRIB_PTR_C;
