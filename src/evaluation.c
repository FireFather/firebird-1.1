#define Ranks78 0xffff000000000000
#define Ranks678 0xffffff0000000000
#define Ranks12 0x000000000000ffff
#define Ranks123 0x00000000000ffffff

#include "firebird.h"
#include "evaluation.h"
#include "tables.h"

typedef struct
    {
    uint64 RandKey;
    uint8 pad[56];
    } RAND;
static RAND Rand[MAX_CPUS]; /* init ? */
static uint32 Random32( int cpu )
    {
    Rand[cpu].RandKey = Rand[cpu].RandKey * 0x7913cc52088a6cfULL + 0x99f2e6bb0313ca0dULL;
    return ((Rand[cpu].RandKey >> 18) & 0xffffffff);
    }
void InitRandom32( uint64 x )
    {
    int cpu;

    for ( cpu = 0; cpu < MAX_CPUS; cpu++ )
        {
        x = x * 0xb18ec564ff729005ULL + 0x86ee25701b5e244fULL;
        Rand[cpu].RandKey = x;
        }
    }
static void AdjustPositionalGainB( typePOS *POSITION, int move )
    {
    int v, p, m;

    if( POSITION->DYN->cp )
        return;

    p = POSITION->sq[TO(move)];
    m = move & 07777;
    v = POSITION->DYN->PositionalValue - ((POSITION->DYN - 1)->PositionalValue);

    if( MAX_POS_GAIN(p, m) <= v )
        MAX_POS_GAIN(p, m) = v;
    else
        MAX_POS_GAIN(p, m)--;
    }
static void AdjustPositionalGainW( typePOS *POSITION, int move )
    {
    int v, p, m;

    if( POSITION->DYN->cp )
        return;

    p = POSITION->sq[TO(move)];
    m = move & 07777;
    v = ((POSITION->DYN - 1)->PositionalValue) - POSITION->DYN->PositionalValue;

    if( MAX_POS_GAIN(p, m) <= v )
        MAX_POS_GAIN(p, m) = v;
    else
        MAX_POS_GAIN(p, m)--;
    }

#define HashSize ( 0x8000 )
#define HASH_MASK ( HashSize - 1 )

uint64 EvalHash[HashSize]; /* non atomico con 32-bits ? */
void EvalHashClear()
    {
    int c;

    for ( c = 0; c < HashSize; c++ )
        EvalHash[c] = 0;
    }

#define PREFETCH_PAWN_HASH _mm_prefetch((char*)PAWN_PTR, 2);

