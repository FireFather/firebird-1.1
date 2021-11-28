#include "firebird.h"

#define AgeDepthMix(x,y) (((Age - (x)) & (MaxAge - 1)) * MaxDepth   + (MaxDepth - ((y) + 1)))

void IncrementAge()
    {
    Age += 1;

    if( Age == MaxAge )
        Age = 0;
    }

int InitHash( int mb )
    {
	int PawnHashMB;
    Age = 0;
    HashSize = ((1ULL << MSB(mb)) << 20) / sizeof(typeHash);

    if( HashSize > 0x100000000 )
        HashSize = 0x100000000;
    mb = (HashSize * sizeof(typeHash)) >> 20;
    HashMask = HashSize - 4;

    if( FlagHashInit )
        FreeAligned(HashTable);
    FlagHashInit = true;
    MemAlign(HashTable, 64, HashSize * sizeof(typeHash));
    HashClear();

	PawnHashMB = (((mb << 1) - 1) >> 3);
	InitPawnHash(PawnHashMB);
    return mb;
    }

int InitPawnHash( int mb )
    {
    if( mb > 1024 )
        mb = 1024;
    PawnHashSize = ((1 << MSB(mb)) << 20) / sizeof(typePawnEval);
    mb = (PawnHashSize * sizeof(typePawnEval)) >> 20;

    if( PawnHash )
        FreeAligned(PawnHash);
    MemAlign(PawnHash, 64, PawnHashSize * sizeof(typePawnEval));
    PawnHashReset();
    return (mb);
    }

void HashClear()
    {
	int i;
    memset(HashTable, 0, HashSize * sizeof(typeHash));
    memset(PVHashTable, 0, 0x10000 * sizeof(typePVHash));
	for (i = 0; i < HashSize; i++)
		(HashTable + i)->age = (MaxAge / 2);
    Age = 0;
    }

void PawnHashReset()
    {
    memset(PawnHash, 0, PawnHashSize * sizeof(typePawnEval));
    }

void HashLowerAll( typePos *Position, int move, int depth, int Value )
    {
    int DEPTH, i, k = Position->Current->Hash & HashMask;
    typeHash *rank;
    int max = 0, w = 0;
    move &= 0x7fff;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 && (!rank->DepthLower || IsAll(rank))
            && rank->DepthLower <= depth )
            {
            rank->DepthLower = depth;
            rank->move = move;
            rank->ValueUpper = Value;
            rank->age = Age;
            rank->flags |= FlagLower | FlagAll;
            return;
            }
        DEPTH = MAX(rank->DepthLower, rank->DepthUpper);

        if( AgeDepthMix(rank->age, DEPTH) > max )
            {
            max = AgeDepthMix(rank->age, DEPTH);
            w = i;
            }
        }
    rank = HashTable + (k + w);
    rank->hash = (Position->Current->Hash >> 32);
    rank->DepthUpper = 0;
    rank->ValueLower = 0;
    rank->DepthLower = depth;
    rank->move = move;
    rank->ValueUpper = Value;
    rank->age = Age;
    rank->flags = FlagLower | FlagAll;
    return;
    }
void HashUpperCut( typePos *Position, int depth, int Value )
    {
    int DEPTH, i, k = Position->Current->Hash & HashMask;
    typeHash *rank;
    int max = 0, w = 0;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( !(rank->hash ^ (Position->Current->Hash >> 32)) && (!rank->DepthUpper || IsCut(rank))
            && rank->DepthUpper <= depth )
            {
            rank->DepthUpper = depth;
            rank->ValueLower = Value;
            rank->age = Age;
            rank->flags |= FlagUpper | FlagCut;
            return;
            }
        DEPTH = MAX(rank->DepthLower, rank->DepthUpper);

        if( AgeDepthMix(rank->age, DEPTH) > max )
            {
            max = AgeDepthMix(rank->age, DEPTH);
            w = i;
            }
        }
    rank = HashTable + (k + w);
    rank->hash = (Position->Current->Hash >> 32);
    rank->DepthLower = 0;
    rank->move = 0;
    rank->ValueUpper = 0;
    rank->DepthUpper = depth;
    rank->ValueLower = Value;
    rank->age = Age;
    rank->flags = FlagUpper | FlagCut;
    return;
    }
