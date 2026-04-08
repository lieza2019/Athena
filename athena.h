#include "ath_misc.h"
#include "ath_mem.h"
#include "ath_type.h"
#include "ath_decl.h"
#include "ath_stmt.h"
#include "ath_symtbl.h"

extern void ath_abort ( SRC_POS_C pos, const ATH_ABORT reason );
extern char *print_value_type ( char *sbuf, TYPE_CONS_PTR_C pvar_desc );
extern char *show_var_decl ( char *sbuf, VAR_DECL_PTR pvar_decl );

extern void *new_memarea ( int size );

extern SYMTBL_SCOPE_PTR enter_scope ( void );
extern SYMTBL_SCOPE_PTR leave_scope ( void );
extern SYMTBL_ENTRY_PTR reg_symbol ( SYMTBL_ENTRY_PTR psym );
extern SYM_ENTITY_PTR find_symbol ( const char *ident );
extern char *find_literal ( char *pname );

extern char *print_var_type ( char *sbuf, TYPE_CONS_PTR_C pty_desc );
extern BOOL typecheck ( TYPE_CONS_PTR_C pty1, TYPE_CONS_PTR_C pty2 );

extern void err_redef ( DECL_ATTRIB_PTR pdecl_attr );
extern BOOL decl_var ( DECL_ATTRIB_PTR *ppdecl_attr, VAR_DECL_PTR pvar_attr );

extern STATEMENT_PTR new_stmt ( void );
extern BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_DECL_PTR pvar_decl );

extern LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos );
extern void free_list_cell ( LIST_CELL_PTR pcell );
extern void destroy_list ( LIST_CELL_PTR plist );
extern TYPE_CONS_PTR list_dup ( TYPE_CONS_PTR *ppdup, TYPE_CONS_PTR porg, SRC_POS_C pos );
extern BOOL list_is_nil ( TYPE_CONS_PTR_C pcons_list );
extern LIST_CELL_PTR list_creat_nil( TYPE_CONS_PTR pty, SRC_POS_C pos );
extern LIST_CELL_PTR cons_list ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos );

extern TYPE_CONS_PTR alloc_tycons_node ( SRC_POS_C pos );
extern VAR_DECL_PTR decl_attrib_var ( VAR_DECL_PTR pvar_attr, char *pvar_name, void *pty_arg, void *pinit, SRC_POS_C pos );
extern TYPE_CONS_PTR var_list_type ( TYPE_CONS_PTR pl_ty, TYPE_CODE elem_ty, SRC_POS_C pos );
extern LIST_CELL_PTR value_list_elem ( TYPE_CODE elem_ty, void *pelem_val, LIST_CELL_PTR psucc_es, SRC_POS_C pos );
