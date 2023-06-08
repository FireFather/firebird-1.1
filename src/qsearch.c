#ifndef BUILD_qsearch
#define BUILD_qsearch
#include "firebird.h"
#include "qsearch.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyQsearch( typePOS *POSITION, int VALUE, int depth )
    {
    int Value, i, k = POSITION->DYN->HASH & HashMask, v, best_value;
    uint32 TEMP, move, trans_move = 0, trans_depth, move_depth = 0;
    uint64 TARGET;
    typeMoveList LIST[512], *list, *p, *q;
    typeDYNAMIC *POS0 = POSITION->DYN;
    typeHash *trans;
    DECLARE();

    if( VALUE < -VALUE_MATE + 1 )
        return (-VALUE_MATE + 1);

    if( VALUE > VALUE_MATE - 1 )
        return (VALUE_MATE - 1);

    CheckRepetition;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 )
            {
            if( trans->DepthLower )
                {
                Value = HashLowerBound(trans);

                if( Value >= VALUE )
                    return (Value);
                }

            if( trans->DepthUpper )
                {
                Value = HashUpperBound(trans);

                if( Value < VALUE )
                    return (Value);
                }

            trans_depth = trans->DepthLower;
            move = trans->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                trans_move = move;
                }
            }
        }

    best_value = POS0->Value + TempoValue2;

    if( best_value >= VALUE )
        return (best_value);
    v = VALUE - PrunePawn;
    TARGET = OppOccupied;

    if( v > best_value )
        {
        v = VALUE - PruneMinor;
        TARGET ^= BitboardOppP;

        if( v > best_value )
            {
            TARGET ^= (BitboardOppN | BitboardOppB);
            v = VALUE - PruneRook;

            if( v > best_value )
                TARGET ^= BitboardOppR;
            }

        if( BitboardOppP & MyAttacked )
            best_value += PrunePawn;
        }

    list = MyCapture(POSITION, LIST, TARGET);
    p = LIST;

    while( p->move )
        {
        if( (p->move & 0x7fff) == trans_move )
            p->move |= 0xffff0000;
        p++;
        }
    p = LIST;

    while( p->move )
        {
        move = p->move;
        q = ++p;

        while( q->move )
            {
            if( move < q->move )
                {
                TEMP = q->move;
                q->move = move;
                move = TEMP;
                }
            q++;
            }

        if( !EasySEE(move) && (move & 0x7fff) != trans_move && SqSet[FROM(move)] & ~MyXRAY && !MySEE(POSITION, move) )
            continue;
        move &= 0x7fff;
        MAKE(POSITION, move);
        EvalLazy(VALUE, VALUE, LazyValue, move);

        if( ILLEGAL_MOVE )
            {
            UNDO(POSITION, move);
            continue;
            }

        if( IS_EXACT(POSITION->DYN->exact) )
            v = -POSITION->DYN->Value;

        else if( MOVE_IS_CHECK )
            v = -OppQsearchCheck(POSITION, 1 - VALUE, depth - 1);

        else
            v = -OppQsearch(POSITION, 1 - VALUE, depth - 1);
        UNDO(POSITION, move);
        CHECK_HALT();

        if( v <= best_value )
            continue;
        best_value = v;

        if( v >= VALUE )
            {
            HashLower(POSITION->DYN->HASH, move, 1, v);
            return (v);
            }
        }

    if( depth >= -1 && POS0->Value >= VALUE - (100 + (12 << (depth + 4))) )
        {
        list = MyQuietChecks(POSITION, LIST, TARGET);

        for ( i = 0; i < list - LIST; i++ )
            {
            move = LIST[i].move;
            move &= 0x7fff;
            MAKE(POSITION, move);
            EvalLazy(VALUE, VALUE, LazyValue, move);

            if( ILLEGAL_MOVE )
                {
                UNDO(POSITION, move);
                continue;
                }

            if( IS_EXACT(POSITION->DYN->exact) )
                v = -POSITION->DYN->Value;
            else
                v = -OppQsearchCheck(POSITION, 1 - VALUE, depth - 1);
            UNDO(POSITION, move);
            CHECK_HALT();

            if( v <= best_value )
                continue;
            best_value = v;

            if( v >= VALUE )
                {
                HashLower(POSITION->DYN->HASH, move, 1, v);
                return (v);
                }
            }
        }
    HashUpper(POSITION->DYN->HASH, 1, best_value);
    return (best_value);
    }

