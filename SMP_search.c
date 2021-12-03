#ifndef BUILD_IVAN_SMP_PV
#define BUILD_IVAN_SMP_PV
#include "firebird.h"

static INLINE void SMP_BAD_HISTORY( typePOS *POS, uint32 m, SPLITPOINT *sp )
    {
    if( (POS->DYN + 1)->cp == 0 && MoveHistory(m) )
        {
        int sv = HISTORY_VALUE(POS, m);

        if( POS->DYN->Value > sp->alpha - 50 )
            HISTORY_VALUE(POS, m) = sv - ((sv * sp->depth) >> 8);
        }
    }

#include "SMP_search.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyPVNodeSMP( typePOS *POSITION )
    {
    int v;
    int alpha;
    int beta;
    int m;
    typeNEXT *NextMove;
    SPLITPOINT *sp;
    int EXTEND;
    int to;
    int new_depth;

    sp = POSITION->SplitPoint;

    while( true )
        {
        LOCK(sp->splock);
        beta = sp->beta;
        alpha = sp->alpha;

        if( sp->tot )
            {
            UNLOCK(sp->splock);
            return;
            }
        NextMove = sp->MOVE_PICK;
        m = MyNext(POSITION, NextMove);

        if( !m )
            {
            NextMove->phase = FASE_0;
            UNLOCK(sp->splock);
            return;
            }
        UNLOCK(sp->splock);
        MAKE(POSITION, m);
        Eval(POSITION, -0x7fff0000, 0x7fff0000, m);

        if( MyKingCheck )
            {
            UNDO(POSITION, m);
            continue;
            }
        EXTEND = 0;
        to = TO(m);

        if( PassedPawnPush(to, SIXTH_RANK(to)) )
            EXTEND = 2;
        else
            {
            if( POSITION->DYN->cp != 0 || OppKingCheck != 0 )
                EXTEND = 1;

            else if( PassedPawnPush(to, FOURTH_RANK(to)) )
                EXTEND = 1;
            }
        new_depth = sp->depth - 2 + EXTEND;

        if( OppKingCheck )
            v = -OppCutCheck(POSITION, -alpha, new_depth);
        else
            v = -OppCut(POSITION, -alpha, new_depth);

        if( v <= alpha )
            {
            UNDO(POSITION, m);

            if( POSITION->stop )
                return;

            SMP_BAD_HISTORY(POSITION, m, sp);
            continue;
            }

        if( !sp->tot && !POSITION->stop )
            {
            boolean b = (OppKingCheck != 0);
            v = -OppPV(POSITION, -beta, -alpha, new_depth, b);
            UNDO(POSITION, m);

            if( POSITION->stop )
                return;

            if( v > alpha )
                {
                LOCK(sp->splock);

                if( v > sp->alpha )
                    {
                    sp->alpha = v;
                    sp->value = v;
                    sp->good_move = m;
                    }
                UNLOCK(sp->splock);
                HashLower(POSITION->DYN->HASH, m, sp->depth, v);
                }
            }
        else
            UNDO(POSITION, m);

        if( POSITION->stop )
            return;

        if( v >= beta )
            {
            ivan_fail_high(sp, POSITION, m);
            return;
            }
        }
    }
void MyAllSMP( typePOS *POSITION )
    {
    int v;
    int m;
    typeNEXT *NextMove;
    SPLITPOINT *sp;
    int scout, depth, ph, c;
    sp = POSITION->SplitPoint;
    scout = sp->beta;
    depth = sp->depth;

    while( true )
        {
        LOCK(sp->splock);

        if( sp->tot )
            {
            UNLOCK(sp->splock);
            return;
            }
        NextMove = sp->MOVE_PICK;
        m = MyNext(POSITION, NextMove) & 0x7fff;
        ph = NextMove->phase;
        c = NextMove->move;

        if( !m )
            {
            NextMove->phase = FASE_0;
            UNLOCK(sp->splock);
            return;
            }
        UNLOCK(sp->splock);

        if( m == NextMove->exclude )
            continue;
        MAKE(POSITION, m);
        Eval(POSITION, -0x7fff0000, 0x7fff0000, m);

        if( MyKingCheck )
            {
            UNDO(POSITION, m);
            continue;
            }
        m &= 0x7fff;

        if( OppKingCheck )
            {
            v = -OppCutCheck(POSITION, 1 - scout, depth - 1);
            }
        else
            {
            int to = TO(m);
            int EXTEND = 0;

            if( PassedPawnPush(to, SIXTH_RANK(to)) )
                EXTEND = 1;

            if( ph == ORDINARY_MOVES && !EXTEND )
                {
                int REDUCTION = 2 + MSB(2 + c);
                int nuovo_abisso = MAX(8, depth - REDUCTION);
                v = -OppCut(POSITION, 1 - scout, nuovo_abisso);

                if( v < scout )
                    goto I;
                }
            v = -OppCut(POSITION, 1 - scout, depth - 2 + EXTEND);
            }
        I:
        UNDO(POSITION, m);

        if( POSITION->stop )
            return;

        if( v >= scout )
            {
            ivan_fail_high(sp, POSITION, m);
            return;
            }
        SMP_BAD_HISTORY(POSITION, m, sp);
        }
    }

void MyCutSMP( typePOS *POSITION )
    {
    int v;
    int m;
    typeNEXT *NextMove;
    SPLITPOINT *sp;
    int scout, depth, ph, c;
    sp = POSITION->SplitPoint;
    scout = sp->beta;
    depth = sp->depth;

    while( true )
        {
        LOCK(sp->splock);

        if( sp->tot )
            {
            UNLOCK(sp->splock);
            return;
            }
        NextMove = sp->MOVE_PICK;
        m = MyNext(POSITION, NextMove);
        ph = NextMove->phase;
        c = NextMove->move;

        if( !m )
            {
            NextMove->phase = FASE_0;
            UNLOCK(sp->splock);
            return;
            }
        UNLOCK(sp->splock);
        MAKE(POSITION, m);
        Eval(POSITION, -0x7fff0000, 0x7fff0000, m);

        if( MyKingCheck )
            {
            UNDO(POSITION, m);
            continue;
            }
        m &= 0x7fff;

        if( OppKingCheck )
            {
            v = -OppAllCheck(POSITION, 1 - scout, depth - 1);
            }
        else
            {
            int to = TO(m);
            int EXTEND = 0;

            if( PassedPawnPush(to, SIXTH_RANK(to)) )
                EXTEND = 1;

            if( ph == ORDINARY_MOVES && !EXTEND )
                {
                int REDUCTION = 4 + MSB(5 + c);
                int nuovo_abisso = MAX(8, depth - REDUCTION);
                v = -OppAll(POSITION, 1 - scout, nuovo_abisso);

                if( v < scout )
                    goto I;
                }
            v = -OppAll(POSITION, 1 - scout, depth - 2 + EXTEND);
            }
        I:
        UNDO(POSITION, m);

        if( POSITION->stop )
            return;

        if( v >= scout )
            {
            ivan_fail_high(sp, POSITION, m);
            return;
            }
        SMP_BAD_HISTORY(POSITION, m, sp);
        }
    }