static int MaterialValue( typePOS *POSITION )
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
static void KingPawnWhite( typePOS *POSITION, int matval, uint8 TOKEN, typePawnEval *PAWN_INFO )
    {
    int Value, WhiteLeader, BlackLeader, sq, trans;
    uint8 C;
    uint64 A, T;

    if( PAWN_INFO->PAWN_HASH != POSITION->DYN->PAWN_HASH )
        PawnEval(POSITION, PAWN_INFO);
    POSITION->DYN->wXray = POSITION->DYN->bXray = 0;
    Value = ((POSITION->DYN->STATIC)+(PAWN_INFO->SCORE));
    Value = (sint16)(Value & 0xffff);
    WhiteLeader = 0;
    C = PAWN_INFO->wPassedFiles;

    while( C )
        {
        sq = MSB(FileArray[LSB(C)] & wBitboardP);
        trans = RANK(sq);
        BitClear(0, C);

        if( (ShepherdWK[sq]&wBitboardK) == 0 )
            {
            if( wBitboardOcc & OpenFileW[sq] || (bBitboardK &QuadrantBKwtm[sq]) == 0 )
                continue;

            if( WhiteLeader <= trans )
                WhiteLeader = trans;
            }

        else if( WhiteLeader <= trans )
            WhiteLeader = trans;
        }
    BlackLeader = 0;
    C = PAWN_INFO->bPassedFiles;

    while( C )
        {
        sq = LSB(FileArray[LSB(C)] & bBitboardP);
        trans = R8 - RANK(sq);
        BitClear(0, C);

        if( (ShepherdBK[sq]&bBitboardK) == 0 )
            {
            if( bBitboardOcc & OpenFileB[sq] || (wBitboardK &QuadrantWKwtm[sq]) == 0 )
                continue;

            if( BlackLeader <= trans )
                BlackLeader = trans;
            }

        else if( BlackLeader <= trans )
            BlackLeader = trans;
        }
    POSITION->DYN->Value = (TOKEN * (Value + matval)) / 128;
    POSITION->DYN->bKcheck = POSITION->DYN->wKcheck = 0;

    if( WhiteLeader > BlackLeader && (bBitboardP &InFrontB[R8 - WhiteLeader + 1]) == 0 )
        POSITION->DYN->Value += 150 + 50 * WhiteLeader;

    if( BlackLeader > WhiteLeader + 1 && (wBitboardP &InFrontW[BlackLeader - 2]) == 0 )
        POSITION->DYN->Value -= 150 + 50 * BlackLeader;

    A = (wBitboardP &( ~FILEa)) << 7;
    T = A & bBitboardK;
    POSITION->DYN->bKcheck |= (T >> 7);
    POSITION->DYN->wAtt = A;
    A = (wBitboardP &( ~FILEh)) << 9;
    T = A & bBitboardK;
    POSITION->DYN->bKcheck |= (T >> 9);
    POSITION->DYN->wAtt |= A | AttK[POSITION->wKsq];
    A = (bBitboardP &( ~FILEh)) >> 7;
    T = A & wBitboardK;
    POSITION->DYN->wKcheck |= (T << 7);
	POSITION->DYN->bAtt = A;
    A = (bBitboardP &( ~FILEa)) >> 9;
    T = A & wBitboardK;
    POSITION->DYN->wKcheck |= (T << 9);
	POSITION->DYN->bAtt |= A | AttK[POSITION->bKsq];

    if( bBitboardK & AttK[POSITION->wKsq] )
        {
        POSITION->DYN->bKcheck |= SqSet[POSITION->wKsq];
        POSITION->DYN->wKcheck |= SqSet[POSITION->bKsq];
        }

    if( POSITION->DYN->Value > 0 && !wBitboardP )
        POSITION->DYN->Value = 0;

    if( POSITION->DYN->Value < 0 && !bBitboardP )
        POSITION->DYN->Value = 0;

    if( POSITION->DYN->Value > 0 )
        {
        if( (wBitboardP & ~FILEh) == 0 && (bBitboardK | AttK[POSITION->bKsq]) & SqSet[H8] )
            POSITION->DYN->Value = 0;

        if( (wBitboardP & ~FILEa) == 0 && (bBitboardK | AttK[POSITION->bKsq]) & SqSet[A8] )
            POSITION->DYN->Value = 0;

        if( (POSITION->DYN->flags & 28) == 28 )
            {
            sq = LSB(wBitboardP);
            trans = RANK(sq);
            Value = Yakov_white[384 * POSITION->wKsq + 6 * POSITION->bKsq + trans - 1]&(1 << FILE(sq));

            if( !Value )
                POSITION->DYN->Value = 0;
            else
                POSITION->DYN->Value = ((sint16)(POSITION->DYN->STATIC & 0xffff)) + 75 * trans + 250;
            }
        }

    if( POSITION->DYN->Value < 0 )
        {
        if( (bBitboardP & ~FILEh) == 0 && (wBitboardK | AttK[POSITION->wKsq]) & SqSet[H1] )
            POSITION->DYN->Value = 0;

        if( (bBitboardP & ~FILEa) == 0 && (wBitboardK | AttK[POSITION->wKsq]) & SqSet[A1] )
            POSITION->DYN->Value = 0;

        if( (POSITION->DYN->flags & 28) == 28 )
            {
            sq = H8 - MSB(bBitboardP);
            trans = RANK(sq);
            Value = Yakov_black[384 * (H8 - POSITION->bKsq) + 6 * (H8 - POSITION->wKsq) + trans - 1]&(1 << FILE(sq));

            if( !Value )
                POSITION->DYN->Value = 0;
            else
                POSITION->DYN->Value = ((sint16)(POSITION->DYN->STATIC & 0xffff)) - 75 * trans - 250;
            }
        }
    }
