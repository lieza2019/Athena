typedef enum const_kind {
  CONST_STR = 1,
  CONST_INT,
  END_OF_CONST_KIND
} CONST_KIND;
typedef enum SYM_KIND {
  SYM_CONST = 1,
  SYM_DECL,
  END_OF_SYM_KIND
} SYM_KIND, *SYM_KIND_PTR;

typedef struct sym_entity {
  SYM_KIND kind;
  union {
    DECL_ATTRIB decl;
    struct {
      CONST_KIND kind;
      char *pstr;
      int n;
    } constant;
  } u;
} SYM_ENTITY, *SYM_ENTITY_PTR;
typedef struct symtbl_entry {
  char *ident;
  SYM_ENTITY entity;
  struct symtbl_entry *passoc;
  struct symtbl_entry *pnext;
} SYMTBL_ENTRY, *SYMTBL_ENTRY_PTR;
#define SYMTBL_HASHTBL_ENTRIES 1024
typedef struct symtbl_scope {
  SYMTBL_ENTRY_PTR index[SYMTBL_HASHTBL_ENTRIES];
  struct symtbl_scope *pparent;
  struct symtbl_scope *decend;
  struct {
    struct symtbl_scope *past;;
    struct symtbl_scope *pkids;
  } attic;
} SYMTBL_SCOPE, *SYMTBL_SCOPE_PTR;

typedef struct symtbl {
  SYMTBL_SCOPE_PTR pgnd_scope;
  SYMTBL_SCOPE_PTR pcrnt_scope;
  SYMTBL_SCOPE_PTR pliteral;
} SYMTBL, *SYMTBL_PTR;
extern SYMTBL symtbl;
