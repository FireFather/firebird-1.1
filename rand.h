
static uint64 randkey = 1;
static uint16 RAND16()
    {
    randkey = randkey * 8765432181103515245ULL + 1234567891ULL;
    return ((randkey >> 32) % 65536);
    }
static uint64 GetRand()
    {
	return (((uint64) RAND16()) << 48) |
		(((uint64) RAND16()) << 32) |
		(((uint64) RAND16()) << 16) |
		(((uint64) RAND16()) << 0);
    }

typedef struct
    {
    uint64 RandKey;
    uint8 pad[56];
    } RAND;
static RAND Rand[MaxCPUs];
static uint32 Random32( int cpu )
    {
    Rand[cpu].RandKey = Rand[cpu].RandKey * 0x7913cc52088a6cfULL + 0x99f2e6bb0313ca0dULL;
    return ((Rand[cpu].RandKey >> 18) & 0xffffffff);
    }
static void InitRandom32( uint64 x )
    {
    int cpu;

    for ( cpu = 0; cpu < MaxCPUs; cpu++ )
        {
        x = x * 0xb18ec564ff729005ULL + 0x86ee25701b5e244fULL;
        Rand[cpu].RandKey = x;
        }
    }
