#include "ath_misc.h"
#include "ath_mem.h"
#include "ath_type.h"
#include "ath_decl.h"
#include "ath_expr.h"
//#include "ath_decl.h"
#include "ath_stmt.h"
#include "ath_symtbl.h"

#ifndef RUNTIME_CONSITENCY_CHECK
#define RUNTIME_CONSITENCY_CHECK
#endif

/* from misc.c */
extern void ath_abort ( SRC_POS_C pos, const ATH_ABORT reason );
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
extern const char *find_literal ( const char *pname, SRC_POS_C pos );

/* from type.c */
extern const char *fresh_tyvar ( SRC_POS_C pos );
extern TYPE_CONS_PTR ty_curve ( TYPE_CONS_PTR pty_cons, SRC_POS_C pos );
extern TYPE_CONS_PTR alloc_type_cons ( SRC_POS_C pos );
extern void free_type_cons ( TYPE_CONS_PTR ptycons );
extern TYPE_CONS_PTR dup_tydesc ( TYPE_CONS_PTR ptydesc_org, SRC_POS_C pos );
extern int enum_gentyvars ( TYPE_CONS_PTR *ppgen_tyvs, TYPE_CONS_PTR pty, SRC_POS_C pos );
extern TYPE_CONS_PTR gen_tyvars ( TYPE_CONS_PTR pty, TYPE_CONS_PTR pgen_tyvs, SRC_POS_C pos );
extern TYPE_MAPSTO_PTR alloc_type_mapping ( SRC_POS_C pos );
extern void free_type_mapping ( TYPE_MAPSTO_PTR ptymap );
extern TYPE_SUBST_PTR alloc_type_subst ( SRC_POS_C pos );
extern void free_type_subst ( TYPE_SUBST_PTR ptysubst );
extern TYPE_CONS_PTR subst_map ( TYPE_SUBST_PTR psubst, const char *tyv_ident, SRC_POS_C pos );
extern TYPE_SUBST_PTR subst_add ( TYPE_SUBST_PTR psubst, const char *tyv_ident, TYPE_CONS_PTR pty, SRC_POS_C pos );
extern TYPE_SUBST_PTR dup_subst ( TYPE_SUBST_PTR psubst_org, SRC_POS_C pos );
extern TYPE_SUBST_PTR comp_subst ( TYPE_SUBST_PTR psubst_1, TYPE_SUBST_PTR psubst_2, SRC_POS_C pos );
extern TYPE_SUBST_PTR restr_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR tyvers_omit, SRC_POS_C pos );
extern TYPE_CONS_PTR ty_subst ( TYPE_SUBST_PTR psubst, TYPE_CONS_PTR pty, SRC_POS_C pos );
extern TYENV_ELEM_PTR alloc_tyenv_elem ( SRC_POS_C pos );
extern void free_tyenv_elems ( TYENV_ELEM_PTR pelem );
extern TYPE_ENV_PTR alloc_type_env ( SRC_POS_C pos );
extern void free_type_env ( TYPE_ENV_PTR penv );
extern TYPE_ENV_PTR env_lnk ( TYPE_ENV_PTR penv_pred, TYPE_ENV_PTR penv_succ );
extern TYPE_ENV_PTR env_rid ( TYPE_ENV_PTR penv, const char *var_ident );
extern TYPE_ENV_PTR env_add ( TYPE_ENV_PTR penv, const char *var_ident, TYPE_CONS_PTR pty, SRC_POS_C pos );
extern TYENV_ELEM_PTR env_lkup ( TYPE_ENV_PTR penv, const char *var_ident );
extern TYPE_ENV_PTR dup_env ( TYPE_ENV_PTR penv_org, SRC_POS_C pos );
extern TYPE_ENV_PTR env_subst ( TYPE_ENV_PTR penv, TYPE_SUBST_PTR psubst, SRC_POS_C pos );
extern char *print_type ( char *sbuf, TYPE_CONS_PTR_C pty_desc );

/* from tychk.c */
extern TYPE_CONS_PTR gen_tvs ( TYPE_ENV_PTR penv, TYPE_CONS_PTR pty, SRC_POS_C pos );
extern TYPE_CONS_PTR inst_gtvs ( TYPE_CONS_PTR pty, SRC_POS_C pos );
extern BOOL ty_unify ( TYPE_SUBST_PTR *pps_unif, TYPE_CONS_PTR pty_1, TYPE_CONS_PTR pty_2, SRC_POS_C pos );
extern TYPE_CONS_PTR typecheck ( STATEMENT_PTR pstmt, SRC_POS_C pos );

/* from expr.c */
extern EXPR_CONS_PTR alloc_expr_cons ( SRC_POS_C pos );
extern void free_expr_cons ( EXPR_CONS_PTR pecons );

/* from decl.c */
extern void err_redef ( DECLARATION_PTR pdecl );
extern VAR_ATTRIB_PTR alloc_var_attr ( SRC_POS_C pos );
extern void free_var_addr ( VAR_ATTRIB_PTR pvattr );
extern BOOL decl_var ( DECLARATION_PTR *pdecl, VAR_ATTRIB_PTR pvar_attr, SRC_POS_C pos );

/* from stmt.c */
extern STATEMENT_PTR new_stmt ( void );
extern BOOL stmt_decl_var ( STATEMENT_PTR *ppstmt, VAR_ATTRIB_PTR pvar_attr, SRC_POS_C pos );

/* from lisp.c */

/* from par_decl.c */
extern VAR_ATTRIB_PTR decl_var_attrib ( VAR_ATTRIB_PTR pvar_attr, char *pvar_name, TYPE_CODE var_type, TYPE_CONS_PTR type_arg, EXPR_CONS_PTR pinit, SRC_POS_C pos );
extern TYPE_CONS_PTR var_list_type ( TYPE_CONS_PTR pty_elem, TYPE_CODE elem_type, SRC_POS_C pos );
extern EXPR_CONS_PTR value_list_elem ( TYPE_CODE elem_ty, void *pelem_val, EXPR_CONS_PTR psucc_cs, SRC_POS_C pos );

/* from par_tychk.c */
TYPE_CONS_PTR tychk_decl_var ( STATEMENT_PTR pstmt, SRC_POS_C pos );
