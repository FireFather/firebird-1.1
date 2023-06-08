#ifndef BUILD_top_analysis
#define BUILD_top_analysis
#include "firebird.h"
#include "control.h"
#define IsCheck   \
 (POSITION->wtm ? \
   (wBitboardK & POSITION->DYN->bAtt) : (bBitboardK & POSITION->DYN->wAtt))

typeRootMoveList ROOT_MOVE_LIST[512];
#include "top_analysis.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyTopAnalysis( typePOS *POSITION )
    {
    int i, k, depth, A, L, U, v, Value = 0, trans_depth;
    int move_depth = 0, EXACT_DEPTH = 0;
    int sm;
    uint32 move, HASH_MOVE = 0, EXACT_MOVE = 0, to, fr;
    typeMoveList *mlx, *ml, ML[512];
    typeRootMoveList *p, *q, *list;
    typeHash *trans;
    typeDYNAMIC *POS0 = POSITION->DYN;
    int PieceValue[16] =
        {
        0, 1, 3, 0, 3, 3, 5, 9, 0, 1, 3, 0, 3, 3, 5, 9
        };
    sm = 0;
    DECLARE();

    EVAL(0);

    if( IsCheck )
        ml = MyEvasion(POSITION, ML, 0xffffffffffffffff);
    else
        {
        mlx = MyCapture(POSITION, ML, OppOccupied);
        ml = MyOrdinary(POSITION, mlx);
        SortOrdinary(ml, mlx, 0, 0, 0);
        }

    k = POSITION->DYN->HASH & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        trans = HashTable + (k + i);

        if( (trans->hash ^ (POSITION->DYN->HASH >> 32)) == 0 )
            {
            trans_depth = trans->DepthLower;
            move = trans->move;

            if( IsExact(trans) )
                {
                EXACT_DEPTH = trans_depth;
                EXACT_MOVE = move;
                Value = HashUpperBound(trans);
                }

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                HASH_MOVE = move;
                }
            }
        }

    for ( i = 0; i < ml - ML; i++ )
        ROOT_MOVE_LIST[i].move = ML[i].move;
    ROOT_MOVE_LIST[ml - ML].move = MOVE_NONE;
    list = ROOT_MOVE_LIST + (ml - ML);
    q = ROOT_MOVE_LIST;

    for ( p = ROOT_MOVE_LIST; p < list; p++ )
        {
        move = p->move & 0x7fff;
        MAKE(POSITION, move);
        EVAL(0);

        if( ILLEGAL_MOVE )
            {
            UNDO(POSITION, move);
            continue;
            }

        if( ANALYSING && DO_SEARCH_MOVES )
            {
            sm = 0;

            while( SEARCH_MOVES[sm] )
                {
                if( SEARCH_MOVES[sm] == move )
                    {
                    (q++)->move = move & 0x7fff;
                    break;
                    }
                sm++;
                }
            }
        else
            (q++)->move = move & 0x7fff;
        UNDO(POSITION, move);
        }
    q->move = 0;
    list = q;

    for ( p = ROOT_MOVE_LIST; p < list; p++ )
        {
        if( POSITION->sq[TO(p->move)] )
            {
            to = POSITION->sq[TO(p->move)];
            fr = POSITION->sq[FROM(p->move)];
            p->move |= 0xff000000 + ((16 * PieceValue[to] - PieceValue[fr]) << 16);
            }
        }

    for ( p = ROOT_MOVE_LIST; p < list; p++ )
        if( p->move == HASH_MOVE )
            p->move |= 0xffff0000;

    for ( p = list - 1; p >= ROOT_MOVE_LIST; p-- )
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

    L = -VALUE_MATE;
    U = VALUE_MATE;

    if( !ROOT_MOVE_LIST[0].move )
        {
        if( IsCheck )
            {
            ROOT_SCORE = L;
            }
        else
            {
            ROOT_SCORE = 0;
            }
        ROOT_BEST_MOVE = 0;
        ROOT_DEPTH = 0;
        return;
        }

    for ( depth = 2; depth <= 250; depth += 2 )
        {
        if( depth >= 14 && ROOT_SCORE <= 25000 && -25000 <= ROOT_SCORE && MULTI_PV == 1 )
            {
            A = 8;
            L = ROOT_SCORE - A;
            U = ROOT_SCORE + A;

            if( L < -25000 )
                L = -VALUE_MATE;

            if( U > 25000 )
                U = VALUE_MATE;
            AGAIN:
            v = MyAnalysis(POSITION, L, U, depth);

            if( v > L && v < U )
                goto CHECK_DONE;

            if( v <= L )
                {
                L -= A;
                A += A / 2;
                ROOT_SCORE = L;
                goto AGAIN;
                }
            else
                {
                U += A;
                A += A / 2;
                ROOT_SCORE = U;
                goto AGAIN;
                }
            }
        else
            v = MyAnalysis(POSITION, -VALUE_MATE, VALUE_MATE, depth);
        CHECK_DONE:
        ROOT_PREVIOUS = ROOT_SCORE;
        CheckDone(POSITION, depth / 2);
        }
    }
