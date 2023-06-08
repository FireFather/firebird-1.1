#include "firebird.h"
#include <string.h>

#define MAX_AGE 256
#define MAX_DEPTH 256

#define AGE_DEPTH_MIX(x,y) \
	(((AGE - (x)) & (MAX_AGE - 1)) * MAX_DEPTH   + (MAX_DEPTH - ((y) + 1)))

void IncrementAge()
    {
    AGE += 1;

    if( AGE == MAX_AGE )
        AGE = 0;
    }

static uint64 HASH_SIZE = 0x400000;
static boolean FLAG_HASH_INIT = 0;
void HashClear()
    {
	int i;
    memset(HashTable, 0, HASH_SIZE * sizeof(typeHash));
    memset(PVHashTable, 0, 0x10000 * sizeof(typePVHash));
	for (i = 0; i < HASH_SIZE; i++)
		(HashTable + i)->age = (MAX_AGE / 2);
    AGE = 0;
    }
int InitHash( int mb )
    {
    AGE = 0;
    HASH_SIZE = ((1ULL << MSB(mb)) << 20) / sizeof(typeHash);

    if( HASH_SIZE > 0x100000000 )
        HASH_SIZE = 0x100000000;
    mb = (HASH_SIZE * sizeof(typeHash)) >> 20;
    HashMask = HASH_SIZE - 4;

    if( FLAG_HASH_INIT )
        ALIGNED_FREE(HashTable);
    FLAG_HASH_INIT = true;
    MEMALIGN(HashTable, 64, HASH_SIZE * sizeof(typeHash));
    HashClear();
    return mb;
    }
void HashLowerALL( typePOS *POSITION, int move, int depth, int Value )
    {
    int DEPTH, i, k = POSITION->DYN->HASH & HashMask;
    typeHash *trans;
    int max = 0, w = 0;
    move &= 0x7fff;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 && (!trans->DepthLower || IsALL(trans))
            && trans->DepthLower <= depth )
            {
            trans->DepthLower = depth;
            trans->move = move;
            trans->ValueUpper = Value;
            trans->age = AGE;
            trans->flags |= FLAG_LOWER | FLAG_ALL;
            return;
            }
        DEPTH = MAX(trans->DepthLower, trans->DepthUpper);

        if( AGE_DEPTH_MIX(trans->age, DEPTH) > max )
            {
            max = AGE_DEPTH_MIX(trans->age, DEPTH);
            w = i;
            }
        }
    trans = HashTable + (k + w);
    trans->hash = (POSITION->DYN->HASH >> 32);
    trans->DepthUpper = 0;
    trans->ValueLower = 0;
    trans->DepthLower = depth;
    trans->move = move;
    trans->ValueUpper = Value;
    trans->age = AGE;
    trans->flags = FLAG_LOWER | FLAG_ALL;
    return;
    }
void HashUpperCUT( typePOS *POSITION, int depth, int Value )
    {
    int DEPTH, i, k = POSITION->DYN->HASH & HashMask;
    typeHash *trans;
    int max = 0, w = 0;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( !(trans->hash ^ (POSITION->DYN->HASH >> 32)) && (!trans->DepthUpper || IsCUT(trans))
            && trans->DepthUpper <= depth )
            {
            trans->DepthUpper = depth;
            trans->ValueLower = Value;
            trans->age = AGE;
            trans->flags |= FLAG_UPPER | FLAG_CUT;
            return;
            }
        DEPTH = MAX(trans->DepthLower, trans->DepthUpper);

        if( AGE_DEPTH_MIX(trans->age, DEPTH) > max )
            {
            max = AGE_DEPTH_MIX(trans->age, DEPTH);
            w = i;
            }
        }
    trans = HashTable + (k + w);
    trans->hash = (POSITION->DYN->HASH >> 32);
    trans->DepthLower = 0;
    trans->move = 0;
    trans->ValueUpper = 0;
    trans->DepthUpper = depth;
    trans->ValueLower = Value;
    trans->age = AGE;
    trans->flags = FLAG_UPPER | FLAG_CUT;
    return;
    }
