#include "firebird.h"
#include "kpk.h"

void ResetPositionalGain()
    {
    int p, m;

#ifdef MultiPosGain
    int cpu;
    for ( cpu = 0; cpu < MaxCPUs; cpu++ )
#endif

	for ( p = 0; p < 0x10; p++ )
		for ( m = 0; m < 010000; m++ )

#ifdef MultiPosGain
	MaxPositionalGain[cpu][p][m] = 0;
#else
    MaxPositionalGain[p][m] = 0;
#endif

    }

static void AdjustPositionalGainB( typePos *Position, int move )
    {
    int v, p, m;

    if( Position->Current->cp )
        return;

    p = Position->sq[To(move)];
    m = move & 07777;
    v = Position->Current->PositionalValue - ((Position->Current - 1)->PositionalValue);

    if( MaxPosGain(p, m) <= v )
        MaxPosGain(p, m) = v;
    else
        MaxPosGain(p, m)--;
    }
static void AdjustPositionalGainW( typePos *Position, int move )
    {
    int v, p, m;

    if( Position->Current->cp )
        return;

    p = Position->sq[To(move)];
    m = move & 07777;
    v = ((Position->Current - 1)->PositionalValue) - Position->Current->PositionalValue;

    if( MaxPosGain(p, m) <= v )
        MaxPosGain(p, m) = v;
    else
        MaxPosGain(p, m)--;
    }

void EvalHashClear()
    {
    int c;

    for ( c = 0; c < EvalHashSize; c++ )
        EvalHash[c] = 0;
    }

static int MaterialValue( typePos *Position )
    {
    int Value = QValue * (POPCNT(wBitboardQ) - POPCNT(bBitboardQ));
    Value += RValue * (POPCNT(wBitboardR) - POPCNT(bBitboardR));
    Value += BValue * (POPCNT(wBitboardB) - POPCNT(bBitboardB));
    Value += NValue * (POPCNT(wBitboardN) - POPCNT(bBitboardN));
    Value += PValue * (POPCNT(wBitboardP) - POPCNT(bBitboardP));

    if( wBitboardBL && wBitboardBD )
        Value += BPValue;

    if( bBitboardBL && bBitboardBD )
        Value -= BPValue;
    return Value;
    }
static void KingPawnWhite( typePos *Position, int matval, uint8 Token, typePawnEval *PawnInfo )
    {
    int Value, WhiteLeader, BlackLeader, sq, rank;
    uint8 C;
    uint64 A, T;

    if( PawnInfo->PawnHash != Position->Current->PawnHash )
        PawnEval(Position, PawnInfo);
    Position->Current->wXray = Position->Current->bXray = 0;
    Value = ((Position->Current->PST)+(PawnInfo->score));
    Value = (sint16)(Value & 0xffff);
    WhiteLeader = 0;
    C = PawnInfo->wPassedFiles;

    while( C )
        {
        sq = MSB(FileArray[LSB(C)] & wBitboardP);
        rank = RANK(sq);
        BitClear(0, C);

        if( (ShepherdWK[sq]&wBitboardK) == 0 )
            {
            if( wBitboardOcc & OpenFileW[sq] || (bBitboardK &QuadrantBKwtm[sq]) == 0 )
                continue;

            if( WhiteLeader <= rank )
                WhiteLeader = rank;
            }

        else if( WhiteLeader <= rank )
            WhiteLeader = rank;
        }
    BlackLeader = 0;
    C = PawnInfo->bPassedFiles;

    while( C )
        {
        sq = LSB(FileArray[LSB(C)] & bBitboardP);
        rank = R8 - RANK(sq);
        BitClear(0, C);

        if( (ShepherdBK[sq]&bBitboardK) == 0 )
            {
            if( bBitboardOcc & OpenFileB[sq] || (wBitboardK &QuadrantWKwtm[sq]) == 0 )
                continue;

            if( BlackLeader <= rank )
                BlackLeader = rank;
            }

        else if( BlackLeader <= rank )
            BlackLeader = rank;
        }
    Position->Current->Value = (Token * (Value + matval)) / 128;
    Position->Current->bKcheck = Position->Current->wKcheck = 0;

    if( WhiteLeader > BlackLeader && (bBitboardP &InFrontB[R8 - WhiteLeader + 1]) == 0 )
        Position->Current->Value += 150 + 50 * WhiteLeader;

    if( BlackLeader > WhiteLeader + 1 && (wBitboardP &InFrontW[BlackLeader - 2]) == 0 )
        Position->Current->Value -= 150 + 50 * BlackLeader;

    A = (wBitboardP &( ~FileA)) << 7;
    T = A & bBitboardK;
    Position->Current->bKcheck |= (T >> 7);
    Position->Current->wAtt = A;
    A = (wBitboardP &( ~FileH)) << 9;
    T = A & bBitboardK;
    Position->Current->bKcheck |= (T >> 9);
    Position->Current->wAtt |= A | AttK[Position->wKsq];
    A = (bBitboardP &( ~FileH)) >> 7;
    T = A & wBitboardK;
    Position->Current->wKcheck |= (T << 7);
	Position->Current->bAtt = A;
    A = (bBitboardP &( ~FileA)) >> 9;
    T = A & wBitboardK;
    Position->Current->wKcheck |= (T << 9);
	Position->Current->bAtt |= A | AttK[Position->bKsq];

    if( bBitboardK & AttK[Position->wKsq] )
        {
        Position->Current->bKcheck |= SqSet[Position->wKsq];
        Position->Current->wKcheck |= SqSet[Position->bKsq];
        }

    if( Position->Current->Value > 0 && !wBitboardP )
        Position->Current->Value = 0;

    if( Position->Current->Value < 0 && !bBitboardP )
        Position->Current->Value = 0;

    if( Position->Current->Value > 0 )
        {
        if( (wBitboardP & ~FileH) == 0 && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8] )
            Position->Current->Value = 0;

        if( (wBitboardP & ~FileA) == 0 && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8] )
            Position->Current->Value = 0;

        if( (Position->Current->flags & 28) == 28 )
            {
            sq = LSB(wBitboardP);
            rank = RANK(sq);
            Value = kpk_white[384 * Position->wKsq + 6 * Position->bKsq + rank - 1]&(1 << FILE(sq));

            if( !Value )
                Position->Current->Value = 0;
            else
                Position->Current->Value = ((sint16)(Position->Current->PST & 0xffff)) + 75 * rank + 250;
            }
        }

    if( Position->Current->Value < 0 )
        {
        if( (bBitboardP & ~FileH) == 0 && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1] )
            Position->Current->Value = 0;

        if( (bBitboardP & ~FileA) == 0 && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1] )
            Position->Current->Value = 0;

        if( (Position->Current->flags & 28) == 28 )
            {
            sq = H8 - MSB(bBitboardP);
            rank = RANK(sq);
            Value = kpk_black[384 * (H8 - Position->bKsq) + 6 * (H8 - Position->wKsq) + rank - 1]&(1 << FILE(sq));

            if( !Value )
                Position->Current->Value = 0;
            else
                Position->Current->Value = ((sint16)(Position->Current->PST & 0xffff)) - 75 * rank - 250;
            }
        }
    }
