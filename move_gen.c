#ifndef BUILD_move_gen
#define BUILD_move_gen

#include "firebird.h"
#include "init_gen.h"



void ResetHistory()
    {
    int pi, sq;

#ifdef MultiHistory
    int cpu;
    for ( cpu = 0; cpu < MaxCPUs; cpu++ )
#endif

	for ( pi = 0; pi < 0x10; pi++ )
		for ( sq = A1; sq <= H8; sq++ )

#ifdef MultiHistory
	HISTORY[cpu][pi][sq] = 0x800;
#else
    HISTORY[pi][sq] = 0x800;
#endif
    }

void SortOrdinary( typeMoveList *m1, typeMoveList *m2, uint32 s1, uint32 s2, uint32 s3 )
    {
    typeMoveList *p, *q;
    int move;

    if( m1 == m2 )
        return;

    for ( p = m2 - 1; p >= m1; p-- )
        {
        if( OK(p->move) )
            break;
        p->move = 0;
        }

    while( p > m1 )
        {
        p--;
        move = p->move;

        if( OK(move) )
            {
            for ( q = p + 1; q < m2; q++ )
                {
                if( move < q->move )
                    (q - 1)->move = q->move;
                else
                    break;
                }
            q--;
            q->move = move;
            }
        else
            {
            m2--;

            for ( q = p; q < m2; q++ )
                q->move = (q + 1)->move;
            m2->move = 0;
            }
        }
    }

#ifdef MultiplePosGain
#define AddGain(L, x, pi, to)                                             \
  { int v = ( (int) MaxPositionalGain[Position->cpu][pi][(x) & 07777]); \
    if (v >= av) (L++)->move = (x) | (v << 16); }
#else
#define AddGain(L, x, pi, to)                              \
  { int v = ( (int) MaxPositionalGain[pi][(x) & 07777]); \
    if (v >= av) (L++)->move = (x) | (v << 16); }

#endif
#define AddGainTo(T, pi)                           \
  { while (T)                                      \
      { to = LSB(T);                               \
    AddGain (List, (sq << 6) | to, pi, to); BitClear(to, T); } }
#define Sort                                       \
  for (p = List - 1; p >= sm; p--)                 \
    { move = p->move;                              \
      for (q = p + 1; q < List; q++)               \
    {                                              \
      if ( move<q->move ) (q - 1)->move = q->move; \
      else break;                                  \
    }                                              \
      q--;                                         \
      q->move = move; }

typeMoveList *EvasionMoves( typePos *Position, typeMoveList *list, uint64 mask )
    {
    if( Position->wtm )
        return WhiteEvasions(Position, list, mask);
    return BlackEvasions(Position, list, mask);
    }
typeMoveList *OrdinaryMoves( typePos *Position, typeMoveList *list )
    {
    if( Position->wtm )
        return WhiteOrdinary(Position, list);
    return BlackOrdinary(Position, list);
    }
typeMoveList *CaptureMoves( typePos *Position, typeMoveList *list, uint64 mask )
    {
    if( Position->wtm )
        return WhiteCaptures(Position, list, mask & bBitboardOcc);
    return BlackCaptures(Position, list, mask & wBitboardOcc);
    }
#include "move_gen.c"
#include "white.h"
#else
#include "black.h"

#endif

