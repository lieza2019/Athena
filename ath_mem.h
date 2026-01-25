#define INIT_ALLOC_MEMSIZ (64 * 1024) // in bytes
typedef struct ath_memchunk {
  void *plim;
  void *parena;
  void *pwp;
  struct ath_memchunk *pnext;
} ATH_MEMCHUNK, *ATH_MEMCHUNK_PTR;

extern void *new_memarea ( int size );
