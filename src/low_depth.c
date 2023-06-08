#ifndef BUILD_low_depth
#define BUILD_low_depth
#include "firebird.h"
#include "history.h"
#include "null_move.h"
#include "low_depth.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyLowDepth( typePOS *POSITION, int VALUE, int depth )
    {
    int cnt, Value, best_value, v, k, i, trans_move = 0;
    int move, move_depth = 0, trans_depth, to, fr;
    typeNEXT NextMove[1];
    typeDYNAMIC *POS0 = POSITION->DYN;
    typeHash *trans;
    DECLARE();

    if( VALUE < -VALUE_MATE + 1 )
        return (-VALUE_MATE + 1);

    if( VALUE > VALUE_MATE - 1 )
        return (VALUE_MATE - 1);
    (POS0 + 1)->move = 0;
    v = POS0->Value + 1125;

    if( v < VALUE )
        return (VALUE - 1);
    CheckRepetition;
    k = POSITION->DYN->HASH & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 )
            {
            if( trans->DepthLower >= depth )
                {
                Value = HashLowerBound(trans);

                if( Value >= VALUE )
                    {
                    (POS0 + 1)->move = trans->move;
                    return (Value);
                    }
                }

            if( trans->DepthUpper >= depth )
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
    v = POS0->Value - (70 + 10 * depth);

    if( v >= VALUE )
        return (POS0->Value);
    best_value = MIN(POS0->Value, VALUE - 1);

    if( POS0->Value >= VALUE && MyNull )
        {
        v = VALUE;

        if( v >= VALUE )
            {
            MakeNull(POSITION);
            v = -OppQsearch(POSITION, 1 - VALUE, 0);
            UndoNull(POSITION);
            CHECK_HALT();
            }

        if( NULL_MOVE_VERIFICATION && v >= VALUE )
            {
            int FLAGS = POSITION->DYN->flags;
            POSITION->DYN->flags &= ~3;
            v = MyQsearch(POSITION, VALUE, 0);
            POSITION->DYN->flags = FLAGS;
            CHECK_HALT();
            }

        if( v >= VALUE )
            {
            if( trans_move == 0 )
                HashLower(POSITION->DYN->HASH, 0, depth, v);
            return (v);
            }
        }
    NextMove->phase = TRANS;
    NextMove->TARGET = OppOccupied;

    if( POS0->Value + 50 + 8 * depth < VALUE )
        {
        NextMove->phase = TRANS2;

        if( VALUE >= POS0->Value + 75 + 32 * depth )
            {
            NextMove->TARGET ^= BitboardOppP;

            if( BitboardOppP & MyAttacked )
                best_value += 125;

            if( depth <= 3 && VALUE >= POS0->Value + 400 + 32 * depth )
                {
                NextMove->TARGET ^= (BitboardOppN | BitboardOppB);
                best_value += 300;

                if( VALUE >= POS0->Value + 600 + 32 * depth )
                    {
                    NextMove->TARGET ^= BitboardOppR;
                    best_value += 200;
                    }
                }
            }
        }
    else if( depth <= 3 && POS0->Value + 4 * depth < VALUE )
        {
        NextMove->phase = TRANS3;
        NextMove->mask = (VALUE - POS0->Value) + 4 * depth + 5;
        }
    NextMove->bc = 0;
    NextMove->move = 0;
    NextMove->trans_move = trans_move;
    cnt = 0;

    while( (move = MyNext(POSITION, NextMove)) )
        {
        to = TO(move);
        fr = FROM(move);

        if( IsRepetition(0) )
            {
            best_value = MAX(0, best_value);
            cnt++;
            continue;
            }

        if( cnt >= depth && NextMove->phase == ORDINARY_MOVES && (move & 0xe000) == 0 && SqSet[fr] & ~MyXRAY
            && MyOccupied ^ (BitboardMyP | BitboardMyK) )
            {
            if( (2 * depth) + MAX_POSITIONAL (move) + POS0->Value <
            VALUE + 40 + 2 * cnt )
                {
                cnt++;
                continue;
                }
            }

        if( (POSITION->sq[to] == 0 || (depth <= 5 && !EasySEE(move))) && SqSet[fr] & ~MyXRAY
            && POSITION->sq[fr] != EnumMyK && !MoveIsEP(move) && move != trans_move && !MySEE(POSITION, move) )
            {
            cnt++;
            continue;
            }

        move &= 0x7fff;
        MAKE(POSITION, move);
        EvalLazy(VALUE, VALUE, LazyValue, move);

        if( ILLEGAL_MOVE || (NextMove->phase == POSITIONAL_GAIN_PHASE && MOVE_IS_CHECK) )
            {
            UNDO(POSITION, move);
            continue;
            }

        if( IS_EXACT(POSITION->DYN->exact) )
            v = -POSITION->DYN->Value;

        else if( MOVE_IS_CHECK )
            v = -OppLowDepthCheck(POSITION, 1 - VALUE, depth - 1);

        else
            {
            if( cnt >= depth && (2 * depth) - POS1->Value < VALUE + cnt )
                {
                UNDO(POSITION, move);
                cnt++;
                continue;
                }

            if( depth <= 3 )
                v = -OppQsearch(POSITION, 1 - VALUE, 0);
            else
                v = -OppLowDepth(POSITION, 1 - VALUE, depth - 2);
            }
        cnt++;
        UNDO(POSITION, move);
        CHECK_HALT();

        if( v >= VALUE )
            {
            if( (POS0 + 1)->cp == 0 && MoveHistory(move) )
                HISTORY_GOOD(move, depth);
            HashLower(POSITION->DYN->HASH, move, depth, v);
            return (v);
            }

        if( v >= best_value )
            best_value = v;

        if( (POS0 + 1)->cp == 0 && MoveHistory(move) )
            HISTORY_BAD(move, depth);
        }

    if( !cnt && NextMove->phase <= TRANS2 )
        return (0);
    HashUpper(POSITION->DYN->HASH, depth, best_value);
    return (best_value);
    }

