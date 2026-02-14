#define BOOL int
#define TRUE 1
#define FALSE 0

#define ABORT_CODE 1

typedef enum ath_abort {
  ABORT_MEMLACK = 1,
  ABORT_CANNOT_REG_SYNBOL,
  END_OF_ATH_ABORT
} ATH_ABORT;

typedef struct src_pos {
  int row;
  int col;
} SRC_POS, *SRC_POS_PTR;
typedef const struct src_pos SRC_POS_C;
typedef struct src_pos const *SRC_POS_PTR_C;

extern void ath_abort ( const ATH_ABORT reason, const int row, const int col );
