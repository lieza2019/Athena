#define MAX_STATEMENTS 64

typedef enum stmt_kind {
  STMT_DECL = 1,
  END_OF_STMT_KIND
} STMT_KIND;

typedef struct statement {
  SRC_POS pos;
  STMT_KIND kind;
  union {
    DECL_ATTRIB_PTR pdecl;
  } u;
} STATEMENT, *STATEMENT_PTR;
typedef struct statements{
  int nstmts;
  STATEMENT stmts_buf[MAX_STATEMENTS];  
} STATEMENTS;
extern STATEMENTS statementso;

