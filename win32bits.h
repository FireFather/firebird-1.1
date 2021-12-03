static __inline int LSB( UINT64 x )
    {
    _asm
        {
        bsf eax, dword ptr x[0]
		jnz f_end
		bsf eax, dword ptr x[4]
		jz f_end
		add eax, 20h
	f_end:
        }
    }
static __inline int MSB( UINT64 x )
    {
    _asm
        {
        bsr	eax, dword ptr x[4]
		jz	l_lo
		add eax, 20h
		jmp	l_end
	l_lo: bsr eax, dword ptr x[0]
	l_end:
        }
    }

static __inline int POPCNT( UINT64 v )
    {
    unsigned int v1, v2;
    v1 = (unsigned int)(v & 0xFFFFFFFF);
    v1 -= (v1 >> 1) & 0x55555555;
    v1 = (v1 & 0x33333333) + ((v1 >> 2) & 0x33333333);
    v1 = (v1 + (v1 >> 4)) & 0x0F0F0F0F;
    v2 = (unsigned int)(v >> 32);
    v2 -= (v2 >> 1) & 0x55555555;
    v2 = (v2 & 0x33333333) + ((v2 >> 2) & 0x33333333);
    v2 = (v2 + (v2 >> 4)) & 0x0F0F0F0F;
    return ((v1 * 0x01010101) >> 24) + ((v2 * 0x01010101) >> 24);
    }

#define BitClear(b, B) B &= (B - 1)
#define BitSet(b, B) B |= ((uint64) 1) << (b)