typeMoveList *MyEvasion( typePos *Position, typeMoveList *List, uint64 c2 )
    {
    uint64 U, T, att, mask;
    int sq, to, fr, c, king, pi;
    king = MyKingSq;
    att = MyKingCheck;
    sq = LSB(att);
    pi = Position->sq[sq];
    mask = ( ~OppAttacked) &(((pi == EnumOppP) ? AttK[king] : 0) | Evade[king][sq]) & ( ~MyOccupied) &c2;
    BitClear(sq, att);

    if( att )
        {
        sq = LSB(att);
        pi = Position->sq[sq];
        mask = mask &(PieceIsOppPawn(pi) | Evade[king][sq]);
        sq = king;
        AddTo(mask, CaptureValue[EnumMyK][c]);
        List->move = 0;
        return List;
        }
    c2 &= Interpose[king][sq];
    sq = king;
    AddTo(mask, CaptureValue[EnumMyK][c]);

    if( !c2 )
        {
        List->move = 0;
        return List;
        }

    if( CaptureRight &(c2 & OppOccupied) )
        {
        to = LSB(c2 & OppOccupied);
        c = Position->sq[to];

        if( EighthRank(to) )
            {
            Add(List, FlagPromQ | FromLeft(to) | to, (0x20 << 24) + CaptureValue[EnumMyP][c]);
            Add(List, FlagPromN | FromLeft(to) | to, 0);
            Add(List, FlagPromR | FromLeft(to) | to, 0);
            Add(List, FlagPromB | FromLeft(to) | to, 0);
            }
        else
            Add(List, FromLeft(to) | to, CaptureValue[EnumMyP][c]);
        }

    if( CaptureLeft &(c2 & OppOccupied) )
        {
        to = LSB(c2 & OppOccupied);
        c = Position->sq[to];

        if( EighthRank(to) )
            {
            Add(List, FlagPromQ | FromRight(to) | to, (0x20 << 24) + CaptureValue[EnumMyP][c]);
            Add(List, FlagPromN | FromRight(to) | to, 0);
            Add(List, FlagPromR | FromRight(to) | to, 0);
            Add(List, FlagPromB | FromRight(to) | to, 0);
            }
        else
            Add(List, FromRight(to) | to, CaptureValue[EnumMyP][c]);
        }
    to = Position->Current->ep;

    if( to )
        {
        if( CaptureRight & SqSet[to] && SqSet[Backward(to)] & c2 )
            Add(List, FlagEP | FromLeft(to) | to, CaptureValue[EnumMyP][EnumOppP]);

        if( CaptureLeft & SqSet[to] && SqSet[Backward(to)] & c2 )
            Add(List, FlagEP | FromRight(to) | to, CaptureValue[EnumMyP][EnumOppP]);
        }
    T = BitboardMyP & BackShift((c2 &OppOccupied) ^ c2);

    while( T )
        {
        fr = LSB(T);
        BitClear(fr, T);

        if( SeventhRank(fr) )
            {
            Add(List, FlagPromQ | (fr << 6) | Forward(fr), CaptureValue[EnumMyP][0]);
            Add(List, FlagPromN | (fr << 6) | Forward(fr), 0);
            Add(List, FlagPromR | (fr << 6) | Forward(fr), 0);
            Add(List, FlagPromB | (fr << 6) | Forward(fr), 0);
            }
        else
            Add(List, (fr << 6) | Forward(fr), CaptureValue[EnumMyP][0]);
        }

    T = BitboardMyP & BackShift2((c2 &OppOccupied) ^ c2) & SecondRank & BackShift( ~Position->OccupiedBW);

    while( T )
        {
        fr = LSB(T);
        BitClear(fr, T);
        Add(List, (fr << 6) | Forward2(fr), CaptureValue[EnumMyP][0]);
        }

    for ( U = BitboardMyN; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttN[sq] & c2;
        AddTo(T, CaptureValue[EnumMyN][c]);
        }

    for ( U = BitboardMyB; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttB(sq) & c2;
        AddTo(T, CaptureValue[EnumMyBL][c]);
        }

    for ( U = BitboardMyR; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttR(sq) & c2;
        AddTo(T, CaptureValue[EnumMyR][c]);
        }

    for ( U = BitboardMyQ; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttQ(sq) & c2;
        AddTo(T, CaptureValue[EnumMyQ][c]);
        }
    List->move = 0;
    return List;
    }
