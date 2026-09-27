#define INIT_ALLOC_MEMSIZ (64 * 1024) // in bytes

typedef struct ath_memchunk {
  void *plim;
  void *parena;
  void *pwp;
  struct ath_memchunk *pnext;
} ATH_MEMCHUNK, *ATH_MEMCHUNK_PTR;
typedef const struct ath_memchunk ATH_MEMCHUNK_C;
typedef struct ath_memchunk const *ATH_MEMCHUNK_PTR_C;

typedef enum {
  ALLOC_TYPE_CONS,
  END_OF_ALLOC_NODE
} ALLOC_NODE_KIND;
typedef struct alloc_node_links {
  ALLOC_NODE_KIND kind;
  struct alloc_node_links *pnext;
  struct alloc_node_links *pprev;
} ALLOC_NODE_LINKS, *ALLOC_NODE_LINKS_PTR;
