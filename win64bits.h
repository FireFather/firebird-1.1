#ifdef _MSC_VER
static int LSB( UINT64 x )
    {
    int y;
    _BitScanForward64(&y, x);
    return y;
    }
static int MSB( UINT64 x )
    {
    int y;
    _BitScanReverse64(&y, x);
    return y;
    }

static __inline int POPCNT( UINT64 w )
    {
    w = w - ((w >> 1) & 0x5555555555555555ULL);
    w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);
    w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
    return (w * 0x0101010101010101ull) >> 56;
    }
#endif

