#ifdef x86_64
static INLINE int BSR( uint64 w )
    {
    uint64 x;
    asm ("bsrq %1,%0\n": "=&r" (x):"r" (w));
    return x;
    }
static INLINE int BSF( uint64 w )
    {
    uint64 x;
    asm ("bsfq %1,%0\n": "=&r" (x):"r" (w));
    return x;
    }

static INLINE int POPCNT( uint64 w )
    {
    uint64 x;
    asm ("popcntq %1,%0\n": "=&r" (x):"r" (w));
    return x;
    }
#else
static INLINE int POPCNT( uint64 w )
    {
    w = w - ((w >> 1) & 0x5555555555555555ull);
    w = (w & 0x3333333333333333ull) + ((w >> 2) & 0x3333333333333333ull);
    w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0full;
    return (w * 0x0101010101010101ull) >> 56;
    }
#else  /* not x86_64 : 32-bit */

static INLINE int BSR (uint64 w)
{
  int x1, x2;
  asm ("bsr %1,%0\n" "jnz 1f\n" "bsr %0,%0\n" "subl $32,%0\n"
       "1: addl $32,%0\n": "=&q" (x1), "=&q" (x2):"1" ((int) (w >> 32)),
       "0" ((int) w));
  return x1;
}

static INLINE int BSF (uint64 w)
{
  int x1, x2;
  asm ("bsf %0,%0\n" "jnz 1f\n" "bsf %1,%0\n" "jz 1f\n" "addl $32,%0\n"
       "1:": "=&q" (x1), "=&q" (x2):"1" ((int) (w >> 32)),
       "0" ((int) w));
  return x1;
}

static INLINE int POPCNT (uint64 w)
{
  uint32 u, v;
  u = (w & 0xffffffff);
  v = w >> 32;
  u -= ((u >> 1) & 0x55555555);
  v -= ((v >> 1) & 0x55555555);
  u = (((u >> 2) & 0x33333333) + (u & 0x33333333));
  v = (((v >> 2) & 0x33333333) + (v & 0x33333333));
  u = (((u >> 4) + u) & 0x0f0f0f0f);
  v = (((v >> 4) + v) & 0x0f0f0f0f);
  return ((u * 0x01010101) >> 24) + ((v * 0x01010101) >> 24);
}
#endif

#define BitClear(b, B) B &= (B - 1)
#define BitSet(b, B) B |= ((uint64) 1) << (b)