int MyQsearchCheck( typePOS *POSITION, int VALUE, int depth )
    {
    int ignored, Value, i, k = POSITION->DYN->HASH & HashMask;
    int v, best_value, trans_depth, move_depth = 0;
    typeHash *trans;
    uint64 TARGET;
    typeMoveList LIST[512], *list, *p, *q;
    typeDYNAMIC *POS0;
    uint32 move, TEMP, trans_move = 0;
    DECLARE();

    POS0 = POSITION->DYN;

    if( VALUE < -VALUE_MATE + 1 )
        return (-VALUE_MATE + 1);

    if( VALUE > VALUE_MATE - 1 )
        return (VALUE_MATE - 1);

    CheckRepetition;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 )
            {
            if( trans->DepthLower )
                {
                Value = HashLowerBound(trans);

                if( Value >= VALUE )
                    return (Value);
                }

            if( trans->DepthUpper )
                {
                Value = HashUpperBound(trans);

                if( Value < VALUE )
                    return (Value);
                }

            trans_depth = trans->DepthLower;
            move = trans->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                trans_move = move;
                }
            }
        }
    best_value = HEIGHT(POSITION) - VALUE_MATE;
    TARGET = 0xffffffffffffffff;

    if( POS0->Value + PruneCheck < VALUE )
        {
        best_value = POS0->Value + PruneCheck;
        v = VALUE - 200;
        TARGET = OppOccupied;

        if( v > best_value )
            {
            TARGET ^= BitboardOppP;
            v = VALUE - 500;
            best_value += 200;

            if( v > best_value )
                TARGET ^= (BitboardOppN | BitboardOppB);
            }
        }

    list = MyEvasion(POSITION, LIST, TARGET);

    if( (list - LIST) > 1 )
        depth--;
    p = LIST;

    while( p->move )
        {
        if( (p->move & 0x7fff) == trans_move )
            p->move |= 0xfff00000;
        p++;
        }
    p = LIST;
    ignored = 0;

    while( p->move )
        {
        move = p->move;
        q = ++p;

        while( q->move )
            {
            if( move < q->move )
                {
                TEMP = q->move;
                q->move = move;
                move = TEMP;
                }
            q++;
            }

        if( IsInterpose(move) && VALUE > -25000 && (move & 0x7fff) != trans_move && !MySEE(POSITION, move) )
            {
            ignored++;
            continue;
            }

        if( POSITION->sq[TO(move)] == 0 && (move & 0x6000) == 0 && (move & 0x7fff)
            != trans_move && MyNull && MAX_POSITIONAL(move)
            + POS0->Value < VALUE + 25 && VALUE > -25000 )
            {
            ignored++;
            continue;
            }
        move &= 0x7fff;
        MAKE(POSITION, move);
        EvalLazy(VALUE, VALUE, LazyValue, move);

        if( ILLEGAL_MOVE )
            {
            UNDO(POSITION, move);
            continue;
            }

        if( IS_EXACT(POSITION->DYN->exact) )
            v = -POSITION->DYN->Value;

        else if( MOVE_IS_CHECK )
            v = -OppQsearchCheck(POSITION, 1 - VALUE, depth);

        else
            v = -OppQsearch(POSITION, 1 - VALUE, depth);
        UNDO(POSITION, move);
        CHECK_HALT();

        if( v <= best_value )
            continue;
        best_value = v;

        if( v >= VALUE )
            {
            HashLower(POSITION->DYN->HASH, move, 1, v);
            return (v);
            }
        }

    if( ignored && best_value < -25000 )
        best_value = VALUE - 1;
    HashUpper(POSITION->DYN->HASH, 1, best_value);
    return (best_value);
    }