typeMoveList *MyPositionalGain( typePos *Position, typeMoveList *List, int av )
    {
    uint64 empty = ~Position->OccupiedBW, U, T;
    int to, sq;
    typeMoveList *sm, *p, *q;
    int move;
    sm = List;

    for ( U = ForwardShift(BitboardMyP & SecondSixthRanks) & empty; U; BitClear(sq, U) )
        {
        to = LSB(U);

        if( OnThirdRank(to) && Position->sq[Forward(to)] == 0 )
            AddGain(List, (Backward(to) << 6) | Forward(to), EnumMyP, Forward(to));
        AddGain(List, (Backward(to) << 6) | to, EnumMyP, to);
        }

    for ( U = BitboardMyN; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttN[sq] & empty;
        AddGainTo(T, EnumMyN);
        }

    for ( U = BitboardMyBL; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttB(sq) & empty;
        AddGainTo(T, EnumMyBL);
        }

    for ( U = BitboardMyBD; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttB(sq) & empty;
        AddGainTo(T, EnumMyBD);
        }

    for ( U = BitboardMyR; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttR(sq) & empty;
        AddGainTo(T, EnumMyR);
        }

    for ( U = BitboardMyQ; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttQ(sq) & empty;
        AddGainTo(T, EnumMyQ);
        }
    sq = MyKingSq;
    T = AttK[sq] & empty &( ~OppAttacked);
    AddGainTo(T, EnumMyK);
    List->move = 0;
    Sort;
    return List;
    }
typeMoveList *MyCapture( typePos *Position, typeMoveList *List, uint64 mask )
    {
    uint64 U, T, AttR, AttB;
    int sq, to, c;
    to = Position->Current->ep;

    if( to )
        {
        if( CaptureLeft & SqSet[to] )
            Add(List, FlagEP | FromRight(to) | to, CaptureEP);

        if( CaptureRight & SqSet[to] )
            Add(List, FlagEP | FromLeft(to) | to, CaptureEP);
        }

    if( (mask &MyAttacked) == 0 )
        goto NO_TARGET;

    T = CaptureLeft &( ~BitboardEighthRank) & mask;

    while( T )
        {
        to = LSB(T);
        c = Position->sq[to];
        Add(List, FromRight(to) | to, CaptureValue[EnumMyP][c]);
        BitClear(to, T);
        }
    T = CaptureRight &( ~BitboardEighthRank) & mask;

    while( T )
        {
        to = LSB(T);
        c = Position->sq[to];
        Add(List, FromLeft(to) | to, CaptureValue[EnumMyP][c]);
        BitClear(to, T);
        }

    for ( U = BitboardMyN; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttN[sq] & mask;
        AddTo(T, CaptureValue[EnumMyN][c]);
        }

    for ( U = BitboardMyB; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        AttB = AttB(sq);
        T = AttB & mask;
        AddTo(T, CaptureValue[EnumMyBL][c]);
        }

    for ( U = BitboardMyR; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        AttR = AttR(sq);
        T = AttR & mask;
        AddTo(T, CaptureValue[EnumMyR][c]);
        }

    for ( U = BitboardMyQ; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        AttR = AttR(sq);
        AttB = AttB(sq);
        T = (AttB | AttR) & mask;
        AddTo(T, CaptureValue[EnumMyQ][c]);
        }
    sq = LSB(BitboardMyK);
    T = AttK[sq] & mask;
    AddTo(T, CaptureValue[EnumMyK][c]);
    NO_TARGET:
    for ( U = BitboardMyP & BitboardSeventhRank; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        to = Forward(sq);

        if( Position->sq[to] == 0 )
            {
            Add(List, FlagPromQ | (sq << 6) | to, PtoQ);

            if( AttN[to] & BitboardOppK )
                Add(List, FlagPromN | (sq << 6) | to, PtoN);
            }
        to = ForwardLeft(sq);

        if( sq != WhiteA7 && SqSet[to] & mask )
            {
            c = Position->sq[to];
            Add(List, FlagPromQ | (sq << 6) | to, PromQueenCap);

            if( AttN[to] & BitboardOppK )
                Add(List, FlagPromN | (sq << 6) | to, PromKnightCap);
            }
        to = ForwardRight(sq);

        if( sq != WhiteH7 && SqSet[to] & mask )
            {
            c = Position->sq[to];
            Add(List, FlagPromQ | (sq << 6) | to, PromQueenCap);

            if( AttN[to] & BitboardOppK )
                Add(List, FlagPromN | (sq << 6) | to, PromKnightCap);
            }
        }
    List->move = 0;
    return List;
    }
