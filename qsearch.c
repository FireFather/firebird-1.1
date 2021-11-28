#ifndef BUILD_qsearch
#define BUILD_qsearch
#include "firebird.h"
#include "qsearch.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyQsearch( typePos *Position, int VALUE, int depth )
    {
    int Value, i, k = Position->Current->Hash & HashMask, v, best_value;
    uint32 Temp, move, trans_move = 0, trans_depth, move_depth = 0;
    uint64 TARGET;
    typeMoveList List[512], *list, *p, *q;
    typePosition *TempPosition = Position->Current;
    typeHash *rank;

    if( VALUE < -ValueMate + 1 )
        return (-ValueMate + 1);

    if( VALUE > ValueMate - 1 )
        return (ValueMate - 1);

    CheckRepetition;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 )
            {
            if( rank->DepthLower )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    return (Value);
                }

            if( rank->DepthUpper )
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

    best_value = TempPosition->Value + TempoValue2;

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

    list = MyCapture(Position, List, TARGET);
    p = List;

    while( p->move )
        {
        if( (p->move & 0x7fff) == trans_move )
            p->move |= 0xffff0000;
        p++;
        }
    p = List;

    while( p->move )
        {
        move = p->move;
        q = ++p;

        while( q->move )
            {
            if( move < q->move )
                {
                Temp = q->move;
                q->move = move;
                move = Temp;
                }
            q++;
            }

        if( !EasySEE(move) && (move & 0x7fff) != trans_move && SqSet[From(move)] & ~MyXray && !MySEE(Position, move) )
            continue;
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
            v = -OppQsearchCheck(Position, 1 - VALUE, depth - 1);

        else
            v = -OppQsearch(Position, 1 - VALUE, depth - 1);
        Undo(Position, move);
        CheckHalt();

        if( v <= best_value )
            continue;
        best_value = v;

        if( v >= VALUE )
            {
            HashLower(Position->Current->Hash, move, 1, v);
            return (v);
            }
        }

    if( depth >= -1 && TempPosition->Value >= VALUE - (100 + (12 << (depth + 4))) )
        {
        list = MyQuietChecks(Position, List, TARGET);

        for ( i = 0; i < list - List; i++ )
            {
            move = List[i].move;
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
            else
                v = -OppQsearchCheck(Position, 1 - VALUE, depth - 1);
            Undo(Position, move);
            CheckHalt();

            if( v <= best_value )
                continue;
            best_value = v;

            if( v >= VALUE )
                {
                HashLower(Position->Current->Hash, move, 1, v);
                return (v);
                }
            }
        }
    HashUpper(Position->Current->Hash, 1, best_value);
    return (best_value);
    }

int MyQsearchCheck( typePos *Position, int VALUE, int depth )
    {
    int ignored, Value, i, k = Position->Current->Hash & HashMask;
    int v, best_value, trans_depth, move_depth = 0;
    typeHash *rank;
    uint64 TARGET;
    typeMoveList List[512], *list, *p, *q;
    typePosition *TempPosition;
    uint32 move, Temp, trans_move = 0;

    TempPosition = Position->Current;

    if( VALUE < -ValueMate + 1 )
        return (-ValueMate + 1);

    if( VALUE > ValueMate - 1 )
        return (ValueMate - 1);

    CheckRepetition;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 )
            {
            if( rank->DepthLower )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    return (Value);
                }

            if( rank->DepthUpper )
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
    best_value = Height(Position) - ValueMate;
    TARGET = 0xffffffffffffffff;

    if( TempPosition->Value + PruneCheck < VALUE )
        {
        best_value = TempPosition->Value + PruneCheck;
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

    list = MyEvasion(Position, List, TARGET);

    if( (list - List) > 1 )
        depth--;
    p = List;

    while( p->move )
        {
        if( (p->move & 0x7fff) == trans_move )
            p->move |= 0xfff00000;
        p++;
        }
    p = List;
    ignored = 0;

    while( p->move )
        {
        move = p->move;
        q = ++p;

        while( q->move )
            {
            if( move < q->move )
                {
                Temp = q->move;
                q->move = move;
                move = Temp;
                }
            q++;
            }

        if( IsInterpose(move) && VALUE > -25000 && (move & 0x7fff) != trans_move && !MySEE(Position, move) )
            {
            ignored++;
            continue;
            }

        if( Position->sq[To(move)] == 0 && (move & 0x6000) == 0 && (move & 0x7fff)
            != trans_move && MyNull && MaxPositional(move)
            + TempPosition->Value < VALUE + 25 && VALUE > -25000 )
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
            v = -OppQsearchCheck(Position, 1 - VALUE, depth);

        else
            v = -OppQsearch(Position, 1 - VALUE, depth);
        Undo(Position, move);
        CheckHalt();

        if( v <= best_value )
            continue;
        best_value = v;

        if( v >= VALUE )
            {
            HashLower(Position->Current->Hash, move, 1, v);
            return (v);
            }
        }

    if( ignored && best_value < -25000 )
        best_value = VALUE - 1;
    HashUpper(Position->Current->Hash, 1, best_value);
    return (best_value);
    }
