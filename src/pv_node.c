#ifndef BUILD_pv_node
#define BUILD_pv_node
#include "firebird.h"
#include "history.h"
#include "control.h"
#include "pv_node.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyPV( typePOS *POSITION, int ALPHA, int BETA, int depth, int check )
    {
    typeNEXT NextMove[1];
    typeHash *trans;
    int good_move, v, Value, k, i, trans_depth, move, move_depth = 0, trans_move = 0, hash_depth;
    int SINGULAR;
    boolean SPLIT;
    typeMoveList *list, *p, *q;
    int EXTEND, best_value, new_depth, move_is_check, to, fr;
    typeDYNAMIC *POS0 = POSITION->DYN;
    DECLARE();

    if( BETA < -VALUE_MATE )
        return (-VALUE_MATE);

    if( ALPHA > VALUE_MATE )
        return (VALUE_MATE);

    if( depth <= 1 )
        {
        if( check )
            return MyPVQsearchCheck(POSITION, ALPHA, BETA, 1);
        else
            return MyPVQsearch(POSITION, ALPHA, BETA, 1);
        }
    CheckRepetition;
    NextMove->trans_move = 0;
    hash_depth = 0;
    NextMove->move = 0;
    NextMove->bc = 0;

    k = POSITION->DYN->HASH & HashMask;
    (POS0 + 1)->move = 0;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 )
            {
            trans_depth = trans->DepthLower;
            move = trans->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                (POS0 + 1)->move = trans_move = move;
                }

            if( trans->DepthLower > trans->DepthUpper )
                {
                trans_depth = trans->DepthLower;
                Value = HashLowerBound(trans);
                }
            else
                {
                trans_depth = trans->DepthUpper;
                Value = HashUpperBound(trans);
                }

            if( trans_depth > hash_depth )
                hash_depth = trans_depth;

            if( IsExact(trans) && trans_depth >= depth )
                {
                UPDATE_AGE();

                if( !ANALYSING )
                    return (Value);
                }
            }
        }

    if( !trans_move && depth >= 6 )
        {
        v = ALPHA;

        if( depth >= 10 )
            {
            v = MyPV(POSITION, ALPHA - depth, BETA + depth, depth - 8, check);
            CHECK_HALT();

            if( v > ALPHA - depth )
                trans_move = (POS0 + 1)->move;
            }

        if( v > ALPHA - depth )
            v = MyPV(POSITION, ALPHA - depth, BETA + depth, depth - 4, check);
        CHECK_HALT();

        if( v > ALPHA - depth )
            trans_move = (POS0 + 1)->move;
        }
    else if( depth >= 10 && depth > hash_depth + 8 )
        {
        v = MyPV(POSITION, ALPHA - depth, BETA + depth, depth - 8, check);
        CHECK_HALT();

        if( v > ALPHA - depth )
            trans_move = (POS0 + 1)->move;

        if( v > ALPHA - depth )
            {
            v = MyPV(POSITION, ALPHA - depth, BETA + depth, depth - 4, check);
            CHECK_HALT();

            if( v > ALPHA - depth )
                trans_move = (POS0 + 1)->move;
            }
        }

    NextMove->trans_move = trans_move;
    NextMove->phase = TRANS;
    EXTEND = 0;
    NextMove->TARGET = OppOccupied;

    SINGULAR = 0;

    if( check )
        {
        list = MyEvasion(POSITION, NextMove->LIST, 0xffffffffffffffff);
        NextMove->phase = EVADE_PHASE;

        for ( p = list - 1; p >= NextMove->LIST; p-- )
            {
            if( (p->move & 0x7fff) == trans_move )
                p->move |= 0xffff0000;
            else if( p->move <= (0x80 << 24) )
                {
                if( (p->move & 0x7fff) == POS0->killer1 )
                    p->move |= 0x7fff8000;

                else if( (p->move & 0x7fff) == POS0->killer2 )
                    p->move |= 0x7fff0000;

                else
                    p->move |= (p->move & 0x7fff) | (HISTORY_VALUE(POSITION, p->move) << 15);
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

        if( (list - NextMove->LIST) <= 1 )
            SINGULAR = 2;

        if( (list - NextMove->LIST) == 2 )
            SINGULAR = 1;

        if( (list - NextMove->LIST) > 2 )
            SINGULAR = 0;
        }

    if( depth >= 16 && NextMove->trans_move && SINGULAR < 2 && MyOK(POSITION, NextMove->trans_move) )
        {
        move = NextMove->trans_move;
        to = TO(move);
        fr = FROM(move);
        MAKE(POSITION, move);
        EVAL(move);

        if( ILLEGAL_MOVE )
            {
            UNDO(POSITION, move);
            goto SKIP;
            }
        Value = -OppPV(POSITION, -BETA, -ALPHA, depth - 10, (MOVE_IS_CHECK) != 0);
        UNDO(POSITION, move);
        CHECK_HALT();
#define DEPTH_RED (MIN (12, depth / 2))
#define VALUE_RED1 (depth / 2)
#define VALUE_RED2 (depth)
		if (check)
			v = MyExcludeCheck (POSITION, Value - VALUE_RED1,
			    depth - DEPTH_RED, move & 0x7fff);
		else
			v = MyExclude (POSITION, Value - VALUE_RED1,
		       depth - DEPTH_RED, move & 0x7fff);
		CHECK_HALT();
		if (v < Value - VALUE_RED1)
			{	
			SINGULAR = 1;
			if (check)
				v = MyExcludeCheck (POSITION, Value - VALUE_RED2,
			depth - DEPTH_RED, move & 0x7fff);
			else
				v = MyExclude (POSITION, Value - VALUE_RED2,
			depth - DEPTH_RED, move & 0x7fff);
			CHECK_HALT();
			if (v < Value - VALUE_RED2)
				SINGULAR = 2;
            }
        }
    SKIP:
    best_value = -VALUE_INFINITY;
    NextMove->move = 0;
    NextMove->bc = 0;
    good_move = 0;
    SPLIT = false;

    while( true )
        {
        if( SMP_FREE != 0 && !check && depth >= PV_SPLIT_DEPTH && !SPLIT && best_value != -VALUE_INFINITY )
            {
            int r;
            boolean b;
            SPLIT = true;
            b = IVAN_SPLIT(POSITION, NextMove, depth, BETA, ALPHA, NODE_TYPE_PV, &r);
            CHECK_HALT();

            if( b )
                {
                if( r > ALPHA || !good_move )
                    return r;
                move = good_move;
                (POS0 + 1)->move = good_move & 0x7fff;
                best_value = r;
                goto IVAN;
                }
            }

        move = MyNext(POSITION, NextMove);

        if( !move )
            break;
        to = TO(move);
        fr = FROM(move);

        if( ALPHA > 0 && POS0->reversible >= 2 && ((TO(move) << 6) | FROM(move)) == (POS0 - 1)->move
            && POSITION->sq[TO(move)] == 0 )
            {
            best_value = MAX(0, best_value);
            continue;
            }
        move &= 0x7fff;
        MAKE(POSITION, move);
        EVAL(move);

        if( ILLEGAL_MOVE )
            {
            UNDO(POSITION, move);
            continue;
            }
        move_is_check = (MOVE_IS_CHECK != 0);
        EXTEND = 0;

        if( EXTEND < 2 )
            {
            if( PassedPawnPush(to, SIXTH_RANK(to)) )
                EXTEND = 2;
            }

        if( EXTEND < 2 )
            {
            if( POS1->cp != 0 || move_is_check || (check && EARLY_GAME) )
                EXTEND = 1;

            else if( PassedPawnPush(to, FOURTH_RANK(to)) )
                EXTEND = 1;
            }

        if( NextMove->trans_move != move )
            SINGULAR = 0;
        new_depth = depth - 2 + MAX(EXTEND, SINGULAR);

        if( IS_EXACT(POSITION->DYN->exact) )
            v = -POSITION->DYN->Value;
        else if( NextMove->trans_move != move && new_depth > 1 )
            {
            if( LOW_DEPTH_CONDITION_PV )
                {
                if( move_is_check )
                    v = -OppLowDepthCheck(POSITION, -ALPHA, new_depth);
                else
                    v = -OppLowDepth(POSITION, -ALPHA, new_depth);
                }
            else
                {
                if( move_is_check )
                    v = -OppCutCheck(POSITION, -ALPHA, new_depth);
                else
                    v = -OppCut(POSITION, -ALPHA, new_depth);
                }

            if( v > ALPHA )
                v = -OppPV(POSITION, -BETA, -ALPHA, new_depth, move_is_check);
            }
        else
            v = -OppPV(POSITION, -BETA, -ALPHA, new_depth, move_is_check);
        UNDO(POSITION, move);
        CHECK_HALT();

        if( v <= ALPHA && POSITION->sq[TO(move)] == 0 && MoveHistory(move) )
            HISTORY_BAD1(move, depth);

        if( v <= best_value )
            continue;
        best_value = v;

        if( v <= ALPHA )
            continue;
        ALPHA = v;
        good_move = move;
        HashLower(POSITION->DYN->HASH, move, depth, v);

        if( v >= BETA )
            {
            if( POSITION->sq[TO(move)] == 0 && MoveHistory(move) )
                HISTORY_GOOD(move, depth);
            return (v);
            }
        }

    move = good_move;
    (POS0 + 1)->move = good_move & 0x7fff;

    if( best_value == -VALUE_INFINITY )
        {
        if( !check )
            return (0);
        return (HEIGHT(POSITION) - VALUE_MATE);
        }
    IVAN:
    if( move )
        {
        if( POSITION->sq[TO(move)] == 0 && MoveHistory(move) )
            HISTORY_GOOD(move, depth);
        HashExact(POSITION, move, depth, best_value, FLAG_EXACT);
        return (best_value);
        }
    HashUpper(POSITION->DYN->HASH, depth, best_value);
    return (best_value);
    }