int MyLowDepthCheck( typePOS *POSITION, int VALUE, int depth )
    {
    int ignored, k, trans_move = 0, trans_depth, move_depth = 0;
    int Value, i, move, best_value, v, new_depth;
    boolean GEN;
    typeHash *trans;
    typeMoveList LIST[512], *list, *p, *q;
    typeDYNAMIC *POS0 = POSITION->DYN;
    DECLARE();

    if( VALUE < -VALUE_MATE + 1 )
        return (-VALUE_MATE + 1);

    if( VALUE > VALUE_MATE - 1 )
        return (VALUE_MATE - 1);

    CheckRepetition;
    k = POSITION->DYN->HASH & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 )
            {
            if( trans->DepthLower && trans->DepthLower >= depth )
                {
                Value = HashLowerBound(trans);

                if( Value >= VALUE )
                    return (Value);
                }

            if( trans->DepthUpper && trans->DepthUpper >= depth )
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

    if( trans_move && !MyOK(POSITION, trans_move) )
        trans_move = 0;

    best_value = HEIGHT(POSITION) - VALUE_MATE;
    p = LIST;
    LIST[0].move = trans_move;
    GEN = false;
    LIST[1].move = 0;
    ignored = 0;

    while( p->move || !GEN )
        {
        if( !p->move )
            {
            list = MyEvasion(POSITION, LIST + 1, 0xffffffffffffffff);
            GEN = true;

            for ( p = list - 1; p >= LIST + 1; p-- )
                {
                if( (p->move & 0x7fff) == trans_move )
                    p->move = 0;
                else if( p->move <= (0x80 << 24) )
                    {
                    if( (p->move & 0x7fff) == POS0->killer1 )
                        p->move |= 0x7fff8000;

                    else if( (p->move & 0x7fff) == POS0->killer2 )
                        p->move |= 0x7fff0000;

                    else
                        p->move |= (p->move & 0xffff) | ((HISTORY_VALUE(POSITION, p->move) >> 1) << 16);
                    }
                move = p->move;

                for ( q = p + 1; q < list; q++ )
                    {
                    if( move < q->move )
                        (q - 1)->move = q->move;
                    else
                        break;
                    }
                q--;
                q->move = move;
                }
            p = LIST + 1;
            continue;
            }
        move = p->move;
        p++;

        if( IsRepetition(0) )
            {
            best_value = MAX(0, best_value);
            continue;
            }

        if( IsInterpose(move) && VALUE > -25000 && (move & 0x7fff) != trans_move && !MySEE(POSITION, move) )
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
            v = -OppLowDepthCheck(POSITION, 1 - VALUE, depth - 1 + (EARLY_GAME));

        else
            {
            new_depth = depth - 2 + EARLY_GAME;

            if( QSEARCH_CONDITION )
                v = -OppQsearch(POSITION, 1 - VALUE, 0);
            else
                v = -OppLowDepth(POSITION, 1 - VALUE, new_depth);
            }
        UNDO(POSITION, move);
        CHECK_HALT();

        if( v <= best_value )
            continue;
        best_value = v;

        if( v >= VALUE )
            {
            HashLower(POSITION->DYN->HASH, move, MAX(1, depth), v);
            return (v);
            }
        }

    if( ignored && best_value < -25000 )
        best_value = VALUE - 1;
    HashUpper(POSITION->DYN->HASH, MAX(1, depth), best_value);
    return (best_value);
    }
