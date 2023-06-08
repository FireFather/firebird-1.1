#define SHIFT 8
#define HISTORY_GOOD(move, depth)                                             \
 { int sv = HISTORY_VALUE (POSITION , move);                                  \
   HISTORY_VALUE (POSITION, move) = sv + (( (0xff00 - sv) * depth) >> SHIFT); \
   if (move != POSITION->DYN->killer1)                                        \
     { POSITION->DYN->killer2 = POSITION->DYN->killer1;                       \
       POSITION->DYN->killer1 = move; } }
#define HISTORY_BAD(move, depth)                                              \
  { int sv = HISTORY_VALUE (POSITION, move);                                  \
    if (POS0->Value > VALUE - 50)                                             \
      HISTORY_VALUE (POSITION, move) = sv - ((sv * depth) >> SHIFT); }
#define HISTORY_BAD1(move, depth)                                             \
  { int sv = HISTORY_VALUE (POSITION, move);                                  \
    if (POS0->Value > ALPHA - 50)                                             \
      HISTORY_VALUE (POSITION, move) = sv - ((sv * depth) >> SHIFT); }
