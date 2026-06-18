typedef enum mnemonic_code {
  MNC_CALL = 1,
  MNC_ASGN,
  MNC_ARITH,
  MNC_CONST,
  MNC_LVALUE,
  MNC_RVALUE,
  
  MNC_CNST_INT,
  MNC_CNST_CHAR,
  MNC_CNST_STR,
  MNC_LIST,
  END_OF_MNEMONIC_CODE
} MNEMONIC_CODE;

#define NUM_EXPRCONS_PER_ALLOC 256
#if 0
typedef struct expr_cons {
  ALLOC_NODE_LINKS alloc;
  SRC_POS pos;
  MNEMONIC_CODE mnemonic;
  struct {
    void *pdaugh;
    struct expr_cons *pleft;
    struct expr_cons *pright;
  } kids;
  TYPE_CONS_PTR ptype;
} EXPR_CONS, *EXPR_CONS_PTR;
#else
typedef struct expr_cons {
  ALLOC_NODE_LINKS alloc;
  SRC_POS pos;
  MNEMONIC_CODE mnemonic;
  struct {
    struct expr_cons *pleft;
    struct expr_cons *pright;
    void *pdaugh; // !!!!!, NOW OBSOLETE.
    union {
      struct {
	struct {
	  int n;
	} integer;
	struct {
	  char c;
	} character;
	struct {
	  const char *s;
	} string;
      } literal;
      struct {
	struct expr_cons *car;
	struct expr_cons *cdr;
	struct expr_cons *plast;
      } list;
    } body;
  } kids;
  TYPE_CONS_PTR ptype;
} EXPR_CONS, *EXPR_CONS_PTR;
typedef const struct expr_cons EXPR_CONS_C;
typedef struct expr_cons const *EXPR_CONS_PTR_C;

#define EXAM_ASGN_EXPR( e )						\
  (((e)->mnemonic == MNC_ASGN) &&					\
   ((!((e)->kids.pdaugh)) && ((e)->kids.pleft) && ((e)->kids.pright))	\
   )
#define EXAM_CONST_EXPR( e )						\
  (((e)->mnemonic == MNC_CONST) &&					\
   (((e)->kids.pdaugh) && (!((e)->kids.pleft)) && (!((e)->kids.pright))) \
   )
#define EXAM_LVALUE_EXPR( e )						\
  (((e)->mnemonic == MNC_LVALUE) &&					\
   (((e)->kids.pdaugh) && (!((e)->kids.pleft)) && (!((e)->kids.pright))) \
   )
#endif
