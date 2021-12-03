#include <math.h>
static INLINE void MakeNull( typePOS *POSITION )
    {
    POSITION->nodes++;
    POSITION->DYN->SAVED_FLAGS = POSITION->DYN->flags;
    memcpy(POSITION->DYN + 1, POSITION->DYN, 64);
    POSITION->DYN++;
    POSITION->DYN->HASH ^= ZobristWTM;
    POSITION->wtm ^= 1;
    POSITION->height++;
    POSITION->DYN->reversible++;

    if( POSITION->DYN->ep )
        {
        POSITION->DYN->HASH ^= ZobristEP[POSITION->DYN->ep & 7];
        POSITION->DYN->ep = 0;
        }
    POSITION->DYN->Value = -((POSITION->DYN - 1)->Value + TempoValue);
    POSITION->DYN->PositionalValue = (POSITION->DYN - 1)->PositionalValue;
    POSITION->DYN->lazy = (POSITION->DYN - 1)->lazy;
    POSITION->DYN->flags &= ~3;
    POSITION->DYN->move = 0;
    POSITION->STACK[++(POSITION->StackHeight)] = POSITION->DYN->HASH;
    }
static INLINE void UndoNull( typePOS *POSITION )
    {
    POSITION->DYN--;
    POSITION->StackHeight--;
    POSITION->height--;
    POSITION->wtm ^= 1;
    POSITION->DYN->flags = POSITION->DYN->SAVED_FLAGS;
    }

static __inline int null_new_depth( int DEPTH, int delta )
    {
    double ddelta, r;
    uint64 NODES = 0;
	int cpu, rp;
    for ( cpu = 0; cpu < NUM_THREADS; cpu++ )
		for ( rp = 0; rp < RP_PER_CPU; rp++ )
			NODES += ROOT_POSITION[cpu][rp].nodes;
    ddelta = MIN((double)delta, 225.0);

    if( DEPTH < 34 || NODES <= 5000 * 1000 )
        r = 8 + sqrt(ddelta * DEPTH) / 60.0;

    else if( DEPTH < 46 || NODES <= 200000 * 1000 )
        r = 10 + sqrt(ddelta * DEPTH) / 60.0;

	else
		r = 12 + sqrt(ddelta * DEPTH) / 60.0;

    return (DEPTH - (int)r);
    }

#define NULL_REDUCTION 8
