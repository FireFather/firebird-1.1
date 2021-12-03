static uint32 CAPTURE_VALUE[16][16];

void InitCaptureValues()
    {
    CAPTURE_VALUE[wEnumP][bEnumQ] = (0xd0 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumN][bEnumQ] = (0xcf << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumBL][bEnumQ] = (0xce << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumBD][bEnumQ] = (0xce << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumR][bEnumQ] = (0xcd << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumQ][bEnumQ] = (0xcc << 24) + (0x01 << 20);

    CAPTURE_VALUE[wEnumP][bEnumR] = (0xc8 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumN][bEnumR] = (0xc7 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumBL][bEnumR] = (0xc6 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumBD][bEnumR] = (0xc6 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumR][bEnumR] = (0xc5 << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumQ][bEnumR] = (0xc4 << 24) + (0x00 << 20);

    CAPTURE_VALUE[wEnumP][bEnumBL] = (0xc0 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumN][bEnumBL] = (0xbf << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumBL][bEnumBL] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumBD][bEnumBL] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumR][bEnumBL] = (0xbd << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumQ][bEnumBL] = (0xbc << 24) + (0x00 << 20);

    CAPTURE_VALUE[wEnumP][bEnumBD] = (0xc0 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumN][bEnumBD] = (0xbf << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumBL][bEnumBD] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumBD][bEnumBD] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumR][bEnumBD] = (0xbd << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumQ][bEnumBD] = (0xbc << 24) + (0x00 << 20);

    CAPTURE_VALUE[wEnumP][bEnumN] = (0xb8 << 24) + (0x02 << 20);
    CAPTURE_VALUE[wEnumN][bEnumN] = (0xb7 << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumBL][bEnumN] = (0xb6 << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumBD][bEnumN] = (0xb6 << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumR][bEnumN] = (0xb5 << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumQ][bEnumN] = (0xb4 << 24) + (0x00 << 20);

    CAPTURE_VALUE[wEnumP][bEnumP] = (0xb0 << 24) + (0x01 << 20);
    CAPTURE_VALUE[wEnumN][bEnumP] = (0xaf << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumBL][bEnumP] = (0xae << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumBD][bEnumP] = (0xae << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumR][bEnumP] = (0xad << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumQ][bEnumP] = (0xac << 24) + (0x00 << 20);

    CAPTURE_VALUE[wEnumK][bEnumQ] = (0xcb << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumK][bEnumR] = (0xc3 << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumK][bEnumBL] = (0xbb << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumK][bEnumBD] = (0xbb << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumK][bEnumN] = (0xb3 << 24) + (0x00 << 20);
    CAPTURE_VALUE[wEnumK][bEnumP] = (0xab << 24) + (0x00 << 20);

    CAPTURE_VALUE[wEnumK][0] = (0x07 << 24) + (0x00 << 15);
    CAPTURE_VALUE[wEnumP][0] = (0x06 << 24) + (0x01 << 15);
    CAPTURE_VALUE[wEnumN][0] = (0x05 << 24) + (0x01 << 15);
    CAPTURE_VALUE[wEnumBL][0] = (0x04 << 24) + (0x01 << 15);
    CAPTURE_VALUE[wEnumBD][0] = (0x04 << 24) + (0x01 << 15);
    CAPTURE_VALUE[wEnumR][0] = (0x03 << 24) + (0x01 << 15);
    CAPTURE_VALUE[wEnumQ][0] = (0x02 << 24) + (0x01 << 15);

    CAPTURE_VALUE[bEnumP][wEnumQ] = (0xd0 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumN][wEnumQ] = (0xcf << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumBL][wEnumQ] = (0xce << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumBD][wEnumQ] = (0xce << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumR][wEnumQ] = (0xcd << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumQ][wEnumQ] = (0xcc << 24) + (0x01 << 20);

    CAPTURE_VALUE[bEnumP][wEnumR] = (0xc8 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumN][wEnumR] = (0xc7 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumBL][wEnumR] = (0xc6 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumBD][wEnumR] = (0xc6 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumR][wEnumR] = (0xc5 << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumQ][wEnumR] = (0xc4 << 24) + (0x00 << 20);

    CAPTURE_VALUE[bEnumP][wEnumBL] = (0xc0 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumN][wEnumBL] = (0xbf << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumBL][wEnumBL] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumBD][wEnumBL] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumR][wEnumBL] = (0xbd << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumQ][wEnumBL] = (0xbc << 24) + (0x00 << 20);

    CAPTURE_VALUE[bEnumP][wEnumBD] = (0xc0 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumN][wEnumBD] = (0xbf << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumBL][wEnumBD] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumBD][wEnumBD] = (0xbe << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumR][wEnumBD] = (0xbd << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumQ][wEnumBD] = (0xbc << 24) + (0x00 << 20);

    CAPTURE_VALUE[bEnumP][wEnumN] = (0xb8 << 24) + (0x02 << 20);
    CAPTURE_VALUE[bEnumN][wEnumN] = (0xb7 << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumBL][wEnumN] = (0xb6 << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumBD][wEnumN] = (0xb6 << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumR][wEnumN] = (0xb5 << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumQ][wEnumN] = (0xb4 << 24) + (0x00 << 20);

    CAPTURE_VALUE[bEnumP][wEnumP] = (0xb0 << 24) + (0x01 << 20);
    CAPTURE_VALUE[bEnumN][wEnumP] = (0xaf << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumBL][wEnumP] = (0xae << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumBD][wEnumP] = (0xae << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumR][wEnumP] = (0xad << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumQ][wEnumP] = (0xac << 24) + (0x00 << 20);

    CAPTURE_VALUE[bEnumK][wEnumQ] = (0xcb << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumK][wEnumR] = (0xc3 << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumK][wEnumBL] = (0xbb << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumK][wEnumBD] = (0xbb << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumK][wEnumN] = (0xb3 << 24) + (0x00 << 20);
    CAPTURE_VALUE[bEnumK][wEnumP] = (0xab << 24) + (0x00 << 20);

    CAPTURE_VALUE[bEnumK][0] = (0x07 << 24) + (0x00 << 15);
    CAPTURE_VALUE[bEnumP][0] = (0x06 << 24) + (0x01 << 15);
    CAPTURE_VALUE[bEnumN][0] = (0x05 << 24) + (0x01 << 15);
    CAPTURE_VALUE[bEnumBL][0] = (0x04 << 24) + (0x01 << 15);
    CAPTURE_VALUE[bEnumBD][0] = (0x04 << 24) + (0x01 << 15);
    CAPTURE_VALUE[bEnumR][0] = (0x03 << 24) + (0x01 << 15);
    CAPTURE_VALUE[bEnumQ][0] = (0x02 << 24) + (0x01 << 15);
    }