void HashLower( uint64 Z, int move, int depth, int Value )
    {
    int DEPTH, i, k = Z & HashMask;
    typeHash *rank;
    int max = 0, w = 0;
    move &= 0x7fff;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( !(rank->hash ^ (Z >> 32)) && !IsExact(rank) && rank->DepthLower <= depth )
            {
            rank->DepthLower = depth;
            rank->move = move;
            rank->ValueUpper = Value;
            rank->age = Age;
            rank->flags |= FlagLower;
            rank->flags &= ~FlagAll;
            return;
            }
        DEPTH = MAX(rank->DepthLower, rank->DepthUpper);

        if( AgeDepthMix(rank->age, DEPTH) > max )
            {
            max = AgeDepthMix(rank->age, DEPTH);
            w = i;
            }
        }
    rank = HashTable + (k + w);
    rank->hash = (Z >> 32);
    rank->DepthUpper = 0;
    rank->ValueLower = 0;
    rank->DepthLower = depth;
    rank->move = move;
    rank->ValueUpper = Value;
    rank->age = Age;
    rank->flags = FlagLower;
    return;
    }
void HashUpper( uint64 Z, int depth, int Value )
    {
    int DEPTH, i, k = Z & HashMask;
    typeHash *rank;
    int max = 0, w = 0;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( !(rank->hash ^ (Z >> 32)) && !IsExact(rank) && rank->DepthUpper <= depth )
            {
            rank->DepthUpper = depth;
            rank->ValueLower = Value;
            rank->age = Age;
            rank->flags |= FlagUpper;
            rank->flags &= ~FlagCut;
            return;
            }
        DEPTH = MAX(rank->DepthLower, rank->DepthUpper);

        if( AgeDepthMix(rank->age, DEPTH) > max )
            {
            max = AgeDepthMix(rank->age, DEPTH);
            w = i;
            }
        }
    rank = HashTable + (k + w);
    rank->hash = (Z >> 32);
    rank->DepthLower = 0;
    rank->move = 0;
    rank->ValueUpper = 0;
    rank->DepthUpper = depth;
    rank->ValueLower = Value;
    rank->age = Age;
    rank->flags = FlagUpper;
    return;
    }
static void pv_zobrist( typePos *Position, int move, int depth, int Value )
    {
    int i, k = Position->Current->Hash & PVHashMask;
    typePVHash *rank;
    int w = 0, max = 0;

    for ( i = 0; i < 4; i++ )
        {
        rank = PVHashTable + (k + i);

        if( rank->hash == Position->Current->Hash )
            {
            rank->depth = depth;
            rank->Value = Value;
            rank->move = move;
            rank->age = Age;
            return;
            }

        if( AgeDepthMix(rank->age, rank->depth) > max )
            {
            max = AgeDepthMix(rank->age, rank->depth);
            w = i;
            }
        }
    rank = PVHashTable + (k + w);
    rank->hash = Position->Current->Hash;
    rank->depth = depth;
    rank->move = move;
    rank->Value = Value;
    rank->age = Age;
    }

void HashExact( typePos *Position, int move, int depth, int Value, int Flags )
    {
    int DEPTH, i, j, k = Position->Current->Hash & HashMask;
    typeHash *rank;
    int max = 0, w = 0;
    move &= 0x7fff;
    pv_zobrist(Position, move, depth, Value);

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 && MAX(rank->DepthUpper, rank->DepthLower) <= depth )
            {
            rank->DepthUpper = rank->DepthLower = depth;
            rank->move = move;
            rank->ValueLower = rank->ValueUpper = Value;
            rank->age = Age;
            rank->flags = Flags;

            for ( j = i + 1; j < 4; j++ )
                {
                rank = HashTable + (k + j);

                if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0
                    && MAX(rank->DepthUpper, rank->DepthLower) <= depth )
                    {
                    memset(rank, 0, 16);
                    rank->age = Age ^ (MaxAge / 2);
                    }
                }
            return;
            }
        DEPTH = MAX(rank->DepthLower, rank->DepthUpper);

        if( AgeDepthMix(rank->age, DEPTH) > max )
            {
            max = AgeDepthMix(rank->age, DEPTH);
            w = i;
            }
        }
    rank = HashTable + (k + w);
    rank->hash = (Position->Current->Hash >> 32);
    rank->DepthUpper = rank->DepthLower = depth;
    rank->move = move;
    rank->ValueLower = rank->ValueUpper = Value;
    rank->age = Age;
    rank->flags = Flags;
    return;
    }