static void KingPawnBlack( typePos *Position, int matval, uint8 Token, typePawnEval *PawnInfo )
    {
    int Value, WhiteLeader, BlackLeader, sq, rank;
    uint8 C;
    uint64 A, T;

    if( PawnInfo->PawnHash != Position->Current->PawnHash )
        PawnEval(Position, PawnInfo);
    Position->Current->wXray = Position->Current->bXray = 0;
    Value = ((Position->Current->PST)+(PawnInfo->score));
    Value = (sint16)(Value & 0xffff);
    WhiteLeader = 0;
    C = PawnInfo->wPassedFiles;

    while( C )
        {
        sq = MSB(FileArray[LSB(C)] & wBitboardP);
        rank = RANK(sq);
        BitClear(0, C);

        if( (ShepherdWK[sq]&wBitboardK) == 0 )
            {
            if( wBitboardOcc & OpenFileW[sq] || (bBitboardK &QuadrantBKbtm[sq]) == 0 )
                continue;

            if( WhiteLeader <= rank )
                WhiteLeader = rank;
            }

        else if( WhiteLeader <= rank )
            WhiteLeader = rank;
        }
    BlackLeader = 0;
    C = PawnInfo->bPassedFiles;

    while( C )
        {
        sq = LSB(FileArray[LSB(C)] & bBitboardP);
        rank = R8 - RANK(sq);
        BitClear(0, C);

        if( (ShepherdBK[sq]&bBitboardK) == 0 )
            {
            if( bBitboardOcc & OpenFileB[sq] || (wBitboardK &QuadrantWKbtm[sq]) == 0 )
                continue;

            if( BlackLeader <= rank )
                BlackLeader = rank;
            }

        else if( BlackLeader <= rank )
            BlackLeader = rank;
        }
    Position->Current->Value = -(Token * (Value + matval)) / 128;
    Position->Current->bKcheck = Position->Current->wKcheck = 0;

    if( WhiteLeader > BlackLeader + 1 && (bBitboardP &InFrontB[R8 - WhiteLeader + 2]) == 0 )
        Position->Current->Value -= 150 + 50 * WhiteLeader;

    if( BlackLeader > WhiteLeader && (wBitboardP &InFrontW[BlackLeader - 1]) == 0 )
        Position->Current->Value += 150 + 50 * BlackLeader;

    A = (wBitboardP &( ~FileA)) << 7;
    T = A & bBitboardK;
    Position->Current->bKcheck |= (T >> 7);
    Position->Current->wAtt = A;
    A = (wBitboardP &( ~FileH)) << 9;
    T = A & bBitboardK;
    Position->Current->bKcheck |= (T >> 9);
    Position->Current->wAtt |= A | AttK[Position->wKsq];
    A = (bBitboardP &( ~FileH)) >> 7;
    T = A & wBitboardK;
    Position->Current->wKcheck |= (T << 7);
	Position->Current->bAtt = A;
    A = (bBitboardP &( ~FileA)) >> 9;
    T = A & wBitboardK;
    Position->Current->wKcheck |= (T << 9);
	Position->Current->bAtt |= A | AttK[Position->bKsq];

    if( bBitboardK & AttK[Position->wKsq] )
        {
        Position->Current->bKcheck |= SqSet[Position->wKsq];
        Position->Current->wKcheck |= SqSet[Position->bKsq];
        }

    if( Position->Current->Value < 0 && !wBitboardP )
        Position->Current->Value = 0;

    if( Position->Current->Value > 0 && !bBitboardP )
        Position->Current->Value = 0;

    if( Position->Current->Value < 0 )
        {
        if( (wBitboardP & ~FileH) == 0 && (AttK[Position->bKsq] | bBitboardK) & SqSet[H8] )
            Position->Current->Value = 0;

        if( (wBitboardP & ~FileA) == 0 && (AttK[Position->bKsq] | bBitboardK) & SqSet[A8] )
            Position->Current->Value = 0;

        if( (Position->Current->flags & 28) == 28 )
            {
            sq = LSB(wBitboardP);
            rank = RANK(sq);
            Value = kpk_black[384 * Position->wKsq + 6 * Position->bKsq + rank - 1]&(1 << FILE(sq));

            if( !Value )
                Position->Current->Value = 0;
            else
                Position->Current->Value = -((sint16)(Position->Current->PST & 0xffff)) - 75 * rank - 250;
            }
        }

    if( Position->Current->Value > 0 )
        {
        if( (bBitboardP & ~FileH) == 0 && (AttK[Position->wKsq] | wBitboardK) & SqSet[H1] )
            Position->Current->Value = 0;

        if( (bBitboardP & ~FileA) == 0 && (AttK[Position->wKsq] | wBitboardK) & SqSet[A1] )
            Position->Current->Value = 0;

        if( (Position->Current->flags & 28) == 28 )
            {
            sq = H8 - MSB(bBitboardP);
            rank = RANK(sq);
            Value = kpk_white[384 * (H8 - Position->bKsq) + 6 * (H8 - Position->wKsq) + rank - 1]&(1 << FILE(sq));

            if( !Value )
                Position->Current->Value = 0;
            else
                Position->Current->Value = -((sint16)(Position->Current->PST & 0xffff)) + 75 * rank + 250;
            }
        }
    }

