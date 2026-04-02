#define BOOL int
#define TRUE 1
#define FALSE 0

#define ABORT_CODE 1

typedef enum ath_abort {
  ABORT_MEMLACK = 1,  
  ABORT_CANNOT_CREAT_SCOPE,
  ABORT_CANNOT_CREAT_OBJ,
  ABORT_CANNOT_REG_SYNBOL,
  INTERNALERR_TYPE_CONS,
  END_OF_ATH_ABORT
} ATH_ABORT;

typedef struct src_pos {
  int row;
  int col;
} SRC_POS, *SRC_POS_PTR;
typedef const struct src_pos SRC_POS_C;
typedef struct src_pos const *SRC_POS_PTR_C;

#ifndef ath_printf
#define ath_printf( pos, args_printf ) {		\
  printf( "(%d, %d): ", (pos).row, (pos).col );		\
  printf( "%s.\n", (args_printf) );		\
  }
#endif // ath_printf
