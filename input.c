#include "firebird.h"

static void SetOption( typePos *Position, char *string )
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
    if( !memcmp(string, "Max Threads", 11) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( Value <= 0 )
                Value = 1;

            if( Value > MaxThreads )
                Value = MaxThreads;
            UCIMaxThreads = Value;
			InitSMP();
            }
        }
    if( !memcmp(string, "MultiPV", 7) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);
            MultiPV = Value;

            if( MultiPV <= 1 )
                MultiPV = 1;

            if( MultiPV >= 250 )
                MultiPV = 250;
            }
        }
    if( !memcmp(string, "Extra Info", 10) )
        {
        v = strstr(string, "value");

        if( v )
            {
            if( !strcmp(v + 6, "false") )
                ExtraInfo = false;

            if( !strcmp(v + 6, "true") )
                ExtraInfo = true;
            }
        }
    if( !memcmp(string, "Ponder", 6) )
        {
        v = strstr(string, "value");

        if( v )
            {
            Value = atoi(v + 6);

            if( !strcmp(v + 6, "false") )
                DoPonder = false;

            if( !strcmp(v + 6, "true") )
                DoPonder = true;
            }
        }
    }
static void uci()
    {
    Send("id name %s %s\n", NAME, VERSION);
    Send("id author Norman Schmidt\n");
    Send("option name Hash type spin min 1 max 65536 default 128\n");
    Send("option name Max Threads type spin min 1 max %d default %d\n", UCIMaxThreads, UCIMaxThreads);
    Send("option name MultiPV type spin min 1 max 16 default 1\n");
    Send("option name UCI Info type check default false\n");
    Send("option name Ponder type check default false\n");
    Send("uciok\n");
    }
static void readyok()
    {
    Send("readyok\n");
    }
static void quit()
    {
    exit(0);
    }
static void ParseInput( typePos *Position, char *I )
    {
    if( !strcmp(I, "quit") )
        quit();

    if( !strcmp(I, "stop") )
        HaltSearch();

    if( !strcmp(I, "isready") )
        readyok();

    if( !strcmp(I, "ponderhit") )
        ponderhit();

    if( !strcmp(I, "ucinewgame") )
        {
        HaltSearch();
        NewGame(Position, true);
        }

    if( JumpIsSet )
        return;

#ifdef BENCHMARK
    if( !strcmp(I, "benchmark") )
        BenchMark(Position, "go movetime 1000");

    if( !memcmp(I, "benchmark", 9) )
        BenchMark(Position, I + 10);
#endif

    if( !memcmp(I, "go", 2) )
        {
        InitSearch(Position, I);

        if( BoardIsOk )
            Search(Position);
        }

    if( !memcmp(I, "position", 8) )
        InitPosition(Position, I + 9);

    if( !memcmp(I, "setoption name", 14) )
        SetOption(Position, I + 15);

    if( !strcmp(I, "uci") )
        uci();
    }

void Input( typePos *Position )
    {
    char I[65536];
    fgets(I, 65536, stdin);
    I[strlen(I) - 1] = 0;
    ParseInput(Position, I);
    }
