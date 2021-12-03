#include <string.h>
#define TempoValue 5
#define TempoValue2 5
#define PrunePawn 160
#define PruneMinor 500
#define PruneRook 800
#define PruneCheck 10
#define LazyValue 150
#define LazyValue2 300
#define EARLY_GAME ((POSITION->DYN->material & 0xff) >= 18)

#ifdef MULTIPLE_POS_GAIN
#define MAX_POSITIONAL(x) \
  ((int) MAX_POSITIONAL_GAIN[POSITION->cpu][POSITION->sq[FROM(x)]][x & 07777])
#else
#define MAX_POSITIONAL(x) \
  ((int) MAX_POSITIONAL_GAIN[POSITION->sq[FROM(x)]][x & 07777])
#endif

#define IsRepetition(x)                                  \
  (VALUE > x && POS0->reversible >= 2 &&                 \
   ((TO(move) << 6) | FROM(move)) == (POS0 - 1)->move && \
   POSITION->sq[TO(move)] == 0)
#define IsInterpose(x) (x & (1 << 15))
#define EasySEE(x) (x & 0x300000)
#define PassedPawnPush(to, x) \
  (POSITION->sq[to] == EnumMyP && x &&   (BitboardOppP & IsPassedPawn[to]) == 0)
#define PIECE_IS_WHITE(pi) (pi <= 7)
#define PIECE_IS_BLACK(pi) (pi >= 8)
#define QSEARCH_CONDITION (new_depth <= 1)
#define LOW_DEPTH_CONDITION (new_depth <= 7)
#define LOW_DEPTH_CONDITION_PV (new_depth <= 7)
