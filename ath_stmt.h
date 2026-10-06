#define MAX_STATEMENTS 64

typedef enum stmt_sort {
  STMT_DECL = 1,
  STMT_EXPR,
  END_OF_STMT_SORT
} STMT_SORT;

#define DECLATTR_PAR_ALLOC 256
typedef struct statement {
  SRC_POS pos;
  STMT_SORT sort;
  union {
    DECLARATION_PTR pdecl;
    EXPR_CONS_PTR pexpr;
  } u;
  TYPE_ENV_PTR penv;
  struct statement *psucc;
} STATEMENT, *STATEMENT_PTR;
typedef const struct statement STATEMENT_C;
typedef struct statement *STATEMENT_PTR_C;

typedef struct statements {
  STATEMENT_PTR phead;
  STATEMENT_PTR plast;
} STATEMENTS, *STATEMENTS_PTR;
extern STATEMENTS statements;
