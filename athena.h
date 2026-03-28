#include "ath_misc.h"
#include "ath_mem.h"
#include "ath_type.h"
#include "ath_decl.h"
#include "ath_stmt.h"
#include "ath_symtbl.h"

extern void ath_abort ( SRC_POS_C pos, const ATH_ABORT reason );

extern void *new_memarea ( int size );

extern SYMTBL_SCOPE_PTR enter_scope ( void );
extern SYMTBL_SCOPE_PTR leave_scope ( void );
extern SYMTBL_ENTRY_PTR reg_symbol ( SYMTBL_ENTRY_PTR psym );
extern SYM_ENTITY_PTR find_symbol ( const char *ident );
extern char *find_literal ( char *pname );

extern char *show_var_type ( char *sbuf, VAR_DECL_PTR pvar_decl );

extern void err_redef ( DECL_ATTRIB_PTR pdecl_attr );
extern BOOL decl_var ( DECL_ATTRIB_PTR *ppdecl_attr, VAR_DECL_PTR pvar_attr );

extern STATEMENT_PTR new_stmt ( void );
extern BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_DECL_PTR pvar_decl );

extern LIST_CELL_PTR alloc_list_cell ( SRC_POS_C pos );
extern void free_list_cell ( LIST_CELL_PTR pcell );
extern BOOL list_is_nil ( TYPE_CONS_PTR_C pcons_list );
extern LIST_CELL_PTR creat_nil_list ( SRC_POS_C pos );
extern LIST_CELL_PTR cons_list ( LIST_CELL_PTR plist, TYPE_CODE cons_ty, SRC_POS_C pos );
extern LIST_CELL_PTR cons_list2 ( LIST_CELL_PTR plist, TYPE_CONS_PTR pcons_ty, SRC_POS_C pos );

extern void poly_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, SRC_POS_C pos );
extern void int_var_attrib ( VAR_DECL_PTR pvar_attr, char *pident, const int n_init, SRC_POS_C pos );
extern void string_var_attrib ( VAR_DECL_PTR pvar_attr, char *pvar_name, const char *s_init, SRC_POS_C pos );
