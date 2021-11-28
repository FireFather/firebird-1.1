#ifndef BUILD_top_analysis
#define BUILD_top_analysis
#include "firebird.h"
#define IsCheck   \
 (Position->wtm ? \
   (wBitboardK & Position->Current->bAtt) : (bBitboardK & Position->Current->wAtt))

typeRootMoveList RootMoveList[512];
#include "top_analysis.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyTopAnalysis( typePos *Position )
    {
    int i, k, depth, A, L, U, v, Value = 0, trans_depth;
    int move_depth = 0, EXACT_DEPTH = 0;
    int sm;
    uint32 move, Hash_MOVE = 0, EXACT_MOVE = 0, to, fr;
    typeMoveList *mlx, *ml, ML[512];
    typeRootMoveList *p, *q, *list;
    typeHash *rank;
    typePosition *TempPosition = Position->Current;
    int PieceValue[16] =
        {
        0, 1, 3, 0, 3, 3, 5, 9, 0, 1, 3, 0, 3, 3, 5, 9
        };
    sm = 0;

    EVAL(0);

    if( IsCheck )
        ml = MyEvasion(Position, ML, 0xffffffffffffffff);
    else
        {
        mlx = MyCapture(Position, ML, OppOccupied);
        ml = MyOrdinary(Position, mlx);
        SortOrdinary(ml, mlx, 0, 0, 0);
        }

    k = Position->Current->Hash & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        rank = HashTable + (k + i);

        if( (rank->hash ^ (Position->Current->Hash >> 32)) == 0 )
            {
            trans_depth = rank->DepthLower;
            move = rank->move;

            if( IsExact(rank) )
                {
                EXACT_DEPTH = trans_depth;
                EXACT_MOVE = move;
                Value = HashUpperBound(rank);
                }

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                Hash_MOVE = move;
                }
            }
        }

    for ( i = 0; i < ml - ML; i++ )
        RootMoveList[i].move = ML[i].move;
    RootMoveList[ml - ML].move = MoveNone;
    list = RootMoveList + (ml - ML);
    q = RootMoveList;

    for ( p = RootMoveList; p < list; p++ )
        {
        move = p->move & 0x7fff;
        Make(Position, move);
        EVAL(0);

        if( IllegalMove )
            {
            Undo(Position, move);
            continue;
            }

        if( Analysing && DoSearchMoves )
            {
            sm = 0;

            while( SearchMoves[sm] )
                {
                if( SearchMoves[sm] == move )
                    {
                    (q++)->move = move & 0x7fff;
                    break;
                    }
                sm++;
                }
            }
        else
            (q++)->move = move & 0x7fff;
        Undo(Position, move);
        }
    q->move = 0;
    list = q;

    for ( p = RootMoveList; p < list; p++ )
        {
        if( Position->sq[To(p->move)] )
            {
            to = Position->sq[To(p->move)];
            fr = Position->sq[From(p->move)];
            p->move |= 0xff000000 + ((16 * PieceValue[to] - PieceValue[fr]) << 16);
            }
        }

    for ( p = RootMoveList; p < list; p++ )
        if( p->move == Hash_MOVE )
            p->move |= 0xffff0000;

    for ( p = list - 1; p >= RootMoveList; p-- )
        {
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

    L = -ValueMate;
    U = ValueMate;

    if( !RootMoveList[0].move )
        {
        if( IsCheck )
            {
            RootScore = L;
            }
        else
            {
            RootScore = 0;
            }
        RootBestMove = 0;
        RootDepth = 0;
        return;
        }

    for ( depth = 2; depth <= 250; depth += 2 )
        {
        if( depth >= 14 && RootScore <= 25000 && -25000 <= RootScore && MultiPV == 1 )
            {
            A = 8;
            L = RootScore - A;
            U = RootScore + A;

            if( L < -25000 )
                L = -ValueMate;

            if( U > 25000 )
                U = ValueMate;
            AGAIN:
            v = MyAnalysis(Position, L, U, depth);

            if( v > L && v < U )
                goto CHECK_DONE;

            if( v <= L )
                {
                L -= A;
                A += A / 2;
                RootScore = L;
                goto AGAIN;
                }
            else
                {
                U += A;
                A += A / 2;
                RootScore = U;
                goto AGAIN;
                }
            }
        else
            v = MyAnalysis(Position, -ValueMate, ValueMate, depth);
        CHECK_DONE:
        RootPrevious = RootScore;
        CheckDone(Position, depth / 2);
        }
    }
