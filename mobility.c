
#include "firebird.h"

void Mobility (typePos* Position)
{
  uint64 U, A, T, AttB, AttR;
  int b;
  Position->Current->wXray = 0;
  Position->Current->bXray = 0;
  A = AttK[Position->wKsq];
  Position->Current->wAtt = A;
  if (A & bBitboardK)
    Position->Current->bKcheck = SqSet[Position->wKsq];
  else
    Position->Current->bKcheck = 0;
  A = AttK[Position->bKsq];
  Position->Current->bAtt = A;
  if (A & wBitboardK)
    Position->Current->wKcheck = SqSet[Position->bKsq];
  else
    Position->Current->wKcheck = 0;
  for (U = wBitboardN; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttN[b];
      Position->Current->wAtt |= A;
      if (A & bBitboardK)
	Position->Current->bKcheck |= SqSet[b];
    }
  for (U = wBitboardB; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttB (b);
      Position->Current->wAtt |= A;
      if (A & bBitboardK)
	Position->Current->bKcheck |= SqSet[b];
      else if (bBitboardK & Diag[b])
	{
	  T = AttB (Position->bKsq) & A;
	  Position->Current->wXray |= T;
	  if (T)
	    Position->XrayW[LSB (T)] = b;
	}
    }
  for (U = wBitboardR; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttR (b);
      Position->Current->wAtt |= A;
      if (A & bBitboardK)
	Position->Current->bKcheck |= SqSet[b];
      else if (bBitboardK & Ortho[b])
	{
	  T = AttR (Position->bKsq) & A;
	  Position->Current->wXray |= T;
	  if (T)
	    Position->XrayW[LSB (T)] = b;
	}
    }
  for (U = wBitboardQ; U; BitClear (b, U))
    {
      b = LSB (U);
      AttR = AttR (b);
      AttB = AttB (b);
      A = AttB | AttR;
      Position->Current->wAtt |= A;
      if (A & bBitboardK)
	Position->Current->bKcheck |= SqSet[b];
      else if (bBitboardK & Diag[b])
	{
	  T = AttB (Position->bKsq) & AttB;
	  Position->Current->wXray |= T;
	  if (T)
	    Position->XrayW[LSB (T)] = b;
	}
      else if (bBitboardK & Ortho[b])
	{
	  T = AttR (Position->bKsq) & AttR;
	  Position->Current->wXray |= T;
	  if (T)
	    Position->XrayW[LSB (T)] = b;
	}
    }
  for (U = bBitboardN; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttN[b];
      Position->Current->bAtt |= A;
      if (A & wBitboardK)
	Position->Current->wKcheck |= SqSet[b];
    }
  for (U = bBitboardB; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttB (b);
      Position->Current->bAtt |= A;
      if (A & wBitboardK)
	Position->Current->wKcheck |= SqSet[b];
      else if (wBitboardK & Diag[b])
	{
	  T = AttB (Position->wKsq) & A;
	  Position->Current->bXray |= T;
	  if (T)
	    Position->XrayB[LSB (T)] = b;
	}
    }
  for (U = bBitboardR; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttR (b);
      Position->Current->bAtt |= A;
      if (A & wBitboardK)
	Position->Current->wKcheck |= SqSet[b];
      else if (wBitboardK & Ortho[b])
	{
	  T = AttR (Position->wKsq) & A;
	  Position->Current->bXray |= T;
	  if (T)
	    Position->XrayB[LSB (T)] = b;
	}
    }
  for (U = bBitboardQ; U; BitClear (b, U))
    {
      b = LSB (U);
      AttB = AttB (b);
      AttR = AttR (b);
      A = AttB | AttR;
      Position->Current->bAtt |= A;
      if (A & wBitboardK)
	Position->Current->wKcheck |= SqSet[b];
      else if (wBitboardK & Diag[b])
	{
	  T = AttB (Position->wKsq) & AttB;
	  Position->Current->bXray |= T;
	  if (T)
	    Position->XrayB[LSB (T)] = b;
	}
      else if (wBitboardK & Ortho[b])
	{
	  T = AttR (Position->wKsq) & AttR;
	  Position->Current->bXray |= T;
	  if (T)
	    Position->XrayB[LSB (T)] = b;
	}
    }
  A = (wBitboardP & (~FileA)) << 7;
  T = A & bBitboardK;
  Position->Current->bKcheck |= (T >> 7);
  Position->Current->wAtt |= A;
  A = (wBitboardP & (~FileH)) << 9;
  T = A & bBitboardK;
  Position->Current->bKcheck |= (T >> 9);
  Position->Current->wAtt |= A;
  A = (bBitboardP & (~FileH)) >> 7;
  T = A & wBitboardK;
  Position->Current->wKcheck |= (T << 7);
  Position->Current->bAtt |= A;
  A = (bBitboardP & (~FileA)) >> 9;
  T = A & wBitboardK;
  Position->Current->wKcheck |= (T << 9);
  Position->Current->bAtt |= A;
}
