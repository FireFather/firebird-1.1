#ifndef BUILD_cut_node
#define BUILD_cut_node
#include "firebird.h"
#include "null_move.h"
#include "cut_node.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyCut( typePos *Position, int VALUE, int depth )
    {
    int height, move, i, Singular;
    int k;
    typeHash *rank;
    int trans_depth, move_depth = 0, trans_move = 0, Value, cnt;
    int v, Extend, new_depth, move_is_check;
    typeNext NextMove[1];
    typePosition *TempPosition = Position->Current;
    uint64 zob = Position->Current->Hash;
    int to, fr;
    boolean Split;

    if( VALUE < -ValueMate + 1 )
        return (-ValueMate + 1);

    if( VALUE > ValueMate - 1 )
        return (ValueMate - 1);
    (TempPosition + 1)->move = 0;
    CheckRepetition;
    k = zob & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (zob >> 32)) == 0 )
            {
            trans_depth = rank->DepthLower;
            move = rank->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                (TempPosition + 1)->move = trans_move = move;
                }
            trans_depth = MAX(rank->DepthLower, rank->DepthUpper);

            if( rank->DepthLower >= depth )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    {
                    if( !((rank->flags &FlagAll) == FlagAll) )
                        if( MyNull || move )
                            {
                            UpdateAge();
                            return (Value);
                            }
                    }
                }

            if( rank->DepthUpper >= depth )
                {
                Value = HashUpperBound(rank);

                if( Value < VALUE )
                    {
                    UpdateAge();
                    return (Value);
                    }
                }
            }
        }
    NextMove->trans_move = trans_move;

    if( TempPosition->Value >= VALUE && MyNull )
        {
		new_depth = depth - NullReduction;
		new_depth -= ((uint32)(MIN(TempPosition->Value - VALUE, 96))) / 32;

	    v = VALUE;

        if( v >= VALUE )
            {
            MakeNull(Position);

            if( QSearchCondition )
                v = -OppQsearch(Position, 1 - VALUE, 0);

            else if( LowDepthCondition )
                v = -OppLowDepth(Position, 1 - VALUE, new_depth);

            else
                v = -OppAll(Position, 1 - VALUE, new_depth);
            UndoNull(Position);
            CheckHalt();
            }

        if( NullMoveVerification && v >= VALUE )
            {
            int Flags = Position->Current->flags;
            Position->Current->flags &= ~3;
            new_depth -= VerificationReduction;

            if( QSearchCondition )
                v = MyQsearch(Position, VALUE, 0);

            else if( LowDepthCondition )
                v = MyLowDepth(Position, VALUE, new_depth);

            else
                v = MyCut(Position, VALUE, new_depth - 2);
            Position->Current->flags = Flags;
            CheckHalt();
            }

        if( v >= VALUE )
            {
            if( trans_move == 0 )
                HashLower(Position->Current->Hash, 0, depth, v);
            return (v);
            }
        }

	if (trans_move == 0 && depth >= 6)
		{
		new_depth = depth - 4;
		if (LowDepthCondition)
			v = MyLowDepth (Position, VALUE, new_depth);
		else
			v = MyCut (Position, VALUE, new_depth);
		CheckHalt();

        if( v >= VALUE )
            trans_move = (TempPosition + 1)->move;
        }
    Singular = 0;

    if( depth >= 16 && trans_move && MyOK(Position, trans_move) )
        {
        v = MyExclude(Position, VALUE - depth, depth - MIN(12, depth / 2), trans_move & 0x7fff);
        CheckHalt();

        if( v < VALUE - depth )
            {
            Singular++;
            height = Height(Position);

            if( height * 4 <= depth )
                Singular++;
            v = MyExclude(Position, VALUE - 2 * depth, depth - MIN(12, depth / 2), trans_move & 0x7fff);
            CheckHalt();

            if( v < VALUE - 2 * depth )
                {
                Singular++;

                if( height * 8 <= depth )
                    Singular++;
                }
            }
        }

    cnt = 0;
    NextMove->trans_move = trans_move;
    NextMove->phase = Trans;
    NextMove->TARGET = OppOccupied;

    if( depth < 20 && VALUE - TempPosition->Value >= 48 * (depth - 5) )
        {
        NextMove->phase = Trans2;
        cnt = 1;

        if( VALUE - TempPosition->Value >= 48 * (depth - 2) )
            NextMove->TARGET ^= BitboardOppP;
        }
    NextMove->move = 0;
    NextMove->bc = 0;
    v = VALUE;

    Split = false;

    while( true )
        {
        if( SMPfree != 0 && depth >= CNSplitDepth && !Split && NextMove->phase != Trans && cnt >= 1
            && NextMove->phase <= Ordinary_Moves )
            {
            int r;
            boolean b;
            Split = true;
            b = SMPSplit(Position, NextMove, depth, VALUE, VALUE, NodeTypeCut, &r);

            if( b )
                return r;
            }

        move = MyNext(Position, NextMove);

        if( !move )
            break;
        to = To(move);
        fr = From(move);

        if( IsRepetition(0) )
            {
            cnt++;
            continue;
            }

        if( cnt > 5 && NextMove->phase == Ordinary_Moves && (move & 0xe000) == 0 && SqSet[fr] & ~MyXray && depth < 20 )
            {
            if( (1 << (depth - 6)) + MaxPositional (move) +
            (TempPosition->Value) < VALUE + 35 + 2 * cnt )
                {
                cnt++;
                continue;
                }
            }
        move &= 0x7fff;
        Make(Position, move);
        EvalLazy(VALUE, VALUE, LazyValue2, move);

        if( IllegalMove )
            {
            Undo(Position, move);
            continue;
            }

        if( MoveIsCheck )
            move_is_check = 1;
        else
            move_is_check = 0;

        if( move != NextMove->trans_move )
            Singular = 0;
        Extend = 0;

        if( move == NextMove->trans_move )
            {
            if( PassedPawnPush(to, FourthRank(to)) )
                Extend = 1;
            }
        else
            {
            if( PassedPawnPush(to, SixthRank(to)) )
                Extend = 1;
            }

        if( NextMove->trans_move == move && To((Pos1 - 1)->move) == To(Pos1->move) && (Pos1 - 1)->cp != 0 )
            Extend++;
        Extend = MAX(Extend, Singular);

        if( PosIsExact(Position->Current->exact) )
            v = -Position->Current->Value;
        else if( move_is_check )
            {
            new_depth = depth - 2 + MAX(1, Extend);
            v = -OppAllCheck(Position, 1 - VALUE, new_depth);
            }
        else
            {
            if( cnt > 2 && depth < 20 && Pos1->cp == 0 && (2 << (depth - 6)) - Pos1->Value < VALUE + cnt - 15 )
                {
                Undo(Position, move);
                cnt++;
                continue;
                }

            if( NextMove->phase == Ordinary_Moves && !Extend )
                {
                new_depth = depth - 2 + Extend - (4 + MSB(4 + cnt));

                if( QSearchCondition )
                    v = -OppQsearch(Position, 1 - VALUE, 0);

                else if( LowDepthCondition )
                    v = -OppLowDepth(Position, 1 - VALUE, new_depth);

                else
                    v = -OppAll(Position, 1 - VALUE, new_depth);

                if( v < VALUE )
                    goto DONE;
                }
            new_depth = depth - 2 + Extend;

            if( LowDepthCondition )
                v = -OppLowDepth(Position, 1 - VALUE, new_depth);
            else
                v = -OppAll(Position, 1 - VALUE, new_depth);
            }
        DONE:
        Undo(Position, move);
        CheckHalt();
        cnt++;

        if( v >= VALUE )
            {
            if( (TempPosition + 1)->cp == 0 && MoveHistory(move) )
                HistoryGood(move, depth);
            HashLower(Position->Current->Hash, move, depth, v);
            return (v);
            }

        if( (TempPosition + 1)->cp == 0 && MoveHistory(move) )
            HistoryBad(move, depth);
        }

    if( !cnt && NextMove->phase <= Trans2 )
        return (0);
    v = VALUE - 1;
    HashUpperCut(Position, depth, v);
    return (v);
    }