static void KingPawnBlack( typePOS *POSITION, int matval, uint8 TOKEN, typePawnEval *PAWN_INFO )
    {
    int Value, WhiteLeader, BlackLeader, sq, trans;
    uint8 C;
    uint64 A, T;

    if( PAWN_INFO->PAWN_HASH != POSITION->DYN->PAWN_HASH )
        PawnEval(POSITION, PAWN_INFO);
    POSITION->DYN->wXray = POSITION->DYN->bXray = 0;
    Value = ((POSITION->DYN->STATIC)+(PAWN_INFO->SCORE));
    Value = (sint16)(Value & 0xffff);
    WhiteLeader = 0;
    C = PAWN_INFO->wPassedFiles;

    while( C )
        {
        sq = MSB(FileArray[LSB(C)] & wBitboardP);
        trans = RANK(sq);
        BitClear(0, C);

        if( (ShepherdWK[sq]&wBitboardK) == 0 )
            {
            if( wBitboardOcc & OpenFileW[sq] || (bBitboardK &QuadrantBKbtm[sq]) == 0 )
                continue;

            if( WhiteLeader <= trans )
                WhiteLeader = trans;
            }

        else if( WhiteLeader <= trans )
            WhiteLeader = trans;
        }
    BlackLeader = 0;
    C = PAWN_INFO->bPassedFiles;

    while( C )
        {
        sq = LSB(FileArray[LSB(C)] & bBitboardP);
        trans = R8 - RANK(sq);
        BitClear(0, C);

        if( (ShepherdBK[sq]&bBitboardK) == 0 )
            {
            if( bBitboardOcc & OpenFileB[sq] || (wBitboardK &QuadrantWKbtm[sq]) == 0 )
                continue;

            if( BlackLeader <= trans )
                BlackLeader = trans;
            }

        else if( BlackLeader <= trans )
            BlackLeader = trans;
        }
    POSITION->DYN->Value = -(TOKEN * (Value + matval)) / 128;
    POSITION->DYN->bKcheck = POSITION->DYN->wKcheck = 0;

    if( WhiteLeader > BlackLeader + 1 && (bBitboardP &InFrontB[R8 - WhiteLeader + 2]) == 0 )
        POSITION->DYN->Value -= 150 + 50 * WhiteLeader;

    if( BlackLeader > WhiteLeader && (wBitboardP &InFrontW[BlackLeader - 1]) == 0 )
        POSITION->DYN->Value += 150 + 50 * BlackLeader;

    A = (wBitboardP &( ~FILEa)) << 7;
    T = A & bBitboardK;
    POSITION->DYN->bKcheck |= (T >> 7);
    POSITION->DYN->wAtt = A;
    A = (wBitboardP &( ~FILEh)) << 9;
    T = A & bBitboardK;
    POSITION->DYN->bKcheck |= (T >> 9);
    POSITION->DYN->wAtt |= A | AttK[POSITION->wKsq];
    A = (bBitboardP &( ~FILEh)) >> 7;
    T = A & wBitboardK;
    POSITION->DYN->wKcheck |= (T << 7);
	POSITION->DYN->bAtt = A;
    A = (bBitboardP &( ~FILEa)) >> 9;
    T = A & wBitboardK;
    POSITION->DYN->wKcheck |= (T << 9);
	POSITION->DYN->bAtt |= A | AttK[POSITION->bKsq];

    if( bBitboardK & AttK[POSITION->wKsq] )
        {
        POSITION->DYN->bKcheck |= SqSet[POSITION->wKsq];
        POSITION->DYN->wKcheck |= SqSet[POSITION->bKsq];
        }

    if( POSITION->DYN->Value < 0 && !wBitboardP )
        POSITION->DYN->Value = 0;

    if( POSITION->DYN->Value > 0 && !bBitboardP )
        POSITION->DYN->Value = 0;

    if( POSITION->DYN->Value < 0 )
        {
        if( (wBitboardP & ~FILEh) == 0 && (AttK[POSITION->bKsq] | bBitboardK) & SqSet[H8] )
            POSITION->DYN->Value = 0;

        if( (wBitboardP & ~FILEa) == 0 && (AttK[POSITION->bKsq] | bBitboardK) & SqSet[A8] )
            POSITION->DYN->Value = 0;

        if( (POSITION->DYN->flags & 28) == 28 )
            {
            sq = LSB(wBitboardP);
            trans = RANK(sq);
            Value = Yakov_black[384 * POSITION->wKsq + 6 * POSITION->bKsq + trans - 1]&(1 << FILE(sq));

            if( !Value )
                POSITION->DYN->Value = 0;
            else
                POSITION->DYN->Value = -((sint16)(POSITION->DYN->STATIC & 0xffff)) - 75 * trans - 250;
            }
        }

    if( POSITION->DYN->Value > 0 )
        {
        if( (bBitboardP & ~FILEh) == 0 && (AttK[POSITION->wKsq] | wBitboardK) & SqSet[H1] )
            POSITION->DYN->Value = 0;

        if( (bBitboardP & ~FILEa) == 0 && (AttK[POSITION->wKsq] | wBitboardK) & SqSet[A1] )
            POSITION->DYN->Value = 0;

        if( (POSITION->DYN->flags & 28) == 28 )
            {
            sq = H8 - MSB(bBitboardP);
            trans = RANK(sq);
            Value = Yakov_white[384 * (H8 - POSITION->bKsq) + 6 * (H8 - POSITION->wKsq) + trans - 1]&(1 << FILE(sq));

            if( !Value )
                POSITION->DYN->Value = 0;
            else
                POSITION->DYN->Value = -((sint16)(POSITION->DYN->STATIC & 0xffff)) + 75 * trans + 250;
            }
        }
    }