void Eval( typePos *Position, int min, int max, int move )
    {
    typePawnEval *PawnPointer;
    int index, matval, Value;
    int b, rank, antiphase, phase;
    int to, cp, wKs, bKs;
	uint64 U, wKatt, bKatt, A, AttB, AttR;
	sint32 wKhit, bKhit;
    uint64 wGoodMinor, bGoodMinor, wSafeMob, bSafeMob, wOKxray, bOKxray;
    uint64 T, bPatt, wPatt;
    int open, end;
    uint8 bGoodAtt, wGoodAtt;
    uint8 Token;
    int v, positional;
    typePawnEval PawnInfo[1];
    int ch;
    uint32 r;
    int n, adj;
    int mask;

    PawnPointer = PawnHash + (Position->Current->PawnHash &(PawnHashSize - 1));
    PrefetchPawnHash;
    index = (Position->Current->material >> 8) & 0x7ffff;
    Token = Material[index].token;
    Position->Current->flags = Material[index].flags;
    Position->Current->exact = false;

    if( !(Position->Current->material & 0x80000000) )
        matval = Material[index].Value;
    else
        {
        if( POPCNT(wBitboardQ) > 1 || POPCNT(bBitboardQ) > 1 || POPCNT(wBitboardR) > 2 || POPCNT(bBitboardR) > 2
            || POPCNT(wBitboardBL) > 1 || POPCNT(bBitboardBL) > 1 || POPCNT(wBitboardBD) > 1 || POPCNT(bBitboardBD) > 1
            || POPCNT(wBitboardN) > 2 || POPCNT(bBitboardN) > 2 )
            {
            Token = 0x80;
            matval = MaterialValue(Position);
            Position->Current->flags = 0;

            if( wBitboardQ | wBitboardR | wBitboardB | wBitboardN )
                Position->Current->flags |= 2;

            if( bBitboardQ | bBitboardR | bBitboardB | bBitboardN )
                Position->Current->flags |= 1;
            }
        else
            {
            matval = Material[index].Value;
            Position->Current->material &= 0x7fffffff;
            }
        }

    if( ((Position->Current->Hash ^ EvalHash[Position->Current->Hash & EvalHashMask]) & 0xffffffffffff0000) == 0 )
        {
        Value = (int)((sint16)(EvalHash[Position->Current->Hash & EvalHashMask] & 0xffff));
        Position->Current->lazy = 0;
        Mobility(Position);
        Position->Current->PositionalValue = ((Position->wtm) ? Value : -Value) - matval;
        Position->Current->Value = Value;

        if( move && !(Position->Current - 1)->lazy )
            {
            Position->wtm ? AdjustPositionalGainW(Position, move) : AdjustPositionalGainB(Position, move);
            }

        if( Value > 15000 || Value < -15000 )
            Position->Current->exact = true;
        return;
        }

    memcpy(PawnInfo, PawnPointer, sizeof(typePawnEval));

    PawnInfo->PawnHash ^= (((uint64 *)(PawnInfo)) + 0x1)[0];
    PawnInfo->PawnHash ^= (((uint64 *)(PawnInfo)) + 0x2)[0];
    PawnInfo->PawnHash ^= (((uint64 *)(PawnInfo)) + 0x3)[0];

    if( (Position->Current->material & 0xff) == 0 )
        {
        Position->wtm
            ? KingPawnWhite(Position, matval, Token, PawnInfo) : KingPawnBlack(Position, matval, Token, PawnInfo);
        Position->Current->lazy = 1;
        Position->Current->PositionalValue = 0;
        EvalHash[Position->Current->Hash & EvalHashMask] =
            (Position->Current->Hash & 0xffffffffffff0000) | (Position->Current->Value & 0xffff);
        return;
        }

    if( (Position->Current->flags & WhiteMinorOnlyShift || Position->Current->flags & BlackMinorOnlyShift)
        && PawnInfo->PawnHash != Position->Current->PawnHash )
        PawnEval(Position, PawnInfo);

    if( (Position->Current->flags & WhiteMinorOnlyShift && PawnInfo->wPfile_count <= 1)
        || (Position->Current->flags & BlackMinorOnlyShift && PawnInfo->bPfile_count <= 1)
            || (Position->Current->flags & 128) )
        ;
    else
        {
        if( Position->wtm )
            {
            positional = (Position->Current - 1)->PositionalValue;
            cp = Position->Current->cp;
            to = To(move);
            Value = PieceSquareValue[Position->sq[to]][to] - PieceSquareValue[Position->sq[to]][From(move)];

            if( cp )
                Value -= PieceSquareValue[cp][to];
			phase = MIN (Position->Current->material & 0xff, 32);
            end = (sint16)(Value & 0xffff);
            open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
            antiphase = 32 - phase;
            Value = (end * antiphase + open * phase) / 32;
            positional += Value;
            v = positional + matval;

            if( v < -max - 16 * (int) (Position->Current - 1)->lazy
            || v > -min + 16 * (int) (Position->Current - 1)->lazy )
                {
                Position->Current->lazy = (Position->Current - 1)->lazy + 1;
                Position->Current->Value = v;
                Position->Current->PositionalValue = positional;
                Mobility(Position);
                return;
                }
            }
        else
            {
            positional = (Position->Current - 1)->PositionalValue;
            cp = Position->Current->cp;
            to = To(move);
            Value = PieceSquareValue[Position->sq[to]][to] - PieceSquareValue[Position->sq[to]][From(move)];

            if( cp )
                Value -= PieceSquareValue[cp][to];
			phase = MIN (Position->Current->material & 0xff, 32);
            end = (sint16)(Value & 0xffff);
            open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
            antiphase = 32 - phase;
            Value = (end * antiphase + open * phase) / 32;
            positional += Value;
            v = positional + matval;

            if( v < min - 16 * (int) (Position->Current - 1)->lazy
            || v > max + 16 * (int) (Position->Current - 1)->lazy )
                {
                Position->Current->lazy = (Position->Current - 1)->lazy + 1;
                Position->Current->Value = -v;
                Position->Current->PositionalValue = positional;
                Mobility(Position);
                return;
                }
            }
        }

    wKs = Position->wKsq;
    bKs = Position->bKsq;
    wKatt = AttK[wKs];
	bKatt = AttK[bKs];
    bGoodAtt = wGoodAtt = 0;

    if( PawnInfo->PawnHash != Position->Current->PawnHash )
        PawnEval(Position, PawnInfo);
    Value = (Position->Current->PST)+(PawnInfo->score);

    Position->Current->wXray = 0;

    A = (wBitboardP &( ~FileA)) << 7;
    T = A & bBitboardK;
    Position->Current->bKcheck = (T >> 7);
    wPatt = A;
    A = (wBitboardP &( ~FileH)) << 9;
    T = A & bBitboardK;
    Position->Current->bKcheck |= (T >> 9);
    wPatt |= A;
    Position->Current->wAtt = wPatt;
    A = (bBitboardP &( ~FileH)) >> 7;
    T = A & wBitboardK;
    Position->Current->wKcheck = (T << 7);
    bPatt = A;
    A = (bBitboardP &( ~FileA)) >> 9;
    T = A & wBitboardK;
    Position->Current->wKcheck |= (T << 9);
    bPatt |= A;
	Position->Current->bAtt = bPatt;

    bOKxray = ( ~bBitboardP) &~wPatt;
    wOKxray = ( ~wBitboardP) &~bPatt;
    wGoodMinor = (wBitboardN | wBitboardB) & wPatt;
    bGoodMinor = (bBitboardN | bBitboardB) & bPatt;

	if (wPatt & bKatt)
		bKhit = HitP;
	else
		bKhit = 0;
    U = (Position->OccupiedBW >> 8) & wBitboardP;

    while( U )
        {
        b = LSB(U);
        Value -= PawnAntiMobility;
        BitClear(b, U);
        }
    wSafeMob = ~(bPatt | wBitboardOcc);

    U = wBitboardQ;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        AttB = AttB(b);
        AttR = AttR(b);

        if( bBitboardK & Diag[b] )
            {
            T = AttB(bKs) & AttB;

            if( T )
                {
                Value += wQxrayD[Position->sq[LSB(T)]];
                Position->Current->wXray |= T;
                Position->XrayW[LSB(T)] = b;
                }
            }
        else if( bBitboardK & Ortho[b] )
            {
            T = AttR(bKs) & AttR;

            if( T )
                {
                Value += wQxrayO[Position->sq[LSB(T)]];
                Position->Current->wXray |= T;
                Position->XrayW[LSB(T)] = b;
                }
            }
        A = AttB | AttR;
        T = A & wSafeMob;
        Position->Current->wAtt |= A;

		if (A & bKatt)
			bKhit += HitQ;

        if( A & bBitboardK )
            Position->Current->bKcheck |= SqSet[b];

        if( A & wKatt )
            {
            Value += QguardK;
            }
        Value += MobQ(T);

        if( A &( ~bPatt) & bBitboardOcc )
            {
            Value += Qatt;
            }

        if( bBitboardP & AttPb[b] )
            {
            Value -= PattQ;
            bGoodAtt += 1;
            }

        if( RANK(b) == R7 )
            {
            if( (bBitboardP | bBitboardK) & Ranks78 )
                {
                Value += Queen7th;

                if( wBitboardR & RANK7 & AttR && bBitboardK & RANK8 )
                    {
                    Value += DoubQueen7th;
                    }
                }
            }
        }

    U = wBitboardR;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        A = AttR(b);
        Position->Current->wAtt |= A;

        if( bBitboardK & Ortho[b] )
            {
            T = AttR(bKs) & A;

            if( T )
                {
                Value += wRxray[Position->sq[LSB(T)]];
                Position->Current->wXray |= T;
                Position->XrayW[LSB(T)] = b;
                }
            }
		if (A & bKatt)
			bKhit += HitR;

        if( A & bBitboardK )
            Position->Current->bKcheck |= SqSet[b];

        if( A & wKatt )
            {
            Value += RguardK;
            }
        Value += MobR(A & wOKxray);

        if( A &( ~bPatt) & bBitboardP )
            {
            Value += RattP;
            }

        if( A &((bBitboardN | bBitboardB) & ~bPatt) )
            {
            Value += RattBN;
            }

        if( A & bBitboardQ )
            {
            Value += RattQ;
            wGoodAtt += 1;
            }

        if( bBitboardP & AttPb[b] )
            {
            Value -= PattR;
            bGoodAtt += 1;
            }

        if( (wBitboardP &OpenFileW[b]) == 0 )
            {
            Value += RookHalfOpen;

            if( (bBitboardP &OpenFileW[b]) == 0 )
                {
                T = bGoodMinor & OpenFileW[b];

                if( !T )
                    {
                    Value += RookOpenFile;
                    }
                else
                    {
                    int t = LSB(T);

                    if( (IsolatedFiles[FILE(t)]&
                    InFrontB[RANK(t)]&wBitboardP) == 0 )
                        {
                        Value += RookOpenFixedMinor;
                        }
                    else
                        {
                        Value += RookOpenMinor;
                        }
                    }
                }
            else
                {
                T = OpenFileW[b] & bBitboardP;

                if( T )
                    {
                    int t = LSB(T);

                    if( (IsolatedFiles[FILE(t)]&
                    InFrontW[RANK(t)]&bBitboardP) == 0 )
                        {
                        Value += RookHalfOpenPawn;
                        }
                    }
                }

            if( bBitboardK & OpenFileW[b] )
                {
                Value += RookHalfOpenKing;
                }
            }

        if( SqSet[b] & wOutpost && (IsolatedFiles[FILE(b)]&InFrontW[RANK(b)]&bBitboardP) == 0 )
            {
            if( wBitboardP & AttPw[b] )
                {
                Value += OutpostRook;

                if( A &(bKatt | (bBitboardOcc & ~bPatt)) & RankArray[RANK(b)] )
                    Value += OutpostRookGuarded;
                }
            }

        if( RANK(b) == R8 )
            {
            if( bBitboardK & RANK8 )
                {
                Value += RookKing8th;
                }
            }

        if( RANK(b) == R7 )
            {
            if( (bBitboardP | bBitboardK) & Ranks78 )
                {
                Value += Rook7thKingPawn;

                if( bBitboardK & RANK8 && (wBitboardQ | wBitboardR) & RANK7 & A )
                    {
                    Value += DoubRook7thKingPawn;
                    }
                }
            }

        if( RANK(b) == R6 && (bBitboardP | bBitboardK) & Ranks678 )
            {
            Value += Rook6thKingPawn;
            }
        }

    wSafeMob |= bBitboardOcc ^ bBitboardP;

    U = wBitboardB;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        A = AttB(b);
        Position->Current->wAtt |= A;

        if( bBitboardK & Diag[b] )
            {
            T = AttB(bKs) & A;

            if( T )
                {
                Value += wBxray[Position->sq[LSB(T)]];
                Position->Current->wXray |= T;
                Position->XrayW[LSB(T)] = b;
                }
            }
		if (A & bKatt)
			bKhit += HitB;

        if( A & bBitboardK )
            Position->Current->bKcheck |= SqSet[b];

        if( A & wKatt )
            {
            Value += BguardK;
            }

        Value += MobB(A & wSafeMob, InFrontW[RANK(b)]);

        if( A &( ~bPatt) & bBitboardP )
            {
			Value += bAttP;
            }

        if( A &( ~bPatt) & bBitboardN )
            {
			Value += bAttN;
            }

        if( A &(bBitboardR | bBitboardQ) )
            {
			Value += bAttRQ;
            wGoodAtt += 1;
            }

        if( bBitboardP & AttPb[b] )
            {
            Value -= PattB;
            bGoodAtt += 1;
            }

        if( SqSet[b] & White )
            {
            Value -= (PawnInfo->wPlight + PawnInfo->bPlight / 2) * Score(1, 1);
            Value += POPCNT(bBitboardP & White & InFrontB[RANK(b)] & ~bPatt) * Score(0, 2);
            }
        else
            {
            Value -= (PawnInfo->wPdark + PawnInfo->bPdark / 2) * Score(1, 1);
            Value += POPCNT(bBitboardP & Black & InFrontB[RANK(b)] & ~bPatt) * Score(0, 2);
            }

        if( SqSet[b] & wOutpost && (IsolatedFiles[FILE(b)]&InFrontW[RANK(b)]&bBitboardP) == 0 )
            {
            if( wBitboardP & AttPw[b] )
                {
                Value += OutpostBishop;

				if (A & (bKatt | (bBitboardOcc & ~bPatt)))
					Value += OutpostBishopGuarded;
                }
            }

        if( Position->sq[BishopTrapSq[b]] == bEnumP )
            {
            Value -= BishopTrapValue;

            if( Position->sq[GoodBishopTrapSq[b]] == bEnumP )
                {
                Value -= BishopTrapGuardValue;
                }
            }
        }

    U = wBitboardN;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        A = AttN[b];
        Position->Current->wAtt |= A;

		if (A & (bKatt | bBitboardK))
			bKhit += HitN;

        if( A & bBitboardK )
            Position->Current->bKcheck |= SqSet[b];

        if( A &(wKatt | wBitboardK) )
            {
            Value += NguardK;
            }
        Value += MobN(A & wSafeMob, InFrontW[RANK(b)]);

        if( A &( ~bPatt) & bBitboardP )
            {
            Value += NattP;
            }

        if( A &( ~bPatt) & bBitboardB )
            {
            Value += NattB;
            }

        if( A &(bBitboardR | bBitboardQ) )
            {
            Value += NattRQ;
            wGoodAtt += 1;
            }

        if( bBitboardP & AttPb[b] )
            {
            Value -= PattN;
            bGoodAtt += 1;
            }

        if( SqSet[b] & wOutpost && (IsolatedFiles[FILE(b)]&InFrontW[RANK(b)]&bBitboardP) == 0 )
            {
            Value += OutpostKnight;

            if( wBitboardP & AttPw[b] )
                {
                Value += OutpostKnightPawn;

				if (A & (bKatt | (bBitboardOcc & ~bPatt)))
                    {
					Value += OutpostKnightAttacks;

                    if( RANK(b) == R5 )
                        Value += OutpostKnight5th;

                    if( FILE(b) == FD || FILE(b) == FE )
                        Value += OutpostKnightONde;
                    }
                }
            }
        }

    if( bPatt & wKatt )
        wKhit = HitP;
    else
        wKhit = 0;
    U = (Position->OccupiedBW << 8) & bBitboardP;
    Position->Current->bXray = 0;

    while( U )
        {
        b = LSB(U);
        Value += PawnAntiMobility;
        BitClear(b, U);
        }
    bSafeMob = ~(wPatt | bBitboardOcc);

    U = bBitboardQ;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        AttB = AttB(b);
        AttR = AttR(b);

        if( wBitboardK & Diag[b] )
            {
            T = AttB(wKs) & AttB;

            if( T )
                {
                Value -= bQxrayD[Position->sq[LSB(T)]];
                Position->Current->bXray |= T;
                Position->XrayB[LSB(T)] = b;
                }
            }
        else if( wBitboardK & Ortho[b] )
            {
            T = AttR(wKs) & AttR;

            if( T )
                {
                Value -= bQxrayO[Position->sq[LSB(T)]];
                Position->Current->bXray |= T;
                Position->XrayB[LSB(T)] = b;
                }
            }
        A = AttB | AttR;
        T = A & bSafeMob;
		Position->Current->bAtt |= A;

        if( A & wKatt )
            wKhit += HitQ;

        if( A & wBitboardK )
            Position->Current->wKcheck |= SqSet[b];

		if (A & bKatt)
            {
            Value -= QguardK;
            }
        Value -= MobQ(T);

        if( A &( ~wPatt) & wBitboardOcc )
            {
            Value -= Qatt;
            }

        if( wBitboardP & AttPw[b] )
            {
            Value += PattQ;
            wGoodAtt += 1;
            }

        if( RANK(b) == R2 )
            {
            if( (wBitboardP | wBitboardK) & Ranks12 )
                {
                Value -= Queen7th;

                if( bBitboardR & RANK2 & AttR && wBitboardK & RANK1 )
                    {
                    Value -= DoubQueen7th;
                    }
                }
            }
        }

    U = bBitboardR;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        A = AttR(b);
		Position->Current->bAtt |= A;

        if( wBitboardK & Ortho[b] )
            {
            T = A & AttR(wKs);

            if( T )
                {
                Value -= bRxray[Position->sq[LSB(T)]];
                Position->Current->bXray |= T;
                Position->XrayB[LSB(T)] = b;
                }
            }

        if( A & wKatt )
            wKhit += HitR;

        if( A & wBitboardK )
            Position->Current->wKcheck |= SqSet[b];

		if (A & bKatt)
            {
            Value -= RguardK;
            }
        Value -= MobR(A & bOKxray);

        if( A &( ~wPatt) & wBitboardP )
            {
            Value -= RattP;
            }

        if( A &(wBitboardN | wBitboardB) & ~wPatt )
            {
            Value -= RattBN;
            }

        if( A & wBitboardQ )
            {
            Value -= RattQ;
            bGoodAtt += 1;
            }

        if( wBitboardP & AttPw[b] )
            {
            Value += PattR;
            wGoodAtt += 1;
            }

        if( (bBitboardP &OpenFileB[b]) == 0 )
            {
            Value -= RookHalfOpen;

            if( (wBitboardP &OpenFileB[b]) == 0 )
                {
                T = wGoodMinor & OpenFileB[b];

                if( !T )
                    {
                    Value -= RookOpenFile;
                    }
                else
                    {
                    int t = MSB(T);

                    if( (IsolatedFiles[FILE(t)]&
                    InFrontW[RANK(t)]&bBitboardP) == 0 )
                        {
                        Value -= RookOpenFixedMinor;
                        }
                    else
                        {
                        Value -= RookOpenMinor;
                        }
                    }
                }
            else
                {
                T = OpenFileB[b] & wBitboardP;

                if( T )
                    {
                    int t = MSB(T);

                    if( (IsolatedFiles[FILE(t)]&
                    InFrontB[RANK(t)]&wBitboardP) == 0 )
                        {
                        Value -= RookHalfOpenPawn;
                        }
                    }
                }

            if( wBitboardK & OpenFileB[b] )
                {
                Value -= RookHalfOpenKing;
                }
            }

        if( SqSet[b] & bOutpost && (IsolatedFiles[FILE(b)]&InFrontB[RANK(b)]&wBitboardP) == 0 )
            {
            if( bBitboardP & AttPb[b] )
                {
                Value -= OutpostRook;

                if( A &(wKatt | (wBitboardOcc & ~wPatt)) & RankArray[RANK(b)] )
                    Value -= OutpostRookGuarded;
                }
            }

        if( RANK(b) == R1 )
            {
            if( wBitboardK & RANK1 )
                {
                Value -= RookKing8th;
                }
            }

        if( RANK(b) == R2 )
            {
            if( (wBitboardP | wBitboardK) & Ranks12 )
                {
                Value -= Rook7thKingPawn;

                if( wBitboardK & RANK1 && (bBitboardQ | bBitboardR) & RANK2 & A )
                    {
                    Value -= DoubRook7thKingPawn;
                    }
                }
            }

        if( RANK(b) == R3 && (wBitboardP | wBitboardK) & Ranks123 )
            {
            Value -= Rook6thKingPawn;
            }
        }

    bSafeMob |= wBitboardOcc ^ wBitboardP;

    U = bBitboardB;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        A = AttB(b);
		Position->Current->bAtt |= A;

        if( wBitboardK & Diag[b] )
            {
            T = A & AttB(wKs);

            if( T )
                {
                Value -= bBxray[Position->sq[LSB(T)]];
                Position->Current->bXray |= T;
                Position->XrayB[LSB(T)] = b;
                }
            }

        if( A & wKatt )
            wKhit += HitB;

        if( A & wBitboardK )
            Position->Current->wKcheck |= SqSet[b];

		if (A & bKatt)
            {
            Value -= BguardK;
            }
		Value -= MobB(A & bSafeMob, InFrontB[RANK(b)]);

        if( A &( ~wPatt) & wBitboardP )
            {
			Value -= bAttP;
            }

        if( A &( ~wPatt) & wBitboardN )
            {
			Value -= bAttN;
            }

        if( A &(wBitboardR | wBitboardQ) )
            {
			Value -= bAttRQ;
            bGoodAtt += 1;
            }

        if( wBitboardP & AttPw[b] )
            {
            Value += PattB;
            wGoodAtt += 1;
            }

        if( SqSet[b] & White )
            {
            Value += (PawnInfo->bPlight + PawnInfo->wPlight / 2) * Score(1, 1);
            Value -= POPCNT(wBitboardP & White & InFrontW[RANK(b)] & ~wPatt) * Score(0, 2);
            }
        else
            {
            Value += (PawnInfo->bPdark + PawnInfo->wPdark / 2) * Score(1, 1);
            Value -= POPCNT(wBitboardP & Black & InFrontW[RANK(b)] & ~wPatt) * Score(0, 2);
            }

        if( SqSet[b] & bOutpost && (IsolatedFiles[FILE(b)]&InFrontB[RANK(b)]&wBitboardP) == 0 )
            {
            if( bBitboardP & AttPb[b] )
                {
                Value -= OutpostBishop;

                if( A &(wKatt | (wBitboardOcc & ~wPatt)) )
                    Value -= OutpostBishopGuarded;
                }
            }

        if( Position->sq[BishopTrapSq[b]] == wEnumP )
            {
            Value += BishopTrapValue;

            if( Position->sq[GoodBishopTrapSq[b]] == wEnumP )
                {
                Value += BishopTrapGuardValue;
                }
            }
        }

    U = bBitboardN;

    while( U )
        {
        b = LSB(U);
        BitClear(b, U);
        A = AttN[b];
		Position->Current->bAtt |= A;

        if( A &(wKatt | wBitboardK) )
            wKhit += HitN;

        if( A & wBitboardK )
            Position->Current->wKcheck |= SqSet[b];

		if (A & (bKatt | bBitboardK))
            {
            Value -= NguardK;
            }
        Value -= MobN(A & bSafeMob, InFrontB[RANK(b)]);

        if( A &( ~wPatt) & wBitboardP )
            {
            Value -= NattP;
            }

        if( A &( ~wPatt) & wBitboardB )
            {
            Value -= NattB;
            }

        if( A &(wBitboardR | wBitboardQ) )
            {
            Value -= NattRQ;
            bGoodAtt += 1;
            }

        if( wBitboardP & AttPw[b] )
            {
            Value += PattN;
            wGoodAtt += 1;
            }

        if( SqSet[b] & bOutpost && (IsolatedFiles[FILE(b)]&InFrontB[RANK(b)]&wBitboardP) == 0 )
            {
            Value -= OutpostKnight;

            if( bBitboardP & AttPb[b] )
                {
                Value -= OutpostKnightPawn;

                if( A &(wKatt | (wBitboardOcc & ~wPatt)) )
                    {
					Value -= OutpostKnightAttacks;

                    if( RANK(b) == R4 )
                        Value -= OutpostKnight5th;

                    if( FILE(b) == FD || FILE(b) == FE )
                        Value -= OutpostKnightONde;
                    }
                }
            }
        }

    Position->Current->wAtt |= wKatt;
	Position->Current->bAtt |= bKatt;

	if (bKatt & wBitboardK)
        {
        Position->Current->wKcheck |= SqSet[Position->bKsq];
        Position->Current->bKcheck |= SqSet[Position->wKsq];
        }

	if ((~Position->Current->bAtt) & wKatt & bBitboardP)
        {
        Value += KingAttUnguardedPawn;
        }

    T = RookTrapped[wKs] & wBitboardR;

    if( T )
        {
        int t = LSB(T);
        T = OpenFileW[t] & wBitboardP;

        if( T )
            {
            t = LSB(T);
            t >>= 3;
            Value -= Score(10 * (6 - t), 0);
            }
        }

	if (wKatt & bKatt)
        wKhit += HitK;
    ch = (((KingSafetyMult[wKhit >> 16] * (wKhit & 0xffff)) / KingSafetyDivider) << 16) + PawnInfo->wKdanger;

    if( !bBitboardQ )
        {
        ch >>= 16;
        ch *= POPCNT(bBitboardR | bBitboardN | bBitboardB);
        ch >>= 3;
        ch <<= 16;
        }
    Value -= ch;

	if ((~Position->Current->wAtt) & bKatt & wBitboardP)
        {
        Value -= KingAttUnguardedPawn;
        }

    T = RookTrapped[bKs] & bBitboardR;

    if( T )
        {
        int t = MSB(T);
        T = OpenFileB[t] & bBitboardP;

        if( T )
            {
            t = MSB(T);
            t >>= 3;
            Value += Score(10 * (t - 1), 0);
            }
        }

	if (wKatt & bKatt)
		bKhit += HitK;
	ch = (((KingSafetyMult[bKhit >> 16] *
		(bKhit & 0xffff)) / KingSafetyDivider) << 16) + PawnInfo->bKdanger;

    if( !wBitboardQ )
        {
        ch >>= 16;
        ch *= POPCNT(wBitboardR | wBitboardN | wBitboardB);
        ch >>= 3;
        ch <<= 16;
        }
    Value += ch;

    if( wGoodAtt >= 2 )
        {
        Value += MultipleAtt;
        }

    if( bGoodAtt >= 2 )
        {
        Value -= MultipleAtt;
        }

    if( (wBitboardR | wBitboardQ) & CrampFile[FILE(bKs)] )
        {
        Value += Score(0, 5);

        if( (CrampFile[FILE(bKs)]&(wBitboardP | bBitboardP)) == 0 )
            Value += Score(5, 15);
        }

    if( (bBitboardR | bBitboardQ) & CrampFile[FILE(wKs)] )
        {
        Value -= Score(0, 5);

        if( (CrampFile[FILE(wKs)]&(bBitboardP | wBitboardP)) == 0 )
            Value -= Score(5, 15);
        }

    U = PawnInfo->wPassedFiles;

    while( U )
        {
        b = MSB(FileArray[LSB(U)] & wBitboardP);
        BitClear(0, U);
        rank = RANK(b);

        if( rank <= R3 )
            continue;

        if( RookEnd )
            {
            if( wBitboardR & OpenFileW[b] )
                {
                if( rank == R7 )
                    {
                    Value -= Rook7thEnd;
                    }
                else if( rank == R6 )
                    {
                    Value -= Rook6thEnd;
                    }
                }

            if( OpenFileW[b] & wBitboardK && CrampFile[FILE(wKs)] & bBitboardR )
                Value -= Score(0, 1 << (rank - R2));
            }

        if( Position->sq[b + 8] == 0 )
            {
            Value += PassedPawnCanMove[rank];
            }

        if( (OpenFileW[b]&wBitboardOcc) == 0 )
            {
            Value += PassedPawnMeClear[rank];
            }

        if( (OpenFileW[b]&bBitboardOcc) == 0 )
            {
            Value += PassedPawnOppClear[rank];
            }

		if ((OpenFileW[b] & (~Position->Current->wAtt) & Position->Current->bAtt) == 0)
            {
            Value += PassedPawnIsFree[rank];
            }

        if( QueenEnd )
            {
            if( rank == R7 && wBitboardQ & OpenFileW[b] )
                {
                Value -= Queen7thEnd;
                }
            Value += RankQueenEnd[rank];
            }
        }

    U = PawnInfo->bPassedFiles;

    while( U )
            {
            b = LSB(FileArray[LSB(U)] & bBitboardP);
            BitClear(0, U);
            rank = RANK(b);

            if( rank >= R6 )
                continue;

            if( RookEnd )
                {
                if( bBitboardR & OpenFileB[b] )
                    {
                    if( rank == R2 )
                        {
                        Value += Rook7thEnd;
                        }
                    else if( rank == R3 )
                        {
                        Value += Rook6thEnd;
                        }
                    }

                if( OpenFileB[b] & bBitboardK && CrampFile[FILE(bKs)] & wBitboardR )
                    Value += Score(0, 1 << (R7 - rank));
                }

            if( Position->sq[b - 8] == 0 )
                {
                Value -= PassedPawnCanMove[7 - rank];
                }

            if( (OpenFileB[b]&bBitboardOcc) == 0 )
                {
                Value -= PassedPawnMeClear[7 - rank];
                }

            if( (OpenFileB[b]&wBitboardOcc) == 0 )
                {
                Value -= PassedPawnOppClear[7 - rank];
                }

			if ((OpenFileB[b] & Position->Current->
				wAtt & ~Position->Current->bAtt) == 0)
                {
                Value -= PassedPawnIsFree[7 - rank];
                }

            if( QueenEnd )
                {
                if( rank == R2 && bBitboardQ & OpenFileB[b] )
                    {
                    Value += Queen7thEnd;
                    }
                Value -= RankQueenEnd[7 - rank];
                }
            }

	phase = MIN (Position->Current->material & 0xff, 32);
    end = (sint16)(Value & 0xffff);
    open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
    antiphase = 32 - phase;
    Value = end * antiphase + open * phase;
    Value = Value / 32 + matval;
    Value = (Value * Token) / 128;

    if( Value > 0 )
        Value -= (PawnInfo->wDrawWeight * MIN(Value, 100)) / 64;
    else
        Value += (PawnInfo->bDrawWeight * MIN(-Value, 100)) / 64;

    if( PosBishopKnightMate )
        {
        if( Value > 0 )
            {
            if( wBitboardBL )
                Value -=
                    20 * MIN(MaxDist(A8, Position->bKsq), MaxDist(H1, Position->bKsq))
                        + 10 * MIN(MinDist(A8, Position->bKsq), MinDist(H1, Position->bKsq));
            else
                Value -=
                    20 * MIN(MaxDist(A1, Position->bKsq), MaxDist(H8, Position->bKsq))
                        + 10 * MIN(MinDist(A1, Position->bKsq), MinDist(H8, Position->bKsq));
            }
        else
            {
            if( bBitboardBL )
                Value +=
                    20 * MIN(MaxDist(A8, Position->wKsq), MaxDist(H1, Position->wKsq))
                        + 10 * MIN(MinDist(A8, Position->wKsq), MinDist(H1, Position->wKsq));
            else
                Value +=
                    20 * MIN(MaxDist(A1, Position->wKsq), MaxDist(H8, Position->wKsq))
                        + 10 * MIN(MinDist(A1, Position->wKsq), MinDist(H8, Position->wKsq));
            }
        }

    if( Position->Current->reversible > 50 )
        {
        Value *= (114 - Position->Current->reversible);
        Value /= 64;
        }

    if( Value > 0 )
        {
        if( !Position->wtm && !BlackHasPiece && (bBitboardK ^ bBitboardP) == bBitboardOcc
            && !((bBitboardP >> 8) & ~Position->OccupiedBW) && !(AttK[Position->bKsq]& ~Position->Current->wAtt)
            && !Position->Current->bKcheck )
            Value = 0;

        if( PosWhiteMinorOnly )
            {
            if( wBitboardN )
                {
                if( wBitboardP == SqSet[A7] && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8] )
                    Value = 0;

                if( wBitboardP == SqSet[H7] && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8] )
                    Value = 0;
                }
            else if( wBitboardBL && !(wBitboardP &NOTh) && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8] )
                {
                if( wBitboardP & SqSet[H5] && bBitboardP == (SqSet[G7] | SqSet[H6]) )
                    ;
                else
                    Value = 0;
                }
            else if( wBitboardBD && !(wBitboardP &NOTa) && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8] )
                {
                if( wBitboardP & SqSet[A5] && bBitboardP == (SqSet[B7] | SqSet[A6]) )
                    ;
                else
                    Value = 0;
                }

            if( !wBitboardP )
                Value = 0;
            }
        }
    else
        {
        if( Position->wtm && !WhiteHasPiece && (wBitboardK ^ wBitboardP) == wBitboardOcc
            && !((wBitboardP << 8) & ~Position->OccupiedBW) && !(AttK[Position->wKsq]& ~Position->Current->bAtt)
            && !Position->Current->wKcheck )
            Value = 0;

        if( PosBlackMinorOnly )
            {
            if( bBitboardN )
                {
                if( bBitboardP == SqSet[A2] && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1] )
                    Value = 0;

                if( bBitboardP == SqSet[H2] && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1] )
                    Value = 0;
                }
            else if( bBitboardBD && !(bBitboardP &NOTh) && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1] )
                {
                if( bBitboardP & SqSet[H4] && wBitboardP == (SqSet[G2] | SqSet[H3]) )
                    ;
                else
                    Value = 0;
                }
            else if( bBitboardBL && !(bBitboardP &NOTa) && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1] )
                {
                if( bBitboardP & SqSet[A4] && wBitboardP == (SqSet[B2] | SqSet[A3]) )
                    ;
                else
                    Value = 0;
                }

            if( !bBitboardP )
                Value = 0;
            }
        }

    Position->Current->Value = Position->wtm ? Value : -Value;
    Position->Current->PositionalValue = Value - matval;
    Position->Current->lazy = 0;
    EvalHash[Position->Current->Hash & EvalHashMask] =
        (Position->Current->Hash & 0xffffffffffff0000) | (Position->Current->Value & 0xffff);

    if( move && !(Position->Current - 1)->lazy )
        Position->wtm ? AdjustPositionalGainW(Position, move) : AdjustPositionalGainB(Position, move);
    }