typeMoveList *MyOrdinary( typePos *Position, typeMoveList *List )
    {
    uint64 empty = ~Position->OccupiedBW, U, T, ROOK, BISHOP, Pawn;
    int to, sq, requ = OppKingSq;

    if( CastleOO && ((Position->OccupiedBW | OppAttacked) & WhiteF1G1) == 0 )
        MoveAdd(List, FlagOO | (WhiteE1 << 6) | WhiteG1, EnumMyK, WhiteG1, 0);

    if( CastleOOO && (Position->OccupiedBW &WhiteB1C1D1) == 0 && (OppAttacked &WhiteC1D1) == 0 )
        MoveAdd(List, FlagOO | (WhiteE1 << 6) | WhiteC1, EnumMyK, WhiteC1, 0);

    Pawn = MyAttackedPawns[requ];

    if( BitboardMyQ | BitboardMyR )
        ROOK = AttR(requ);

    if( BitboardMyQ | BitboardMyB )
        BISHOP = AttB(requ);

    for ( U = ForwardShift(BitboardMyP & SecondSixthRanks) & empty; U; BitClear(sq, U) )
        {
        to = LSB(U);

        if( OnThirdRank(to) && Position->sq[Forward(to)] == 0 )
            MoveAdd(List, (Backward(to) << 6) | Forward(to), EnumMyP, Forward(to), Pawn);
        MoveAdd(List, (Backward(to) << 6) | to, EnumMyP, to, Pawn);
        }

    for ( U = BitboardMyQ; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttQ(sq) & empty;
        MovesTo(T, EnumMyQ, ROOK | BISHOP);
        }

    for ( U = BitboardMyR; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttR(sq) & empty;
        MovesTo(T, EnumMyR, ROOK);
        }

    for ( U = BitboardMyB; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttB(sq) & empty;
        MovesTo(T, ((SqSet[sq]&Black) ? EnumMyBD : EnumMyBL), BISHOP);
        }
    sq = LSB(BitboardMyK);
    T = AttK[sq] & empty &( ~OppAttacked);
    MovesTo(T, EnumMyK, 0);

    for ( U = BitboardMyN; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttN[sq] & empty;
        MovesTo(T, EnumMyN, AttN[requ]);
        }

    for ( U = BitboardMyP & BitboardSeventhRank; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        to = Forward(sq);

        if( Position->sq[to] == 0 )
            UnderProm();
        to = ForwardLeft(sq);

        if( sq != WhiteA7 && SqSet[to] & OppOccupied )
            UnderProm();
        to = ForwardRight(sq);

        if( sq != WhiteH7 && SqSet[to] & OppOccupied )
            UnderProm();
        }
    List->move = 0;
    return List;
    }