void Eval( typePOS *POSITION, int min, int max, int move )
    {
    typePawnEval *PAWN_PTR;
    int index, matval, Value;
    int b, trans, antiphase, phase;
    int to, cp, wKs, bKs;
	uint64 U, wKatt, bKatt, A, AttB, AttR;
	sint32 wKhit, bKhit;
    uint64 wGoodMinor, bGoodMinor, wSafeMob, bSafeMob, wOKxray, bOKxray;
    uint64 T, bPatt, wPatt;
    int open, end;
    uint8 bGoodAtt, wGoodAtt;
    uint8 TOKEN;
    int v, positional;
    typePawnEval PAWN_INFO[1];
    int ch;
    uint32 r;
    int n, adj;
    int mask;

    PAWN_PTR = PawnHash + (POSITION->DYN->PAWN_HASH &(PawnHashSize - 1));
    PREFETCH_PAWN_HASH;
    index = (POSITION->DYN->material >> 8) & 0x7ffff;
    TOKEN = MATERIAL[index].token;
    POSITION->DYN->flags = MATERIAL[index].flags;
    POSITION->DYN->exact = false;

    if( !(POSITION->DYN->material & 0x80000000) )
        matval = MATERIAL[index].Value;
    else
        {
        if( POPCNT(wBitboardQ) > 1 || POPCNT(bBitboardQ) > 1 || POPCNT(wBitboardR) > 2 || POPCNT(bBitboardR) > 2
            || POPCNT(wBitboardBL) > 1 || POPCNT(bBitboardBL) > 1 || POPCNT(wBitboardBD) > 1 || POPCNT(bBitboardBD) > 1
            || POPCNT(wBitboardN) > 2 || POPCNT(bBitboardN) > 2 )
            {
            TOKEN = 0x80;
            matval = MaterialValue(POSITION);
            POSITION->DYN->flags = 0;

            if( wBitboardQ | wBitboardR | wBitboardB | wBitboardN )
                POSITION->DYN->flags |= 2;

            if( bBitboardQ | bBitboardR | bBitboardB | bBitboardN )
                POSITION->DYN->flags |= 1;
            }
        else
            {
            matval = MATERIAL[index].Value;
            POSITION->DYN->material &= 0x7fffffff;
            }
        }

    if( ((POSITION->DYN->HASH ^ EvalHash[POSITION->DYN->HASH & HASH_MASK]) & 0xffffffffffff0000) == 0 )
        {
        Value = (int)((sint16)(EvalHash[POSITION->DYN->HASH & HASH_MASK] & 0xffff));
        POSITION->DYN->lazy = 0;
        Mobility(POSITION);
        POSITION->DYN->PositionalValue = ((POSITION->wtm) ? Value : -Value) - matval;
        POSITION->DYN->Value = Value;

        if( move && !(POSITION->DYN - 1)->lazy )
            {
            POSITION->wtm ? AdjustPositionalGainW(POSITION, move) : AdjustPositionalGainB(POSITION, move);
            }

        if( Value > 15000 || Value < -15000 )
            POSITION->DYN->exact = true;
        return;
        }

    memcpy(PAWN_INFO, PAWN_PTR, sizeof(typePawnEval));

    PAWN_INFO->PAWN_HASH ^= (((uint64 *)(PAWN_INFO)) + 0x1)[0];
    PAWN_INFO->PAWN_HASH ^= (((uint64 *)(PAWN_INFO)) + 0x2)[0];
    PAWN_INFO->PAWN_HASH ^= (((uint64 *)(PAWN_INFO)) + 0x3)[0];

    if( (POSITION->DYN->material & 0xff) == 0 )
        {
        POSITION->wtm
            ? KingPawnWhite(POSITION, matval, TOKEN, PAWN_INFO) : KingPawnBlack(POSITION, matval, TOKEN, PAWN_INFO);
        POSITION->DYN->lazy = 1;
        POSITION->DYN->PositionalValue = 0;
        EvalHash[POSITION->DYN->HASH & HASH_MASK] =
            (POSITION->DYN->HASH & 0xffffffffffff0000) | (POSITION->DYN->Value & 0xffff);
        return;
        }

#define WHITE_MINOR_ONLY_SHIFT (8 << 2)
#define BLACK_MINOR_ONLY_SHIFT (16 << 2)

    if( (POSITION->DYN->flags & WHITE_MINOR_ONLY_SHIFT || POSITION->DYN->flags & BLACK_MINOR_ONLY_SHIFT)
        && PAWN_INFO->PAWN_HASH != POSITION->DYN->PAWN_HASH )
        PawnEval(POSITION, PAWN_INFO);

    if( (POSITION->DYN->flags & WHITE_MINOR_ONLY_SHIFT && PAWN_INFO->wPfile_count <= 1)
        || (POSITION->DYN->flags & BLACK_MINOR_ONLY_SHIFT && PAWN_INFO->bPfile_count <= 1)
            || (POSITION->DYN->flags & 128) )
        ;
    else
        {
        if( POSITION->wtm )
            {
            positional = (POSITION->DYN - 1)->PositionalValue;
            cp = POSITION->DYN->cp;
            to = TO(move);
            Value = PieceSquareValue[POSITION->sq[to]][to] - PieceSquareValue[POSITION->sq[to]][FROM(move)];

            if( cp )
                Value -= PieceSquareValue[cp][to];
			phase = MIN (POSITION->DYN->material & 0xff, 32);
            end = (sint16)(Value & 0xffff);
            open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
            antiphase = 32 - phase;
            Value = (end * antiphase + open * phase) / 32;
            positional += Value;
            v = positional + matval;

            if( v < -max - 16 * (int) (POSITION->DYN - 1)->lazy
            || v > -min + 16 * (int) (POSITION->DYN - 1)->lazy )
                {
                POSITION->DYN->lazy = (POSITION->DYN - 1)->lazy + 1;
                POSITION->DYN->Value = v;
                POSITION->DYN->PositionalValue = positional;
                Mobility(POSITION);
                return;
                }
            }
        else
            {
            positional = (POSITION->DYN - 1)->PositionalValue;
            cp = POSITION->DYN->cp;
            to = TO(move);
            Value = PieceSquareValue[POSITION->sq[to]][to] - PieceSquareValue[POSITION->sq[to]][FROM(move)];

            if( cp )
                Value -= PieceSquareValue[cp][to];
			phase = MIN (POSITION->DYN->material & 0xff, 32);
            end = (sint16)(Value & 0xffff);
            open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
            antiphase = 32 - phase;
            Value = (end * antiphase + open * phase) / 32;
            positional += Value;
            v = positional + matval;

            if( v < min - 16 * (int) (POSITION->DYN - 1)->lazy
            || v > max + 16 * (int) (POSITION->DYN - 1)->lazy )
                {
                POSITION->DYN->lazy = (POSITION->DYN - 1)->lazy + 1;
                POSITION->DYN->Value = -v;
                POSITION->DYN->PositionalValue = positional;
                Mobility(POSITION);
                return;
                }
            }
        }

    wKs = POSITION->wKsq;
    bKs = POSITION->bKsq;
    wKatt = AttK[wKs];
	bKatt = AttK[bKs];
    bGoodAtt = wGoodAtt = 0;

    if( PAWN_INFO->PAWN_HASH != POSITION->DYN->PAWN_HASH )
        PawnEval(POSITION, PAWN_INFO);
    Value = (POSITION->DYN->STATIC)+(PAWN_INFO->SCORE);

    POSITION->DYN->wXray = 0;

    A = (wBitboardP &( ~FILEa)) << 7;
    T = A & bBitboardK;
    POSITION->DYN->bKcheck = (T >> 7);
    wPatt = A;
    A = (wBitboardP &( ~FILEh)) << 9;
    T = A & bBitboardK;
    POSITION->DYN->bKcheck |= (T >> 9);
    wPatt |= A;
    POSITION->DYN->wAtt = wPatt;
    A = (bBitboardP &( ~FILEh)) >> 7;
    T = A & wBitboardK;
    POSITION->DYN->wKcheck = (T << 7);
    bPatt = A;
    A = (bBitboardP &( ~FILEa)) >> 9;
    T = A & wBitboardK;
    POSITION->DYN->wKcheck |= (T << 9);
    bPatt |= A;
	POSITION->DYN->bAtt = bPatt;

    bOKxray = ( ~bBitboardP) &~wPatt;
    wOKxray = ( ~wBitboardP) &~bPatt;
    wGoodMinor = (wBitboardN | wBitboardB) & wPatt;
    bGoodMinor = (bBitboardN | bBitboardB) & bPatt;

	if (wPatt & bKatt)
		bKhit = HitP;
	else
		bKhit = 0;
    U = (POSITION->OccupiedBW >> 8) & wBitboardP;

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

        if( bBitboardK & DIAG[b] )
            {
            T = AttB(bKs) & AttB;

            if( T )
                {
                Value += wQxrayD[POSITION->sq[LSB(T)]];
                POSITION->DYN->wXray |= T;
                POSITION->XRAYw[LSB(T)] = b;
                }
            }
        else if( bBitboardK & ORTHO[b] )
            {
            T = AttR(bKs) & AttR;

            if( T )
                {
                Value += wQxrayO[POSITION->sq[LSB(T)]];
                POSITION->DYN->wXray |= T;
                POSITION->XRAYw[LSB(T)] = b;
                }
            }
        A = AttB | AttR;
        T = A & wSafeMob;
        POSITION->DYN->wAtt |= A;

		if (A & bKatt)
			bKhit += HitQ;

        if( A & bBitboardK )
            POSITION->DYN->bKcheck |= SqSet[b];

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
        POSITION->DYN->wAtt |= A;

        if( bBitboardK & ORTHO[b] )
            {
            T = AttR(bKs) & A;

            if( T )
                {
                Value += wRxray[POSITION->sq[LSB(T)]];
                POSITION->DYN->wXray |= T;
                POSITION->XRAYw[LSB(T)] = b;
                }
            }
		if (A & bKatt)
			bKhit += HitR;

        if( A & bBitboardK )
            POSITION->DYN->bKcheck |= SqSet[b];

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
        POSITION->DYN->wAtt |= A;

        if( bBitboardK & DIAG[b] )
            {
            T = AttB(bKs) & A;

            if( T )
                {
                Value += wBxray[POSITION->sq[LSB(T)]];
                POSITION->DYN->wXray |= T;
                POSITION->XRAYw[LSB(T)] = b;
                }
            }
		if (A & bKatt)
			bKhit += HitB;

        if( A & bBitboardK )
            POSITION->DYN->bKcheck |= SqSet[b];

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

        if( SqSet[b] & LIGHT )
            {
            Value -= (PAWN_INFO->wPlight + PAWN_INFO->bPlight / 2) * SCORE(1, 1);
            Value += POPCNT(bBitboardP & LIGHT & InFrontB[RANK(b)] & ~bPatt) * SCORE(0, 2);
            }
        else
            {
            Value -= (PAWN_INFO->wPdark + PAWN_INFO->bPdark / 2) * SCORE(1, 1);
            Value += POPCNT(bBitboardP & DARK & InFrontB[RANK(b)] & ~bPatt) * SCORE(0, 2);
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

        if( POSITION->sq[BishopTrapSq[b]] == bEnumP )
            {
            Value -= BishopTrapValue;

            if( POSITION->sq[GoodBishopTrapSq[b]] == bEnumP )
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
        POSITION->DYN->wAtt |= A;

		if (A & (bKatt | bBitboardK))
			bKhit += HitN;

        if( A & bBitboardK )
            POSITION->DYN->bKcheck |= SqSet[b];

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
    U = (POSITION->OccupiedBW << 8) & bBitboardP;
    POSITION->DYN->bXray = 0;

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

        if( wBitboardK & DIAG[b] )
            {
            T = AttB(wKs) & AttB;

            if( T )
                {
                Value -= bQxrayD[POSITION->sq[LSB(T)]];
                POSITION->DYN->bXray |= T;
                POSITION->XRAYb[LSB(T)] = b;
                }
            }
        else if( wBitboardK & ORTHO[b] )
            {
            T = AttR(wKs) & AttR;

            if( T )
                {
                Value -= bQxrayO[POSITION->sq[LSB(T)]];
                POSITION->DYN->bXray |= T;
                POSITION->XRAYb[LSB(T)] = b;
                }
            }
        A = AttB | AttR;
        T = A & bSafeMob;
		POSITION->DYN->bAtt |= A;

        if( A & wKatt )
            wKhit += HitQ;

        if( A & wBitboardK )
            POSITION->DYN->wKcheck |= SqSet[b];

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
		POSITION->DYN->bAtt |= A;

        if( wBitboardK & ORTHO[b] )
            {
            T = A & AttR(wKs);

            if( T )
                {
                Value -= bRxray[POSITION->sq[LSB(T)]];
                POSITION->DYN->bXray |= T;
                POSITION->XRAYb[LSB(T)] = b;
                }
            }

        if( A & wKatt )
            wKhit += HitR;

        if( A & wBitboardK )
            POSITION->DYN->wKcheck |= SqSet[b];

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
		POSITION->DYN->bAtt |= A;

        if( wBitboardK & DIAG[b] )
            {
            T = A & AttB(wKs);

            if( T )
                {
                Value -= bBxray[POSITION->sq[LSB(T)]];
                POSITION->DYN->bXray |= T;
                POSITION->XRAYb[LSB(T)] = b;
                }
            }

        if( A & wKatt )
            wKhit += HitB;

        if( A & wBitboardK )
            POSITION->DYN->wKcheck |= SqSet[b];

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

        if( SqSet[b] & LIGHT )
            {
            Value += (PAWN_INFO->bPlight + PAWN_INFO->wPlight / 2) * SCORE(1, 1);
            Value -= POPCNT(wBitboardP & LIGHT & InFrontW[RANK(b)] & ~wPatt) * SCORE(0, 2);
            }
        else
            {
            Value += (PAWN_INFO->bPdark + PAWN_INFO->wPdark / 2) * SCORE(1, 1);
            Value -= POPCNT(wBitboardP & DARK & InFrontW[RANK(b)] & ~wPatt) * SCORE(0, 2);
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

        if( POSITION->sq[BishopTrapSq[b]] == wEnumP )
            {
            Value += BishopTrapValue;

            if( POSITION->sq[GoodBishopTrapSq[b]] == wEnumP )
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
		POSITION->DYN->bAtt |= A;

        if( A &(wKatt | wBitboardK) )
            wKhit += HitN;

        if( A & wBitboardK )
            POSITION->DYN->wKcheck |= SqSet[b];

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

    POSITION->DYN->wAtt |= wKatt;
	POSITION->DYN->bAtt |= bKatt;

	if (bKatt & wBitboardK)
        {
        POSITION->DYN->wKcheck |= SqSet[POSITION->bKsq];
        POSITION->DYN->bKcheck |= SqSet[POSITION->wKsq];
        }

	if ((~POSITION->DYN->bAtt) & wKatt & bBitboardP)
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
            Value -= SCORE(10 * (6 - t), 0);
            }
        }

	if (wKatt & bKatt)
        wKhit += HitK;
    ch = (((KingSafetyMult[wKhit >> 16] * (wKhit & 0xffff)) / KingSafetyDivider) << 16) + PAWN_INFO->wKdanger;

    if( !bBitboardQ )
        {
        ch >>= 16;
        ch *= POPCNT(bBitboardR | bBitboardN | bBitboardB);
        ch >>= 3;
        ch <<= 16;
        }
    Value -= ch;

	if ((~POSITION->DYN->wAtt) & bKatt & wBitboardP)
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
            Value += SCORE(10 * (t - 1), 0);
            }
        }

	if (wKatt & bKatt)
		bKhit += HitK;
	ch = (((KingSafetyMult[bKhit >> 16] *
		(bKhit & 0xffff)) / KingSafetyDivider) << 16) + PAWN_INFO->bKdanger;

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

#define QueenEnd ((POSITION->DYN->flags & 28) == 4)
#define RookEnd ((POSITION->DYN->flags & 28) == 8)

    if( (wBitboardR | wBitboardQ) & CrampFile[FILE(bKs)] )
        {
        Value += SCORE(0, 5);

        if( (CrampFile[FILE(bKs)]&(wBitboardP | bBitboardP)) == 0 )
            Value += SCORE(5, 15);
        }

    if( (bBitboardR | bBitboardQ) & CrampFile[FILE(wKs)] )
        {
        Value -= SCORE(0, 5);

        if( (CrampFile[FILE(wKs)]&(bBitboardP | wBitboardP)) == 0 )
            Value -= SCORE(5, 15);
        }

    U = PAWN_INFO->wPassedFiles;

    while( U )
        {
        b = MSB(FileArray[LSB(U)] & wBitboardP);
        BitClear(0, U);
        trans = RANK(b);

        if( trans <= R3 )
            continue;

        if( RookEnd )
            {
            if( wBitboardR & OpenFileW[b] )
                {
                if( trans == R7 )
                    {
                    Value -= Rook7thEnd;
                    }
                else if( trans == R6 )
                    {
                    Value -= Rook6thEnd;
                    }
                }

            if( OpenFileW[b] & wBitboardK && CrampFile[FILE(wKs)] & bBitboardR )
                Value -= SCORE(0, 1 << (trans - R2));
            }

        if( POSITION->sq[b + 8] == 0 )
            {
            Value += PassedPawnCanMove[trans];
            }

        if( (OpenFileW[b]&wBitboardOcc) == 0 )
            {
            Value += PassedPawnMeClear[trans];
            }

        if( (OpenFileW[b]&bBitboardOcc) == 0 )
            {
            Value += PassedPawnOppClear[trans];
            }

		if ((OpenFileW[b] & (~POSITION->DYN->wAtt) & POSITION->DYN->bAtt) == 0)
            {
            Value += PassedPawnIsFree[trans];
            }

        if( QueenEnd )
            {
            if( trans == R7 && wBitboardQ & OpenFileW[b] )
                {
                Value -= Queen7thEnd;
                }
            Value += RankQueenEnd[trans];
            }
        }

    U = PAWN_INFO->bPassedFiles;

    while( U )
            {
            b = LSB(FileArray[LSB(U)] & bBitboardP);
            BitClear(0, U);
            trans = RANK(b);

            if( trans >= R6 )
                continue;

            if( RookEnd )
                {
                if( bBitboardR & OpenFileB[b] )
                    {
                    if( trans == R2 )
                        {
                        Value += Rook7thEnd;
                        }
                    else if( trans == R3 )
                        {
                        Value += Rook6thEnd;
                        }
                    }

                if( OpenFileB[b] & bBitboardK && CrampFile[FILE(bKs)] & wBitboardR )
                    Value += SCORE(0, 1 << (R7 - trans));
                }

            if( POSITION->sq[b - 8] == 0 )
                {
                Value -= PassedPawnCanMove[7 - trans];
                }

            if( (OpenFileB[b]&bBitboardOcc) == 0 )
                {
                Value -= PassedPawnMeClear[7 - trans];
                }

            if( (OpenFileB[b]&wBitboardOcc) == 0 )
                {
                Value -= PassedPawnOppClear[7 - trans];
                }

			if ((OpenFileB[b] & POSITION->DYN->
				wAtt & ~POSITION->DYN->bAtt) == 0)
                {
                Value -= PassedPawnIsFree[7 - trans];
                }

            if( QueenEnd )
                {
                if( trans == R2 && bBitboardQ & OpenFileB[b] )
                    {
                    Value += Queen7thEnd;
                    }
                Value -= RankQueenEnd[7 - trans];
                }
            }

	phase = MIN (POSITION->DYN->material & 0xff, 32);
    end = (sint16)(Value & 0xffff);
    open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
    antiphase = 32 - phase;
    Value = end * antiphase + open * phase;
    Value = Value / 32 + matval;
    Value = (Value * TOKEN) / 128;

    if( Value > 0 )
        Value -= (PAWN_INFO->wDrawWeight * MIN(Value, 100)) / 64;
    else
        Value += (PAWN_INFO->bDrawWeight * MIN(-Value, 100)) / 64;

#define BISHOP_KNIGHT_MATE (POSITION->DYN->flags & 128)
#define MAX_DIST(i,j) (MAX (FileDistance (i, j), RankDistance (i, j)))
#define MIN_DIST(i,j) (MIN (FileDistance (i, j), RankDistance (i, j)))

    if( BISHOP_KNIGHT_MATE )
        {
        if( Value > 0 )
            {
            if( wBitboardBL )
                Value -=
                    20 * MIN(MAX_DIST(A8, POSITION->bKsq), MAX_DIST(H1, POSITION->bKsq))
                        + 10 * MIN(MIN_DIST(A8, POSITION->bKsq), MIN_DIST(H1, POSITION->bKsq));
            else
                Value -=
                    20 * MIN(MAX_DIST(A1, POSITION->bKsq), MAX_DIST(H8, POSITION->bKsq))
                        + 10 * MIN(MIN_DIST(A1, POSITION->bKsq), MIN_DIST(H8, POSITION->bKsq));
            }
        else
            {
            if( bBitboardBL )
                Value +=
                    20 * MIN(MAX_DIST(A8, POSITION->wKsq), MAX_DIST(H1, POSITION->wKsq))
                        + 10 * MIN(MIN_DIST(A8, POSITION->wKsq), MIN_DIST(H1, POSITION->wKsq));
            else
                Value +=
                    20 * MIN(MAX_DIST(A1, POSITION->wKsq), MAX_DIST(H8, POSITION->wKsq))
                        + 10 * MIN(MIN_DIST(A1, POSITION->wKsq), MIN_DIST(H8, POSITION->wKsq));
            }
        }

    if( POSITION->DYN->reversible > 50 )
        {
        Value *= (114 - POSITION->DYN->reversible);
        Value /= 64;
        }

#define WHITE_MINOR_ONLY (POSITION->DYN->flags & 32)
#define WHITE_HAS_PIECE (POSITION->DYN->flags & 2)
#define BLACK_MINOR_ONLY (POSITION->DYN->flags & 64)
#define BLACK_HAS_PIECE (POSITION->DYN->flags & 1)

    if( Value > 0 )
        {
        if( !POSITION->wtm && !BLACK_HAS_PIECE && (bBitboardK ^ bBitboardP) == bBitboardOcc
            && !((bBitboardP >> 8) & ~POSITION->OccupiedBW) && !(AttK[POSITION->bKsq]& ~POSITION->DYN->wAtt)
            && !POSITION->DYN->bKcheck )
            Value = 0;

        if( WHITE_MINOR_ONLY )
            {
            if( wBitboardN )
                {
                if( wBitboardP == SqSet[A7] && (bBitboardK | AttK[POSITION->bKsq]) & SqSet[A8] )
                    Value = 0;

                if( wBitboardP == SqSet[H7] && (bBitboardK | AttK[POSITION->bKsq]) & SqSet[H8] )
                    Value = 0;
                }
            else if( wBitboardBL && !(wBitboardP &NOTh) && (bBitboardK | AttK[POSITION->bKsq]) & SqSet[H8] )
                {
                if( wBitboardP & SqSet[H5] && bBitboardP == (SqSet[G7] | SqSet[H6]) )
                    ;
                else
                    Value = 0;
                }
            else if( wBitboardBD && !(wBitboardP &NOTa) && (bBitboardK | AttK[POSITION->bKsq]) & SqSet[A8] )
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
        if( POSITION->wtm && !WHITE_HAS_PIECE && (wBitboardK ^ wBitboardP) == wBitboardOcc
            && !((wBitboardP << 8) & ~POSITION->OccupiedBW) && !(AttK[POSITION->wKsq]& ~POSITION->DYN->bAtt)
            && !POSITION->DYN->wKcheck )
            Value = 0;

        if( BLACK_MINOR_ONLY )
            {
            if( bBitboardN )
                {
                if( bBitboardP == SqSet[A2] && (wBitboardK | AttK[POSITION->wKsq]) & SqSet[A1] )
                    Value = 0;

                if( bBitboardP == SqSet[H2] && (wBitboardK | AttK[POSITION->wKsq]) & SqSet[H1] )
                    Value = 0;
                }
            else if( bBitboardBD && !(bBitboardP &NOTh) && (wBitboardK | AttK[POSITION->wKsq]) & SqSet[H1] )
                {
                if( bBitboardP & SqSet[H4] && wBitboardP == (SqSet[G2] | SqSet[H3]) )
                    ;
                else
                    Value = 0;
                }
            else if( bBitboardBL && !(bBitboardP &NOTa) && (wBitboardK | AttK[POSITION->wKsq]) & SqSet[A1] )
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

    n = 0;
    adj = 0;
    mask = (1 << RANDOM_BITS) - 1;
    r = Random32(POSITION->cpu);

    for ( n = 0; n < RANDOM_COUNT; n++ )
        {
        adj = r & mask;
        r >>= RANDOM_BITS;
        Value += (r & 1) ? adj : -adj;
        r >>= 1;
        }

    POSITION->DYN->Value = POSITION->wtm ? Value : -Value;
    POSITION->DYN->PositionalValue = Value - matval;
    POSITION->DYN->lazy = 0;
    EvalHash[POSITION->DYN->HASH & HASH_MASK] =
        (POSITION->DYN->HASH & 0xffffffffffff0000) | (POSITION->DYN->Value & 0xffff);

    if( move && !(POSITION->DYN - 1)->lazy )
        POSITION->wtm ? AdjustPositionalGainW(POSITION, move) : AdjustPositionalGainB(POSITION, move);
    }