int MyCutCheck( typePos *Position, int VALUE, int depth )
    {
    int height, move, k, cnt, Reduction, Extend;
    int trans_depth, move_depth = 0, trans_move = 0, Value, new_depth, v, i;
    typeHash *rank;
    typeMoveList List[512], *list, *p, *q;
    uint64 zob = Position->Current->Hash;
    int best_value, Singular;
    typePosition *TempPosition = Position->Current;
    boolean Gen;

    if( VALUE < -ValueMate + 1 )
        return (-ValueMate + 1);

    if( VALUE > ValueMate - 1 )
        return (ValueMate - 1);
    (TempPosition + 1)->move = 0;
    CheckRepetition;
    k = zob & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (zob >> 32)) == 0 )
            {
            trans_depth = rank->DepthLower;
            move = rank->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                (TempPosition + 1)->move = trans_move = move;
                }
            trans_depth = MAX(rank->DepthLower, rank->DepthUpper);

            if( rank->DepthLower >= depth )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    {
                    if( !((rank->flags &FlagAll) == FlagAll) )
                        {
                        UpdateAge();
                        return (Value);
                        }
                    }
                }

            if( rank->DepthUpper >= depth )
                {
                Value = HashUpperBound(rank);

                if( Value < VALUE )
                    {
                    UpdateAge();
                    return (Value);
                    }
                }
            }
        }

    if( trans_move && !MyOK(Position, trans_move) )
        trans_move = 0;

    best_value = Height(Position) - ValueMate;
    Singular = 0;

    if( depth >= 16 && trans_move )
        {
        v = MyExcludeCheck(Position, VALUE - depth, depth - MIN(12, depth / 2), trans_move & 0x7fff);
        CheckHalt();

        if( v < VALUE - depth )
            {
            Singular++;
            height = Height(Position);

            if( height * 4 <= depth )
                Singular++;
            v = MyExcludeCheck(Position, VALUE - 2 * depth, depth - MIN(12, depth / 2), trans_move & 0x7fff);
            CheckHalt();

            if( v < VALUE - 2 * depth )
                {
                Singular++;

                if( height * 8 <= depth )
                    Singular++;
                }
            }
        }

    p = List;
    List[0].move = trans_move;
    cnt = 0;
    Gen = false;
    List[1].move = 0;

    while( p->move || !Gen )
        {
        if( !p->move )
            {
            list = MyEvasion(Position, List + 1, 0xffffffffffffffff);
            Gen = true;

            for ( p = list - 1; p >= List + 1; p-- )
                {
                if( (p->move & 0x7fff) == trans_move )
                    p->move = 0;
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
            p = List + 1;
            continue;
            }
        move = p->move & 0x7fff;
        p++;

        if( move != trans_move )
            Singular = 0;

        if( IsRepetition(0) )
            {
            cnt++;
            best_value = MAX(0, best_value);
            continue;
            }
        Make(Position, move);
        EvalLazy(VALUE, VALUE, LazyValue2, move);

        if( IllegalMove )
            {
            Undo(Position, move);
            continue;
            }

        if( PosIsExact(Position->Current->exact) )
            v = -Position->Current->Value;
        else if( MoveIsCheck )
            {
            new_depth = depth - 2;

            if( Singular )
                new_depth += Singular;
            else
                new_depth++;

            if( LowDepthCondition )
                v = -OppLowDepthCheck(Position, 1 - VALUE, new_depth);
            else
                v = -OppAllCheck(Position, 1 - VALUE, new_depth);
            }
        else
            {
            if( cnt >= 1 )
                {
                if( depth > 8 )
                    Reduction = MSB(depth - 7);
                else
                    Reduction = 0;
                Reduction += 1 + MIN(cnt, 2);

                if( EarlyGame )
                    Extend = 1;
                else
                    Extend = 0;
                new_depth = depth + Extend - Reduction - 2;

                if( QSearchCondition )
                    v = -OppQsearch(Position, 1 - VALUE, 0);

                else if( LowDepthCondition )
                    v = -OppLowDepth(Position, 1 - VALUE, new_depth);

                else
                    v = -OppAll(Position, 1 - VALUE, new_depth);

                if( v < VALUE )
                    goto LOOP;
                }

            if( !Singular && EarlyGame )
                Extend = 1;
            else
                Extend = 0;
            new_depth = depth - 2 + Extend + Singular;

            if( LowDepthCondition )
                v = -OppLowDepth(Position, 1 - VALUE, new_depth);
            else
                v = -OppAll(Position, 1 - VALUE, new_depth);
            }
        LOOP:
        Undo(Position, move);
        CheckHalt();

        if( v > best_value )
            best_value = v;

        if( v < VALUE )
            {
            cnt++;
            continue;
            }
        HashLower(Position->Current->Hash, move, MAX(1, depth), v);
        return (v);
        }
    HashUpperCut(Position, MAX(1, depth), best_value);
    return (best_value);
    }
