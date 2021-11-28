#ifndef BUILD_pv_node
#define BUILD_pv_node
#include "firebird.h"
#include "pv_node.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyPV( typePos *Position, int Alpha, int Beta, int depth, int check )
    {
    typeNext NextMove[1];
    typeHash *rank;
    int good_move, v, Value, k, i, trans_depth, move, move_depth = 0, trans_move = 0, hash_depth;
    int Singular;
    boolean Split;
    typeMoveList *list, *p, *q;
    int Extend, best_value, new_depth, move_is_check, to, fr;
    typePosition *TempPosition = Position->Current;

    if( Beta < -ValueMate )
        return (-ValueMate);

    if( Alpha > ValueMate )
        return (ValueMate);

    if( depth <= 1 )
        {
        if( check )
            return MyPVQsearchCheck(Position, Alpha, Beta, 1);
        else
            return MyPVQsearch(Position, Alpha, Beta, 1);
        }
    CheckRepetition;
    NextMove->trans_move = 0;
    hash_depth = 0;
    NextMove->move = 0;
    NextMove->bc = 0;

    k = Position->Current->Hash & HashMask;
    (TempPosition + 1)->move = 0;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 )
            {
            trans_depth = rank->DepthLower;
            move = rank->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                (TempPosition + 1)->move = trans_move = move;
                }

            if( rank->DepthLower > rank->DepthUpper )
                {
                trans_depth = rank->DepthLower;
                Value = HashLowerBound(rank);
                }
            else
                {
                trans_depth = rank->DepthUpper;
                Value = HashUpperBound(rank);
                }

            if( trans_depth > hash_depth )
                hash_depth = trans_depth;

            if( IsExact(rank) && trans_depth >= depth )
                {
                UpdateAge();

                if( !Analysing )
                    return (Value);
                }
            }
        }

    if( !trans_move && depth >= 6 )
        {
        v = Alpha;

        if( depth >= 10 )
            {
            v = MyPV(Position, Alpha - depth, Beta + depth, depth - 8, check);
            CheckHalt();

            if( v > Alpha - depth )
                trans_move = (TempPosition + 1)->move;
            }

        if( v > Alpha - depth )
            v = MyPV(Position, Alpha - depth, Beta + depth, depth - 4, check);
        CheckHalt();

        if( v > Alpha - depth )
            trans_move = (TempPosition + 1)->move;
        }
    else if( depth >= 10 && depth > hash_depth + 8 )
        {
        v = MyPV(Position, Alpha - depth, Beta + depth, depth - 8, check);
        CheckHalt();

        if( v > Alpha - depth )
            trans_move = (TempPosition + 1)->move;

        if( v > Alpha - depth )
            {
            v = MyPV(Position, Alpha - depth, Beta + depth, depth - 4, check);
            CheckHalt();

            if( v > Alpha - depth )
                trans_move = (TempPosition + 1)->move;
            }
        }

    NextMove->trans_move = trans_move;
    NextMove->phase = Trans;
    Extend = 0;
    NextMove->TARGET = OppOccupied;

    Singular = 0;

    if( check )
        {
        list = MyEvasion(Position, NextMove->List, 0xffffffffffffffff);
        NextMove->phase = Evade_Phase;

        for ( p = list - 1; p >= NextMove->List; p-- )
            {
            if( (p->move & 0x7fff) == trans_move )
                p->move |= 0xffff0000;
            else if( p->move <= (0x80 << 24) )
                {
                if( (p->move & 0x7fff) == TempPosition->killer1 )
                    p->move |= 0x7fff8000;

                else if( (p->move & 0x7fff) == TempPosition->killer2 )
                    p->move |= 0x7fff0000;

                else
                    p->move |= (p->move & 0x7fff) | (HistoryValue(Position, p->move) << 15);
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

        if( (list - NextMove->List) <= 1 )
            Singular = 2;

        if( (list - NextMove->List) == 2 )
            Singular = 1;

        if( (list - NextMove->List) > 2 )
            Singular = 0;
        }

    if( depth >= 16 && NextMove->trans_move && Singular < 2 && MyOK(Position, NextMove->trans_move) )
        {
        move = NextMove->trans_move;
        to = To(move);
        fr = From(move);
        Make(Position, move);
        EVAL(move);

        if( IllegalMove )
            {
            Undo(Position, move);
            goto SKIP;
            }
        Value = -OppPV(Position, -Beta, -Alpha, depth - 10, (MoveIsCheck) != 0);
        Undo(Position, move);
        CheckHalt();
#define DepthRed (MIN (12, depth / 2))
#define ValueRed1 (depth / 2)
#define ValueRed2 (depth)
		if (check)
			v = MyExcludeCheck (Position, Value - ValueRed1,
			    depth - DepthRed, move & 0x7fff);
		else
			v = MyExclude (Position, Value - ValueRed1,
		       depth - DepthRed, move & 0x7fff);
		CheckHalt();
		if (v < Value - ValueRed1)
			{	
			Singular = 1;
			if (check)
				v = MyExcludeCheck (Position, Value - ValueRed2,
			depth - DepthRed, move & 0x7fff);
			else
				v = MyExclude (Position, Value - ValueRed2,
			depth - DepthRed, move & 0x7fff);
			CheckHalt();
			if (v < Value - ValueRed2)
				Singular = 2;
            }
        }
    SKIP:
    best_value = -ValueInfinity;
    NextMove->move = 0;
    NextMove->bc = 0;
    good_move = 0;
    Split = false;

    while( true )
        {
        if( SMPfree != 0 && !check && depth >= PVSplitDepth && !Split && best_value != -ValueInfinity )
            {
            int r;
            boolean b;
            Split = true;
            b = SMPSplit(Position, NextMove, depth, Beta, Alpha, NodeTypePV, &r);
            CheckHalt();

            if( b )
                {
                if( r > Alpha || !good_move )
                    return r;
                move = good_move;
                (TempPosition + 1)->move = good_move & 0x7fff;
                best_value = r;
                goto IVAN;
                }
            }

        move = MyNext(Position, NextMove);

        if( !move )
            break;
        to = To(move);
        fr = From(move);

        if( Alpha > 0 && TempPosition->reversible >= 2 && ((To(move) << 6) | From(move)) == (TempPosition - 1)->move
            && Position->sq[To(move)] == 0 )
            {
            best_value = MAX(0, best_value);
            continue;
            }
        move &= 0x7fff;
        Make(Position, move);
        EVAL(move);

        if( IllegalMove )
            {
            Undo(Position, move);
            continue;
            }
        move_is_check = (MoveIsCheck != 0);
        Extend = 0;

        if( Extend < 2 )
            {
            if( PassedPawnPush(to, SixthRank(to)) )
                Extend = 2;
            }

        if( Extend < 2 )
            {
            if( Pos1->cp != 0 || move_is_check || (check && EarlyGame) )
                Extend = 1;

            else if( PassedPawnPush(to, FourthRank(to)) )
                Extend = 1;
            }

        if( NextMove->trans_move != move )
            Singular = 0;
        new_depth = depth - 2 + MAX(Extend, Singular);

        if( PosIsExact(Position->Current->exact) )
            v = -Position->Current->Value;
        else if( NextMove->trans_move != move && new_depth > 1 )
            {
            if( LowDepthConditionPV )
                {
                if( move_is_check )
                    v = -OppLowDepthCheck(Position, -Alpha, new_depth);
                else
                    v = -OppLowDepth(Position, -Alpha, new_depth);
                }
            else
                {
                if( move_is_check )
                    v = -OppCutCheck(Position, -Alpha, new_depth);
                else
                    v = -OppCut(Position, -Alpha, new_depth);
                }

            if( v > Alpha )
                v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);
            }
        else
            v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);
        Undo(Position, move);
        CheckHalt();

        if( v <= Alpha && Position->sq[To(move)] == 0 && MoveHistory(move) )
            HistoryBad1(move, depth);

        if( v <= best_value )
            continue;
        best_value = v;

        if( v <= Alpha )
            continue;
        Alpha = v;
        good_move = move;
        HashLower(Position->Current->Hash, move, depth, v);

        if( v >= Beta )
            {
            if( Position->sq[To(move)] == 0 && MoveHistory(move) )
                HistoryGood(move, depth);
            return (v);
            }
        }

    move = good_move;
    (TempPosition + 1)->move = good_move & 0x7fff;

    if( best_value == -ValueInfinity )
        {
        if( !check )
            return (0);
        return (Height(Position) - ValueMate);
        }
    IVAN:
    if( move )
        {
        if( Position->sq[To(move)] == 0 && MoveHistory(move) )
            HistoryGood(move, depth);
        HashExact(Position, move, depth, best_value, FlagExact);
        return (best_value);
        }
    HashUpper(Position->Current->Hash, depth, best_value);
    return (best_value);
    }