void HashLower( uint64 Z, int move, int depth, int Value )
    {
    int DEPTH, i, k = Z & HashMask;
    typeHash *trans;
    int max = 0, w = 0;
    move &= 0x7fff;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( !(trans->hash ^ (Z >> 32)) && !IsExact(trans) && trans->DepthLower <= depth )
            {
            trans->DepthLower = depth;
            trans->move = move;
            trans->ValueUpper = Value;
            trans->age = AGE;
            trans->flags |= FLAG_LOWER;
            trans->flags &= ~FLAG_ALL;
            return;
            }
        DEPTH = MAX(trans->DepthLower, trans->DepthUpper);

        if( AGE_DEPTH_MIX(trans->age, DEPTH) > max )
            {
            max = AGE_DEPTH_MIX(trans->age, DEPTH);
            w = i;
            }
        }
    trans = HashTable + (k + w);
    trans->hash = (Z >> 32);
    trans->DepthUpper = 0;
    trans->ValueLower = 0;
    trans->DepthLower = depth;
    trans->move = move;
    trans->ValueUpper = Value;
    trans->age = AGE;
    trans->flags = FLAG_LOWER;
    return;
    }
void HashUpper( uint64 Z, int depth, int Value )
    {
    int DEPTH, i, k = Z & HashMask;
    typeHash *trans;
    int max = 0, w = 0;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( !(trans->hash ^ (Z >> 32)) && !IsExact(trans) && trans->DepthUpper <= depth )
            {
            trans->DepthUpper = depth;
            trans->ValueLower = Value;
            trans->age = AGE;
            trans->flags |= FLAG_UPPER;
            trans->flags &= ~FLAG_CUT;
            return;
            }
        DEPTH = MAX(trans->DepthLower, trans->DepthUpper);

        if( AGE_DEPTH_MIX(trans->age, DEPTH) > max )
            {
            max = AGE_DEPTH_MIX(trans->age, DEPTH);
            w = i;
            }
        }
    trans = HashTable + (k + w);
    trans->hash = (Z >> 32);
    trans->DepthLower = 0;
    trans->move = 0;
    trans->ValueUpper = 0;
    trans->DepthUpper = depth;
    trans->ValueLower = Value;
    trans->age = AGE;
    trans->flags = FLAG_UPPER;
    return;
    }
static void pv_zobrist( typePOS *POSITION, int move, int depth, int Value )
    {
    int i, k = POSITION->DYN->HASH & PVHashMask;
    typePVHash *trans;
    int w = 0, max = 0;

    for ( i = 0; i < 4; i++ )
        {
        trans = PVHashTable + (k + i);

        if( trans->hash == POSITION->DYN->HASH )
            {
            trans->depth = depth;
            trans->Value = Value;
            trans->move = move;
            trans->age = AGE;
            return;
            }

        if( AGE_DEPTH_MIX(trans->age, trans->depth) > max )
            {
            max = AGE_DEPTH_MIX(trans->age, trans->depth);
            w = i;
            }
        }
    trans = PVHashTable + (k + w);
    trans->hash = POSITION->DYN->HASH;
    trans->depth = depth;
    trans->move = move;
    trans->Value = Value;
    trans->age = AGE;
    }

void HashExact( typePOS *POSITION, int move, int depth, int Value, int FLAGS )
    {
    int DEPTH, i, j, k = POSITION->DYN->HASH & HashMask;
    typeHash *trans;
    int max = 0, w = 0;
    move &= 0x7fff;
    pv_zobrist(POSITION, move, depth, Value);

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 && MAX(trans->DepthUpper, trans->DepthLower) <= depth )
            {
            trans->DepthUpper = trans->DepthLower = depth;
            trans->move = move;
            trans->ValueLower = trans->ValueUpper = Value;
            trans->age = AGE;
            trans->flags = FLAGS;

            for ( j = i + 1; j < 4; j++ )
                {
                trans = HashTable + (k + j);

                if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0
                    && MAX(trans->DepthUpper, trans->DepthLower) <= depth )
                    {
                    memset(trans, 0, 16);
                    trans->age = AGE ^ (MAX_AGE / 2);
                    }
                }
            return;
            }
        DEPTH = MAX(trans->DepthLower, trans->DepthUpper);

        if( AGE_DEPTH_MIX(trans->age, DEPTH) > max )
            {
            max = AGE_DEPTH_MIX(trans->age, DEPTH);
            w = i;
            }
        }
    trans = HashTable + (k + w);
    trans->hash = (POSITION->DYN->HASH >> 32);
    trans->DepthUpper = trans->DepthLower = depth;
    trans->move = move;
    trans->ValueLower = trans->ValueUpper = Value;
    trans->age = AGE;
    trans->flags = FLAGS;
    return;
    }
