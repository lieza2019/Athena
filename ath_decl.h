typedef enum type_cons {
  TY_INT = 1,
  TY_STRING,
} TYPE_CONS;

typedef enum decl_kind {
  DECL_FUN = 1,
  DECL_VAR,
  END_OF_DECL_KIND
} DECL_KIND;

typedef struct var_decl {
  SRC_POS pos;
  char *ident;
  TYPE_CONS ty;
  union {
    struct {
      int nat;
      char *str;
    } var_int;
  } u;  
} VAR_DECL, *VAR_DECL_PTR;
typedef struct decl_attrib {
  char *ident;
  DECL_KIND kind;
  union {
    VAR_DECL var;
  } u;
} DECL_ATTRIB, *DECL_ATTRIB_PTR;
