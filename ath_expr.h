typedef enum mnemonic_code {
  MNC_CALL = 1,
  MNC_ASGN,
  MNC_ARITH,
  MNC_VAR,
  MNC_VAL,
  MNC_LVALUE,
  MNC_RVALUE,
  END_OF_MNEMONIC_CODE
} MNEMONIC_CODE;

#define NUM_EXPRCONS_PER_ALLOC 256
typedef struct expr_cons {
  ALLOC_NODE_LINKS alloc;
  SRC_POS pos;
  MNEMONIC_CODE mnemonic;
  struct {
    void *pdaugh;
    struct expr_cons *pleft;
    struct expr_cons *pright;    
  } kids;
} EXPR_CONS, *EXPR_CONS_PTR;
