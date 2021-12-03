#include "firebird.h"
#include <string.h>
#include "control.h"
#include "functions.h"

static void SetOption( typePOS *POSITION, char *string )
    {
    char *v;
    int Value;

    if( !memcmp(string, "Hash", 4) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            Value = InitHash(Value);
            }
        }
    if( !memcmp(string, "Pawn Hash", 9) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            Value = InitPawnHash(Value);
            }
        }
    if( !memcmp(string, "Max Threads", 11) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( Value <= 0 )
                Value = 1;

            if( Value > MAX_THREADS )
                Value = MAX_THREADS;
            OPT_MAX_THREADS = Value;
            GetSysInfo();
            }
        }
    if( !memcmp(string, "MultiPV", 7) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            MULTI_PV = Value;

            if( MULTI_PV <= 1 )
                MULTI_PV = 1;

            if( MULTI_PV >= 250 )
                MULTI_PV = 250;
            }
        }
    if( !memcmp(string, "Pawn Value", 10) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            PValue = Value;
            }
        }
    if( !memcmp(string, "Knight Value", 12) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            NValue = Value;
            }
        }
    if( !memcmp(string, "Bishop Value", 12) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            BValue = Value;
            }
        }
    if( !memcmp(string, "Rook Value", 10) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            RValue = Value;
            }
        }
    if( !memcmp(string, "Queen Value", 11) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            QValue = Value;
            }
        }
    if( !memcmp(string, "Bishop Pair", 11) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            BPValue = Value;
            }
        }
    if( !memcmp(string, "Verification Reduction", 22) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            VERIFICATION_REDUCTION = Value;

            if( VERIFICATION_REDUCTION <= 0 )
                VERIFICATION_REDUCTION = 0;

            if( VERIFICATION_REDUCTION >= 8 )
                VERIFICATION_REDUCTION = 8;
            }
        }
	if( !memcmp(string, "Move on Ponderhit", 17) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( !strcmp(v + 6, "Never") )
                MPH = 1;

            if( !strcmp(v + 6, "Sometimes") )
                MPH = 2;

            if( !strcmp(v + 6, "Always") )
                MPH = 3;
            }
        }
    if( !memcmp(string, "Analysis Mode", 13) )
        {
        v = strstr(string, "value");

        if( v )
            {
            if( !strcmp(v + 6, "false") )
                ANALYSIS_MODE = false;

            if( !strcmp(v + 6, "true") )
                ANALYSIS_MODE = true;
            }
        }
    if( !memcmp(string, "Extra Info", 10) )
        {
        v = strstr(string, "value");

        if( v )
            {
            if( !strcmp(v + 6, "false") )
                EXTRA_INFO = false;

            if( !strcmp(v + 6, "true") )
                EXTRA_INFO = true;
            }
        }
    if( !memcmp(string, "Multi History", 13) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( !strcmp(v + 6, "false") )
                MULTI_HISTORY = false;

            if( !strcmp(v + 6, "true") )
                MULTI_HISTORY = true;
            }
        }
    if( !memcmp(string, "Multi Positional Gain", 21) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( !strcmp(v + 6, "false") )
                MULTI_POS_GAIN = false;

            if( !strcmp(v + 6, "true") )
                MULTI_POS_GAIN = true;
            }
        }
    if( !memcmp(string, "NMR Smooth Scaling", 18) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( !strcmp(v + 6, "false") )
                NMR_SCALING = false;

            if( !strcmp(v + 6, "true") )
                NMR_SCALING = true;
            }
        }
    if( !memcmp(string, "Null Move Verification", 22) )
        {
        v = strstr(string, "value");

        if( v )
            {
            if( !strcmp(v + 6, "false") )
                NULL_MOVE_VERIFICATION = false;

            if( !strcmp(v + 6, "true") )
                NULL_MOVE_VERIFICATION = true;
            }
        }
    if( !memcmp(string, "Ponder", 6) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( !strcmp(v + 6, "false") )
                DO_PONDER = false;

            if( !strcmp(v + 6, "true") )
                DO_PONDER = true;
            }
        }
    if( !memcmp(string, "Create FEN", 10) )
        {
        EmitFen(POSITION, STRING1);
        SEND("info string %s\n", STRING1);
        }
    if( !memcmp(string, "Clear Hash", 10) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( !strcmp(v + 6, "true") )
                HashClear();
            }
        }
    }
static void uci()
    {
    SEND("id name %s %s\n", NAME, VERSION);
    SEND("id author Kranium and Sentinel, based on IppoLit\n");
    SEND("option name Hash type spin min 1 max 65536 default 128\n");
    SEND("option name Pawn Hash type spin min 1 max 1024 default 32\n");
    SEND("option name Max Threads type spin min 1 max %d default %d\n", MAX_THREADS, MAX_THREADS);
    SEND("option name MultiPV type spin min 1 max 16 default 1\n");
    SEND("option name Pawn Value type spin min 0 max 200 default 100\n");
    SEND("option name Knight Value type spin min 0 max 650 default 325\n");
    SEND("option name Bishop Value type spin min 0 max 650 default 325\n");
    SEND("option name Rook Value type spin min 0 max 1000 default 500\n");
    SEND("option name Queen Value type spin min 0 max 1950 default 975\n");
    SEND("option name Bishop Pair Value type spin min 0 max 100 default 50\n");
    SEND("option name Verification Reduction type spin min 0 max 10 default 7\n");
    SEND("option name Move on Ponderhit type combo var Never var Sometimes var Always default Sometimes\n");
    SEND("option name Analysis Mode type check default false\n");
    SEND("option name Extra Info type check default false\n");
    SEND("option name Multi History type check default true\n");
    SEND("option name Multi Positional Gain type check default true\n");
    SEND("option name NMR Smooth Scaling type check default false\n");
    SEND("option name Null Move Verification type check default true\n");
    SEND("option name Ponder type check default false\n");
    SEND("option name Create FEN type button default false\n");
    SEND("option name Clear Hash type button default false\n");
    SEND("uciok\n");
    }
static void readyok()
    {
    SEND("readyok\n");
    }
static void quit()
    {
    exit(0);
    }
static void ParseInput( typePOS *POSITION, char *I )
    {
    if( !strcmp(I, "quit") )
        quit();

    if( !strcmp(I, "stop") )
        HaltSearch(0);

    if( !strcmp(I, "isready") )
        readyok();

    if( !strcmp(I, "ponderhit") )
        ponderhit();

    if( !strcmp(I, "ucinewgame") )
        {
        HaltSearch(0);
        NewGame(POSITION, true);
        }

    if( JUMP_IS_SET )
        return;

#if defined (UTILITIES)
    if( !strcmp(I, "benchmark") )
        BenchMark(POSITION, "go movetime 1000");

    if( !memcmp(I, "benchmark", 9) )
        BenchMark(POSITION, I + 10);
#endif

    if( !memcmp(I, "go", 2) )
        {
        InitSearch(POSITION, I);

        if( BOARD_IS_OK )
            Search(POSITION);
        }

    if( !memcmp(I, "position", 8) )
        InitPosition(POSITION, I + 9);

    if( !memcmp(I, "setoption name", 14) )
        SetOption(POSITION, I + 15);

    if( !strcmp(I, "uci") )
        uci();
    }

void Input( typePOS *POSITION )
    {
    char I[65536];
    fgets(I, 65536, stdin);
    I[strlen(I) - 1] = 0;
    ParseInput(POSITION, I);
    }
