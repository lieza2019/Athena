#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "athena.h"

static struct {
  EXPR_CONS_PTR pavail;
  EXPR_CONS_PTR palive;
} expr_cons_manage;
EXPR_CONS_PTR alloc_expr_cons ( SRC_POS_C pos ) {
  EXPR_CONS_PTR pecons = NULL;
  pecons = (EXPR_CONS_PTR)alloc_node( (ALLOC_NODE_LINKS_PTR *)&expr_cons_manage.pavail, (ALLOC_NODE_LINKS_PTR *)&expr_cons_manage.palive, sizeof(EXPR_CONS), NUM_EXPRCONS_PER_ALLOC, pos );
  return pecons;
}

void free_expr_cons ( EXPR_CONS_PTR pecons ) {
  free_node ( (ALLOC_NODE_LINKS_PTR *)&expr_cons_manage.pavail, (ALLOC_NODE_LINKS_PTR *)&expr_cons_manage.palive, (ALLOC_NODE_LINKS_PTR)pecons );
}
