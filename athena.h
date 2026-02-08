#include "ath_misc.h"
#include "ath_mem.h"
#include "ath_decl.h"
#include "ath_stmt.h"
#include "ath_symtbl.h"

extern SYMTBL_SCOPE_PTR enter_scope ( void );
extern SYMTBL_SCOPE_PTR leave_scope ( void );
extern SYMTBL_ENTRY_PTR reg_symbol ( SYMTBL_ENTRY_PTR psym );
extern SYM_ENTITY_PTR find_crnt_scope ( SYMTBL_SCOPE_PTR psco, const char *ident );
extern SYM_ENTITY_PTR find_symbol ( const char *ident );
extern char *find_literal ( char *pname );

extern void err_redef ( DECL_ATTRIB_PTR pdecl_attr, int row, int col );
extern BOOL decl_var ( DECL_ATTRIB_PTR *ppdecl_attr, VAR_DECL_PTR pvar_attr );

extern STATEMENT_PTR new_stmt ( void );
extern BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_DECL_PTR pvar_decl, int row, int col );
