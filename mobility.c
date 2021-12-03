
#include "firebird.h"

void Mobility (typePOS* POSITION)
{
  uint64 U, A, T, AttB, AttR;
  int b;
  POSITION->DYN->wXray = 0;
  POSITION->DYN->bXray = 0;
  A = AttK[POSITION->wKsq];
  POSITION->DYN->wAtt = A;
  if (A & bBitboardK)
    POSITION->DYN->bKcheck = SqSet[POSITION->wKsq];
  else
    POSITION->DYN->bKcheck = 0;
  A = AttK[POSITION->bKsq];
  POSITION->DYN->bAtt = A;
  if (A & wBitboardK)
    POSITION->DYN->wKcheck = SqSet[POSITION->bKsq];
  else
    POSITION->DYN->wKcheck = 0;
  for (U = wBitboardN; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttN[b];
      POSITION->DYN->wAtt |= A;
      if (A & bBitboardK)
	POSITION->DYN->bKcheck |= SqSet[b];
    }
  for (U = wBitboardB; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttB (b);
      POSITION->DYN->wAtt |= A;
      if (A & bBitboardK)
	POSITION->DYN->bKcheck |= SqSet[b];
      else if (bBitboardK & DIAG[b])
	{
	  T = AttB (POSITION->bKsq) & A;
	  POSITION->DYN->wXray |= T;
	  if (T)
	    POSITION->XRAYw[LSB (T)] = b;
	}
    }
  for (U = wBitboardR; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttR (b);
      POSITION->DYN->wAtt |= A;
      if (A & bBitboardK)
	POSITION->DYN->bKcheck |= SqSet[b];
      else if (bBitboardK & ORTHO[b])
	{
	  T = AttR (POSITION->bKsq) & A;
	  POSITION->DYN->wXray |= T;
	  if (T)
	    POSITION->XRAYw[LSB (T)] = b;
	}
    }
  for (U = wBitboardQ; U; BitClear (b, U))
    {
      b = LSB (U);
      AttR = AttR (b);
      AttB = AttB (b);
      A = AttB | AttR;
      POSITION->DYN->wAtt |= A;
      if (A & bBitboardK)
	POSITION->DYN->bKcheck |= SqSet[b];
      else if (bBitboardK & DIAG[b])
	{
	  T = AttB (POSITION->bKsq) & AttB;
	  POSITION->DYN->wXray |= T;
	  if (T)
	    POSITION->XRAYw[LSB (T)] = b;
	}
      else if (bBitboardK & ORTHO[b])
	{
	  T = AttR (POSITION->bKsq) & AttR;
	  POSITION->DYN->wXray |= T;
	  if (T)
	    POSITION->XRAYw[LSB (T)] = b;
	}
    }
  for (U = bBitboardN; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttN[b];
      POSITION->DYN->bAtt |= A;
      if (A & wBitboardK)
	POSITION->DYN->wKcheck |= SqSet[b];
    }
  for (U = bBitboardB; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttB (b);
      POSITION->DYN->bAtt |= A;
      if (A & wBitboardK)
	POSITION->DYN->wKcheck |= SqSet[b];
      else if (wBitboardK & DIAG[b])
	{
	  T = AttB (POSITION->wKsq) & A;
	  POSITION->DYN->bXray |= T;
	  if (T)
	    POSITION->XRAYb[LSB (T)] = b;
	}
    }
  for (U = bBitboardR; U; BitClear (b, U))
    {
      b = LSB (U);
      A = AttR (b);
      POSITION->DYN->bAtt |= A;
      if (A & wBitboardK)
	POSITION->DYN->wKcheck |= SqSet[b];
      else if (wBitboardK & ORTHO[b])
	{
	  T = AttR (POSITION->wKsq) & A;
	  POSITION->DYN->bXray |= T;
	  if (T)
	    POSITION->XRAYb[LSB (T)] = b;
	}
    }
  for (U = bBitboardQ; U; BitClear (b, U))
    {
      b = LSB (U);
      AttB = AttB (b);
      AttR = AttR (b);
      A = AttB | AttR;
      POSITION->DYN->bAtt |= A;
      if (A & wBitboardK)
	POSITION->DYN->wKcheck |= SqSet[b];
      else if (wBitboardK & DIAG[b])
	{
	  T = AttB (POSITION->wKsq) & AttB;
	  POSITION->DYN->bXray |= T;
	  if (T)
	    POSITION->XRAYb[LSB (T)] = b;
	}
      else if (wBitboardK & ORTHO[b])
	{
	  T = AttR (POSITION->wKsq) & AttR;
	  POSITION->DYN->bXray |= T;
	  if (T)
	    POSITION->XRAYb[LSB (T)] = b;
	}
    }
  A = (wBitboardP & (~FILEa)) << 7;
  T = A & bBitboardK;
  POSITION->DYN->bKcheck |= (T >> 7);
  POSITION->DYN->wAtt |= A;
  A = (wBitboardP & (~FILEh)) << 9;
  T = A & bBitboardK;
  POSITION->DYN->bKcheck |= (T >> 9);
  POSITION->DYN->wAtt |= A;
  A = (bBitboardP & (~FILEh)) >> 7;
  T = A & wBitboardK;
  POSITION->DYN->wKcheck |= (T << 7);
  POSITION->DYN->bAtt |= A;
  A = (bBitboardP & (~FILEa)) >> 9;
  T = A & wBitboardK;
  POSITION->DYN->wKcheck |= (T << 9);
  POSITION->DYN->bAtt |= A;
}
