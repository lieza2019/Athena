#include "ath_misc.h"
#include "ath_mem.h"
#include "ath_type.h"
#include "ath_expr.h"
#include "ath_decl.h"
#include "ath_stmt.h"
#include "ath_symtbl.h"

extern void ath_abort ( SRC_POS_C pos, const ATH_ABORT reason );
extern char *print_value_type ( char *sbuf, TYPE_CONS_PTR_C pvar_desc );
extern char *show_var_decl ( char *sbuf, VAR_ATTRIB_PTR pvar_decl );

extern void *new_memarea ( int size );
extern ALLOC_NODE_LINKS_PTR alloc_node ( ALLOC_NODE_LINKS_PTR *ppavail, ALLOC_NODE_LINKS_PTR *ppalive, const int node_size, const int nnodes_alloc, SRC_POS_C pos );
extern void free_node ( ALLOC_NODE_LINKS_PTR *ppavail, ALLOC_NODE_LINKS_PTR *ppalive, ALLOC_NODE_LINKS_PTR pnode_freed );

extern SYMTBL_SCOPE_PTR enter_scope ( void );
extern SYMTBL_SCOPE_PTR leave_scope ( void );
extern SYMTBL_ENTRY_PTR reg_symbol ( SYMTBL_ENTRY_PTR psym );
extern SYM_ENTITY_PTR find_symbol ( const char *ident );
extern char *find_literal ( char *pname );

extern TYPE_CONS_PTR alloc_type_cons ( SRC_POS_C pos );
extern void free_type_cons ( TYPE_CONS_PTR ptycons );
extern TYPE_ENV_ELEM_PTR alloc_tyenv_elem ( SRC_POS_C pos );
extern void free_tyenv_elems ( TYPE_ENV_ELEM_PTR pelem );
extern char *fresh_tyvar ( SRC_POS_C pos );
extern BOOL typecheck ( TYPE_CONS_PTR_C pty1, TYPE_CONS_PTR_C pty2 );
extern char *print_var_type ( char *sbuf, TYPE_CONS_PTR_C pty_desc );

extern EXPR_CONS_PTR alloc_expr_cons ( SRC_POS_C pos );
extern void free_expr_cons ( EXPR_CONS_PTR pelem );

extern void err_redef ( DECLARATION_PTR pdecl_attr );
extern BOOL decl_var ( DECLARATION_PTR *ppdecl_attr, VAR_ATTRIB_PTR pvar_attr );

extern STATEMENT_PTR new_stmt ( void );
extern BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_ATTRIB_PTR pvar_decl );

extern LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos );
extern void free_list_cell ( LIST_CELL_PTR pcell );
extern void destroy_list ( LIST_CELL_PTR plist );
extern TYPE_CONS_PTR list_dup ( TYPE_CONS_PTR *ppdup, TYPE_CONS_PTR porg, SRC_POS_C pos );
extern BOOL list_is_nil ( TYPE_CONS_PTR_C pcons_list );
extern LIST_CELL_PTR list_creat_nil( TYPE_CONS_PTR pty, SRC_POS_C pos );
extern LIST_CELL_PTR cons_list ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos );

extern TYPE_CONS_PTR alloc_tycons_node ( SRC_POS_C pos );
extern VAR_ATTRIB_PTR decl_attrib_var ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, void *pty_arg, void *pinit, SRC_POS_C pos );
extern TYPE_CONS_PTR var_list_type ( TYPE_CONS_PTR pl_ty, TYPE_CODE elem_ty, SRC_POS_C pos );
extern LIST_CELL_PTR value_list_elem ( TYPE_CODE elem_ty, void *pelem_val, LIST_CELL_PTR psucc_es, SRC_POS_C pos );
extern LIST_CELL_PTR value_list ( LIST_CELL_PTR plist_elems, LIST_CELL_PTR psucc_ls, SRC_POS_C pos );
