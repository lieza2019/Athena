typedef enum mnemonic_code {
  MNC_CALL = 1,
  MNC_ASGN,
#if 0 // NOW OBSOLETE
  MNC_ARITH,
#endif
  MNC_LVALUE,
  MNC_RVALUE,
  MNC_DECL,
  MNC_INCL,
  MNC_LIST,
  MNC_CONST,
  END_OF_MNEMONIC_CODE
} MNEMONIC_CODE;

#define NUM_EXPRCONS_PER_ALLOC 256
typedef struct expr_cons {
  ALLOC_NODE_LINKS alloc;
  SRC_POS pos;
  MNEMONIC_CODE mnemonic;
  struct {
    struct expr_cons *pleft;
    struct expr_cons *pright;
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
      union {
#if 1 // *****
	VAR_ATTRIB var; // with MNC_LVALUE
#else	
	VAR_ATTRIB_PTR pvar; // comming from t of environment's mapping s.t.v -> t.
#endif
      } refaddr;
    } body;
  } kids;
  TYPE_CONS_PTR ptype;
} EXPR_CONS, *EXPR_CONS_PTR;
typedef const struct expr_cons EXPR_CONS_C;
typedef struct expr_cons const *EXPR_CONS_PTR_C;

#define EXAM_ASGN_EXPR( e )						\
  (((e)->mnemonic == MNC_ASGN) && (((e)->kids.pleft) && ((e)->kids.pright)))
#define EXAM_LVALUE_EXPR( e )						\
  ( (((e)->mnemonic == MNC_LVALUE) && ((!((e)->kids.pleft)) && (!((e)->kids.pright)))) \
    && (e)->kids.body.refaddr.var.ident )
#define EXAM_RVALUE_EXPR( e )						\
  ( (((e)->mnemonic == MNC_RVALUE) && ((!((e)->kids.pleft)) && (!((e)->kids.pright)))) \
    && (e)->kids.body.refaddr.var.ident )
#define EXAM_DECL_EXPR( e )						\
  ( ((e)->mnemonic == MNC_DECL) && (((e)->kids.pleft) && !((e)->kids.pright)) )
#define EXAM_INCL_EXPR( e )						\
  ( ((e)->mnemonic == MNC_INCL) && (((e)->kids.pleft) && !((e)->kids.pright)) )
#if 0 // *****
#define EXAM_CONST_EXPR( e )						\
  (((e)->mnemonic == MNC_CONST) && ((!((e)->kids.pleft)) && (!((e)->kids.pright))))
#else
#define EXAM_CONST_EXPR( e )						\
  ( (((e)->mnemonic == MNC_CONST) && ((!((e)->kids.pleft)) && (!((e)->kids.pright)))) \
    && ((e)->ptype) )
#endif