typeMoveList *MyQuietChecks( typePos *Position, typeMoveList *List, uint64 mask )
    {
    int requ, king, sq, to, fr, pi;
    uint64 U, T, V;
    typeMoveList *list;
    uint32 move;
    uint64 gcm;
    gcm = ~MyXray;
    mask = ( ~mask) &~MyOccupied;
    ;
    list = List;
    king = OppKingSq;
    list = List;

    for ( U = MyXray & MyOccupied; U; BitClear(fr, U) )
        {
        fr = LSB(U);
        pi = Position->sq[fr];

        if( pi == EnumMyP )
            {
            if( FILE(fr) != FILE(king) && !SeventhRank(fr) && Position->sq[Forward(fr)] == 0 )
                {
                (List++)->move = (fr << 6) | Forward(fr);

                if( OnSecondRank(fr) && Position->sq[Forward2(fr)] == 0 )
                    (List++)->move = (fr << 6) | Forward2(fr);
                }

            if( CanCaptureRight )
                (List++)->move = (fr << 6) | ForwardRight(fr);

            if( CanCaptureLeft )
                (List++)->move = (fr << 6) | ForwardLeft(fr);
            }
        else if( pi == EnumMyN )
            {
            V = AttN[fr] & mask;

            while( V )
                {
                to = LSB(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        else if( pi == EnumMyBL || pi == EnumMyBD )
            {
            V = AttB(fr) & mask;

            while( V )
                {
                to = LSB(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        else if( pi == EnumMyR )
            {
            V = AttR(fr) & mask;

            while( V )
                {
                to = LSB(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        else if( pi == EnumMyK )
            {
            if( FILE(fr) == FILE(king) || RANK(fr) == RANK(king) )
                V = AttK[fr] & NonOrtho[king] & mask &( ~OppAttacked);
            else
                V = AttK[fr] & NonDiag[king] & mask &( ~OppAttacked);

            while( V )
                {
                to = LSB(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        }

    requ = OppKingSq;
    T = CaptureLeft &( ~BitboardEighthRank) & mask & OppOccupied & MyAttackedPawns[requ];

    while( T )
        {
        to = LSB(T);
        (List++)->move = FromRight(to) | to;
        BitClear(to, T);
        }
    T = CaptureRight &( ~BitboardEighthRank) & mask & OppOccupied & MyAttackedPawns[requ];

    while( T )
        {
        to = LSB(T);
        (List++)->move = FromLeft(to) | to;
        BitClear(to, T);
        }

    for ( U = BitboardMyQ; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttQ(sq) & AttQ(king) & mask;

        while( T )
            {
            to = LSB(T);
            BitClear(to, T);

            if( (OppAttackedPawns[to]&BitboardOppP &gcm) == 0 && (AttN[to]&BitboardOppN &gcm) == 0 )
                {
                move = (sq << 6) | to;

                if( MySEE(Position, move) )
                    (List++)->move = (sq << 6) | to;
                }
            }
        }

    for ( U = BitboardMyR; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttR(sq) & AttR(king) & mask;

        while( T )
            {
            to = LSB(T);
            BitClear(to, T);

            if( (OppAttackedPawns[to]&BitboardOppP &gcm) == 0 && (AttN[to]&BitboardOppN &gcm) == 0 )
                {
                move = (sq << 6) | to;

                if( MySEE(Position, move) )
                    (List++)->move = (sq << 6) | to;
                }
            }
        }

    for ( U = BitboardMyB; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttB(sq) & AttB(king) & mask;

        while( T )
            {
            to = LSB(T);
            BitClear(to, T);

            if( (OppAttackedPawns[to]&BitboardOppP &gcm) == 0 )
                {
                move = (sq << 6) | to;

                if( MySEE(Position, move) )
                    (List++)->move = (sq << 6) | to;
                }
            }
        }

    for ( U = BitboardMyN; U; BitClear(sq, U) )
        {
        sq = LSB(U);
        T = AttN[sq] & AttN[king] & mask;

        while( T )
            {
            to = LSB(T);
            BitClear(to, T);

            if( (OppAttackedPawns[to]&BitboardOppP &gcm) == 0 )
                {
                move = (sq << 6) | to;

                if( MySEE(Position, move) )
                    (List++)->move = (sq << 6) | to;
                }
            }
        }

    if( BitboardOppK & FourthEighthRankNoH && Position->sq[BackRight(requ)] == 0 )
        {
        if( Position->sq[BackRight2(requ)] == EnumMyP )
            {
            fr = BackRight2(requ);
            to = BackRight(requ);
            move = (fr << 6) | to;

            if( PawnGuard(to, fr) && MySEE(Position, move) )
                (List++)->move = move;
            }

        if( RANK(requ) == NumberRank5 && Position->sq[BackRight2(requ)] == 0
            && Position->sq[BackRight3(requ)] == EnumMyP )
            {
            to = BackRight(requ);
            fr = BackRight3(requ);
            move = (fr << 6) | to;

            if( PawnGuard(to, fr) && MySEE(Position, move) )
                (List++)->move = move;
            }
        }

    if( BitboardOppK & FourthEighthRankNoA && Position->sq[BackLeft(requ)] == 0 )
        {
        if( Position->sq[BackLeft2(requ)] == EnumMyP )
            {
            fr = BackLeft2(requ);
            to = BackLeft(requ);
            move = (fr << 6) | to;

            if( PawnGuard(to, fr) && MySEE(Position, move) )
                (List++)->move = move;
            }

        if( RANK(requ) == NumberRank5 && Position->sq[BackLeft2(requ)] == 0
            && Position->sq[BackLeft3(requ)] == EnumMyP )
            {
            to = BackLeft(requ);
            fr = BackLeft3(requ);
            move = (fr << 6) | to;

            if( PawnGuard(to, fr) && MySEE(Position, move) )
                (List++)->move = move;
            }
        }
    List->move = MoveNone;
    return List;
    }
