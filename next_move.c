#ifndef BUILD_next_move
#define BUILD_next_move
#include "firebird.h"
#include "next_move.c"
#include "white.h"
#else
#include "black.h"
#endif

uint32 MyNext( typePOS *POSITION, typeNEXT *NextMove )
    {
    typeMoveList *p, *q, *list;
    uint32 move, TEMP;

    switch( NextMove->phase )
        {
        case TRANS:
            NextMove->phase = CAPTURE_GEN;
            if( NextMove->trans_move && MyOK(POSITION, NextMove->trans_move) )
                return(NextMove->trans_move);

        case CAPTURE_GEN:
            NextMove->phase = CAPTURE_MOVES;
            NextMove->move = 0;
            MyCapture(POSITION, NextMove->LIST, OppOccupied);

        case CAPTURE_MOVES:
            while( 1 )
                {
                p = NextMove->LIST + NextMove->move;
                move = p->move;

                if( !move )
                    break;
                q = p + 1;
                NextMove->move++;

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

                if( (move & 0x7fff) == NextMove->trans_move )
                    continue;

                if( !EasySEE(move) && !MySEE(POSITION, move) )
                    NextMove->BAD_CAPS[NextMove->bc++] = move;
                else
                    break;
                }
            if( move )
                return(move);
            NextMove->phase = KILLER1;
            move = POSITION->DYN->killer1;
            if( move && move != NextMove->trans_move && POSITION->sq[TO(move)] == 0 && MyOK(POSITION, move) )
                return(move);

        case KILLER1:
            NextMove->phase = KILLER2;
            move = POSITION->DYN->killer2;
            if( move && move != NextMove->trans_move && POSITION->sq[TO(move)] == 0 && MyOK(POSITION, move) )
                return(move);

        case KILLER2:
            NextMove->phase = ORDINARY_MOVES;
            NextMove->move = 0;
            list = MyOrdinary(POSITION, NextMove->LIST);
            SortOrdinary(NextMove->LIST, list, NextMove->trans_move, POSITION->DYN->killer1, POSITION->DYN->killer2);

        case ORDINARY_MOVES:
            move = (NextMove->LIST + NextMove->move)->move;
            NextMove->move++;
            if( move )
                return(move);
            NextMove->phase = BAD_CAPS;
            NextMove->BAD_CAPS[NextMove->bc] = 0;
            NextMove->move = 0;

        case BAD_CAPS:
            move = NextMove->BAD_CAPS[NextMove->move++];
            return(move);

        case TRANS2:
            NextMove->phase = CAPTURE_PGEN2;
            if( NextMove->trans_move && MyOK(POSITION, NextMove->trans_move) )
                return(NextMove->trans_move);

        case CAPTURE_PGEN2:
            NextMove->phase = CAPTURE_MOVES2;
            NextMove->move = 0;
            MyCapture(POSITION, NextMove->LIST, NextMove->TARGET);

        case CAPTURE_MOVES2:
            while( 1 )
                {
                p = NextMove->LIST + NextMove->move;
                move = p->move;

                if( !move )
                    break;
                q = p + 1;
                NextMove->move++;

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

                if( (move & 0x7fff) == NextMove->trans_move )
                    continue;
                else
                    break;
                }
            if( move )
                return(move);
            NextMove->move = 0;
            NextMove->phase = QUIET_CHECKS;
            MyQuietChecks(POSITION, NextMove->LIST, NextMove->TARGET);

        case QUIET_CHECKS:
            move = (NextMove->LIST + NextMove->move)->move;
            NextMove->move++;
            return(move);

        case EVADE_PHASE:
            move = (NextMove->LIST + NextMove->move)->move;
            NextMove->move++;
            return(move);

        case TRANS3:
            NextMove->phase = CAPTURE_GEN3;
            if( NextMove->trans_move && MyOK(POSITION, NextMove->trans_move) )
                return(NextMove->trans_move);

        case CAPTURE_GEN3:
            NextMove->phase = CAPTURE_MOVES3;
            NextMove->move = 0;
            MyCapture(POSITION, NextMove->LIST, OppOccupied);

        case CAPTURE_MOVES3:
            while( 1 )
                {
                p = NextMove->LIST + NextMove->move;
                move = p->move;

                if( !move )
                    break;
                q = p + 1;
                NextMove->move++;

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

                if( (move & 0x7fff) == NextMove->trans_move )
                    continue;
                else
                    break;
                }
            if( move )
                return(move);
            NextMove->move = 0;
            NextMove->phase = QUIET_CHECKS3;
            MyQuietChecks(POSITION, NextMove->LIST, OppOccupied);

        case QUIET_CHECKS3:
            move = (NextMove->LIST + NextMove->move)->move;
            NextMove->move++;
            if( move )
                return(move);
            NextMove->move = 0;
            NextMove->phase = POSITIONAL_GAIN_PHASE;
            MyPositionalGain(POSITION, NextMove->LIST, NextMove->mask);

        case POSITIONAL_GAIN_PHASE:
            move = (NextMove->LIST + NextMove->move)->move;
            NextMove->move++;
            return(move);

        case FASE_0:
            return(0);
        }
    return 0;
    }
