#include "ath_misc.h"
#include "ath_mem.h"
#include "ath_type.h"
#include "ath_expr.h"
#include "ath_decl.h"
#include "ath_stmt.h"
#include "ath_symtbl.h"

/* from misc.c */
extern void ath_abort ( SRC_POS_C pos, const ATH_ABORT reason );
extern char *print_value_type ( char *sbuf, TYPE_CONS_PTR_C pvar_tydesc );
extern char *show_var_decl ( char *sbuf, VAR_ATTRIB_PTR pvar_attr );
  
/* from mem.c */
extern void *new_memarea ( int size );
extern ALLOC_NODE_LINKS_PTR alloc_node ( ALLOC_NODE_LINKS_PTR *ppavail, ALLOC_NODE_LINKS_PTR *ppalive, const int node_size, const int nnodes_alloc, SRC_POS_C pos );
extern void free_node ( ALLOC_NODE_LINKS_PTR *ppavail, ALLOC_NODE_LINKS_PTR *ppalive, ALLOC_NODE_LINKS_PTR pnode_freed );

/* from symtbl.c */
extern SYMTBL_SCOPE_PTR enter_scope ( void );
extern SYMTBL_SCOPE_PTR leave_scope ( void );
extern SYMTBL_ENTRY_PTR reg_symbol ( SYMTBL_ENTRY_PTR psym );
extern SYM_ENTITY_PTR find_symbol ( const char *ident );
extern char *find_literal ( char *pname );
  
/* from type.c */
extern TYPE_CONS_PTR asgn_tyvar ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos );
extern TYPE_CONS_PTR alloc_type_cons ( SRC_POS_C pos );
extern void free_type_cons ( TYPE_CONS_PTR ptycons );
extern TYPE_CONS_PTR dup_tydesc ( TYPE_CONS_PTR ptydesc_org, SRC_POS_C pos );
extern TYPE_CONS_PTR gen_tyvers ( TYPE_CONS_PTR pty, char *gen_tyvers[], const int ngenvars, SRC_POS_C pos );
extern TYENV_ELEM_PTR alloc_tyenv_elem ( SRC_POS_C pos );
extern void free_tyenv_elems ( TYENV_ELEM_PTR pelem );
extern TYPE_ENV_PTR alloc_type_env ( SRC_POS_C pos );
extern void free_type_env ( TYPE_ENV_PTR penv );
extern TYPE_ENV_PTR dup_env ( TYPE_ENV_PTR penv_org, SRC_POS_C pos );
extern TYPE_MAPSTO_PTR alloc_type_mapping ( SRC_POS_C pos );
extern void free_type_mapping ( TYPE_MAPSTO_PTR ptymap );
extern TYPE_SUBST_PTR alloc_type_subst ( SRC_POS_C pos );
extern void free_type_subst ( TYPE_SUBST_PTR ptysubst );
extern TYPE_SUBST_PTR dup_subst ( TYPE_SUBST_PTR psub_org, SRC_POS_C pos );
extern TYPE_SUBST_PTR comp_subst ( TYPE_SUBST_PTR psub_1, TYPE_SUBST_PTR psub_2, SRC_POS_C pos );
extern TYPE_CONS_PTR ty_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR pty, SRC_POS_C pos );
extern TYPE_CONS_PTR typematch ( TYPE_SUBST_PTR *ppsubst, TYPE_ENV_PTR penv, STATEMENT_PTR pstmt, SRC_POS_C pos );
extern BOOL typecheck ( TYPE_CONS_PTR_C pty1, TYPE_CONS_PTR_C pty2 );
extern char *print_var_type ( char *sbuf, TYPE_CONS_PTR_C pty_desc );

/* from expr.c */
extern EXPR_CONS_PTR alloc_expr_cons ( SRC_POS_C pos );
extern void free_expr_cons ( EXPR_CONS_PTR pecons );

/* from decl.c */
extern BOOL decl_var ( DECLARATION_PTR *pdecl, VAR_ATTRIB_PTR pvar_attr );
extern void err_redef ( DECLARATION_PTR pdecl );

/* from stmt.c */
extern STATEMENT_PTR new_stmt ( void );
extern BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_ATTRIB_PTR pvar_attr );

/* from lisp.c */
extern LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos );
extern void free_list_cell ( LIST_CELL_PTR pcell );
extern void destroy_list ( LIST_CELL_PTR plist );
extern TYPE_CONS_PTR list_dup ( TYPE_CONS_PTR *ppdup, TYPE_CONS_PTR porg, SRC_POS_C pos );
extern BOOL list_is_nil ( TYPE_CONS_PTR_C pcons_list );
extern LIST_CELL_PTR list_creat_nil( TYPE_CONS_PTR pty, SRC_POS_C pos );
extern LIST_CELL_PTR cons_list ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos );

/* from parse.c */
extern TYPE_CONS_PTR alloc_tycons_node ( SRC_POS_C pos );
extern VAR_ATTRIB_PTR decl_attrib_var ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, void *pty_arg, void *pinit, SRC_POS_C pos );
extern TYPE_CONS_PTR var_list_type ( TYPE_CONS_PTR pl_ty, TYPE_CODE elem_ty, SRC_POS_C pos );
extern LIST_CELL_PTR value_list_elem ( TYPE_CODE elem_ty, void *pelem_val, LIST_CELL_PTR psucc_es, SRC_POS_C pos );
extern LIST_CELL_PTR value_list ( LIST_CELL_PTR plist_elems, LIST_CELL_PTR psucc_ls, SRC_POS_C pos );
