#ifndef BUILD_low_depth
#define BUILD_low_depth
#include "firebird.h"
#include "null_move.h"
#include "low_depth.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyLowDepth( typePos *Position, int VALUE, int depth )
    {
    int cnt, Value, best_value, v, k, i, trans_move = 0;
    int move, move_depth = 0, trans_depth, to, fr;
    typeNext NextMove[1];
    typePosition *TempPosition = Position->Current;
    typeHash *rank;

    if( VALUE < -ValueMate + 1 )
        return (-ValueMate + 1);

    if( VALUE > ValueMate - 1 )
        return (ValueMate - 1);
    (TempPosition + 1)->move = 0;
    v = TempPosition->Value + 1125;

    if( v < VALUE )
        return (VALUE - 1);
    CheckRepetition;
    k = Position->Current->Hash & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 )
            {
            if( rank->DepthLower >= depth )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    {
                    (TempPosition + 1)->move = rank->move;
                    return (Value);
                    }
                }

            if( rank->DepthUpper >= depth )
                {
                Value = HashUpperBound(rank);

                if( Value < VALUE )
                    return (Value);
                }

            trans_depth = rank->DepthLower;
            move = rank->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                trans_move = move;
                }
            }
        }
    v = TempPosition->Value - (70 + 10 * depth);

    if( v >= VALUE )
        return (TempPosition->Value);
    best_value = MIN(TempPosition->Value, VALUE - 1);

    if( TempPosition->Value >= VALUE && MyNull )
        {
        v = VALUE;

        if( v >= VALUE )
            {
            MakeNull(Position);
            v = -OppQsearch(Position, 1 - VALUE, 0);
            UndoNull(Position);
            CheckHalt();
            }

        if( NullMoveVerification && v >= VALUE )
            {
            int Flags = Position->Current->flags;
            Position->Current->flags &= ~3;
            v = MyQsearch(Position, VALUE, 0);
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
    NextMove->phase = Trans;
    NextMove->TARGET = OppOccupied;

    if( TempPosition->Value + 50 + 8 * depth < VALUE )
        {
        NextMove->phase = Trans2;

        if( VALUE >= TempPosition->Value + 75 + 32 * depth )
            {
            NextMove->TARGET ^= BitboardOppP;

            if( BitboardOppP & MyAttacked )
                best_value += 125;

            if( depth <= 3 && VALUE >= TempPosition->Value + 400 + 32 * depth )
                {
                NextMove->TARGET ^= (BitboardOppN | BitboardOppB);
                best_value += 300;

                if( VALUE >= TempPosition->Value + 600 + 32 * depth )
                    {
                    NextMove->TARGET ^= BitboardOppR;
                    best_value += 200;
                    }
                }
            }
        }
    else if( depth <= 3 && TempPosition->Value + 4 * depth < VALUE )
        {
        NextMove->phase = Trans3;
        NextMove->mask = (VALUE - TempPosition->Value) + 4 * depth + 5;
        }
    NextMove->bc = 0;
    NextMove->move = 0;
    NextMove->trans_move = trans_move;
    cnt = 0;

    while( (move = MyNext(Position, NextMove)) )
        {
        to = To(move);
        fr = From(move);

        if( IsRepetition(0) )
            {
            best_value = MAX(0, best_value);
            cnt++;
            continue;
            }

        if( cnt >= depth && NextMove->phase == Ordinary_Moves && (move & 0xe000) == 0 && SqSet[fr] & ~MyXray
            && MyOccupied ^ (BitboardMyP | BitboardMyK) )
            {
            if( (2 * depth) + MaxPositional (move) + TempPosition->Value <
            VALUE + 40 + 2 * cnt )
                {
                cnt++;
                continue;
                }
            }

        if( (Position->sq[to] == 0 || (depth <= 5 && !EasySEE(move))) && SqSet[fr] & ~MyXray
            && Position->sq[fr] != EnumMyK && !MoveIsEP(move) && move != trans_move && !MySEE(Position, move) )
            {
            cnt++;
            continue;
            }

        move &= 0x7fff;
        Make(Position, move);
        EvalLazy(VALUE, VALUE, LazyValue, move);

        if( IllegalMove || (NextMove->phase == PositionalGainPhase && MoveIsCheck) )
            {
            Undo(Position, move);
            continue;
            }

        if( PosIsExact(Position->Current->exact) )
            v = -Position->Current->Value;

        else if( MoveIsCheck )
            v = -OppLowDepthCheck(Position, 1 - VALUE, depth - 1);

        else
            {
            if( cnt >= depth && (2 * depth) - Pos1->Value < VALUE + cnt )
                {
                Undo(Position, move);
                cnt++;
                continue;
                }

            if( depth <= 3 )
                v = -OppQsearch(Position, 1 - VALUE, 0);
            else
                v = -OppLowDepth(Position, 1 - VALUE, depth - 2);
            }
        cnt++;
        Undo(Position, move);
        CheckHalt();

        if( v >= VALUE )
            {
            if( (TempPosition + 1)->cp == 0 && MoveHistory(move) )
                HistoryGood(move, depth);
            HashLower(Position->Current->Hash, move, depth, v);
            return (v);
            }

        if( v >= best_value )
            best_value = v;

        if( (TempPosition + 1)->cp == 0 && MoveHistory(move) )
            HistoryBad(move, depth);
        }

    if( !cnt && NextMove->phase <= Trans2 )
        return (0);
    HashUpper(Position->Current->Hash, depth, best_value);
    return (best_value);
    }

int MyLowDepthCheck( typePos *Position, int VALUE, int depth )
    {
    int ignored, k, trans_move = 0, trans_depth, move_depth = 0;
    int Value, i, move, best_value, v, new_depth;
    boolean Gen;
    typeHash *rank;
    typeMoveList List[512], *list, *p, *q;
    typePosition *TempPosition = Position->Current;
  
    if( VALUE < -ValueMate + 1 )
        return (-ValueMate + 1);

    if( VALUE > ValueMate - 1 )
        return (ValueMate - 1);

    CheckRepetition;
    k = Position->Current->Hash & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 )
            {
            if( rank->DepthLower && rank->DepthLower >= depth )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    return (Value);
                }

            if( rank->DepthUpper && rank->DepthUpper >= depth )
                {
                Value = HashUpperBound(rank);

                if( Value < VALUE )
                    return (Value);
                }

            trans_depth = rank->DepthLower;
            move = rank->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                trans_move = move;
                }
            }
        }

    if( trans_move && !MyOK(Position, trans_move) )
        trans_move = 0;

    best_value = Height(Position) - ValueMate;
    p = List;
    List[0].move = trans_move;
    Gen = false;
    List[1].move = 0;
    ignored = 0;

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
                        p->move |= (p->move & 0xffff) | ((HistoryValue(Position, p->move) >> 1) << 16);
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
        move = p->move;
        p++;

        if( IsRepetition(0) )
            {
            best_value = MAX(0, best_value);
            continue;
            }

        if( IsInterpose(move) && VALUE > -25000 && (move & 0x7fff) != trans_move && !MySEE(Position, move) )
            {
            ignored++;
            continue;
            }
        move &= 0x7fff;
        Make(Position, move);
        EvalLazy(VALUE, VALUE, LazyValue, move);

        if( IllegalMove )
            {
            Undo(Position, move);
            continue;
            }

        if( PosIsExact(Position->Current->exact) )
            v = -Position->Current->Value;

        else if( MoveIsCheck )
            v = -OppLowDepthCheck(Position, 1 - VALUE, depth - 1 + (EarlyGame));

        else
            {
            new_depth = depth - 2 + EarlyGame;

            if( QSearchCondition )
                v = -OppQsearch(Position, 1 - VALUE, 0);
            else
                v = -OppLowDepth(Position, 1 - VALUE, new_depth);
            }
        Undo(Position, move);
        CheckHalt();

        if( v <= best_value )
            continue;
        best_value = v;

        if( v >= VALUE )
            {
            HashLower(Position->Current->Hash, move, MAX(1, depth), v);
            return (v);
            }
        }

    if( ignored && best_value < -25000 )
        best_value = VALUE - 1;
    HashUpper(Position->Current->Hash, MAX(1, depth), best_value);
    return (best_value);
    }
