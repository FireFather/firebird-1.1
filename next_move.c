#ifndef BUILD_next_move
#define BUILD_next_move
#include "firebird.h"
#include "next_move.c"
#include "white.h"
#else
#include "black.h"
#endif

uint32 MyNext( typePos *Position, typeNext *NextMove )
    {
    typeMoveList *p, *q, *list;
    uint32 move, Temp;

    switch( NextMove->phase )
        {
        case Trans:
            NextMove->phase = CaptureGen;
            if( NextMove->trans_move && MyOK(Position, NextMove->trans_move) )
                return(NextMove->trans_move);

        case CaptureGen:
            NextMove->phase = Capture_Moves;
            NextMove->move = 0;
            MyCapture(Position, NextMove->List, OppOccupied);

        case Capture_Moves:
            while( 1 )
                {
                p = NextMove->List + NextMove->move;
                move = p->move;

                if( !move )
                    break;
                q = p + 1;
                NextMove->move++;

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

                if( (move & 0x7fff) == NextMove->trans_move )
                    continue;

                if( !EasySEE(move) && !MySEE(Position, move) )
                    NextMove->BadCaps[NextMove->bc++] = move;
                else
                    break;
                }
            if( move )
                return(move);
            NextMove->phase = Killer1;
            move = Position->Current->killer1;
            if( move && move != NextMove->trans_move && Position->sq[To(move)] == 0 && MyOK(Position, move) )
                return(move);

        case Killer1:
            NextMove->phase = Killer2;
            move = Position->Current->killer2;
            if( move && move != NextMove->trans_move && Position->sq[To(move)] == 0 && MyOK(Position, move) )
                return(move);

        case Killer2:
            NextMove->phase = Ordinary_Moves;
            NextMove->move = 0;
            list = MyOrdinary(Position, NextMove->List);
            SortOrdinary(NextMove->List, list, NextMove->trans_move, Position->Current->killer1, Position->Current->killer2);

        case Ordinary_Moves:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            if( move )
                return(move);
            NextMove->phase = BadCaps;
            NextMove->BadCaps[NextMove->bc] = 0;
            NextMove->move = 0;

        case BadCaps:
            move = NextMove->BadCaps[NextMove->move++];
            return(move);

        case Trans2:
            NextMove->phase = CaptureGen2;
            if( NextMove->trans_move && MyOK(Position, NextMove->trans_move) )
                return(NextMove->trans_move);

        case CaptureGen2:
            NextMove->phase = CaptureMoves2;
            NextMove->move = 0;
            MyCapture(Position, NextMove->List, NextMove->TARGET);

        case CaptureMoves2:
            while( 1 )
                {
                p = NextMove->List + NextMove->move;
                move = p->move;

                if( !move )
                    break;
                q = p + 1;
                NextMove->move++;

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

                if( (move & 0x7fff) == NextMove->trans_move )
                    continue;
                else
                    break;
                }
            if( move )
                return(move);
            NextMove->move = 0;
            NextMove->phase = QuietChecks;
            MyQuietChecks(Position, NextMove->List, NextMove->TARGET);

        case QuietChecks:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            return(move);

        case Evade_Phase:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            return(move);

        case Trans3:
            NextMove->phase = CaptureGen3;
            if( NextMove->trans_move && MyOK(Position, NextMove->trans_move) )
                return(NextMove->trans_move);

        case CaptureGen3:
            NextMove->phase = CaptureMoves3;
            NextMove->move = 0;
            MyCapture(Position, NextMove->List, OppOccupied);

        case CaptureMoves3:
            while( 1 )
                {
                p = NextMove->List + NextMove->move;
                move = p->move;

                if( !move )
                    break;
                q = p + 1;
                NextMove->move++;

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

                if( (move & 0x7fff) == NextMove->trans_move )
                    continue;
                else
                    break;
                }
            if( move )
                return(move);
            NextMove->move = 0;
            NextMove->phase = QuietChecks3;
            MyQuietChecks(Position, NextMove->List, OppOccupied);

        case QuietChecks3:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            if( move )
                return(move);
            NextMove->move = 0;
            NextMove->phase = PositionalGainPhase;
            MyPositionalGain(Position, NextMove->List, NextMove->mask);

        case PositionalGainPhase:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            return(move);

        case Fase0:
            return(0);
        }
    return 0;
    }
