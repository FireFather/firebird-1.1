uint32 ROOT_BEST_MOVE;
int ROOT_SCORE, ROOT_PREVIOUS;
int ROOT_DEPTH, PREVIOUS_DEPTH, PREVIOUS_FAST;

#include <setjmp.h>
#include <time.h>

extern jmp_buf J;
boolean JUMP_IS_SET;
boolean EASY_MOVE, BAD_MOVE, BATTLE_MOVE;
uint64 START_CLOCK, CPU_TIME;
sint64 DESIRED_TIME;
boolean ANALYSING;
boolean init_flag;
