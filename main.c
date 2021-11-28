#include "firebird.h"

void InitRootPosition()
    {
    memset((void *)RootPosition0, 0, sizeof(typePos));
    RootPosition0->Root = malloc(MaxPly * sizeof(typePosition));
    RootPosition0->Current = RootPosition0->Root + 1;
    RootPosition0->stop = false;
    }
static void InitVars()
    {
    AnalysisMode = false;
    BufferTime = 0;
    ExtraInfo = false;
    DoInfinite = false;
    DoPonder = false;
    DoSearchMoves = false;

    NumThreads = 1;
    MultiPosGain = true;
    MultiHistory = true;
    MPH = 2;
    MultiPV = 1;
    UCIMaxThreads = 16;
    PonderHit = false;
    Stop = false;

    PValue = 100;
    NValue = 300;
    BValue = 310;
    RValue = 500;
    QValue = 950;
    BPValue = 42;
    }

int main()
    {
    ShowBanner();
    InitVars();
    GetSysInfo();
    InitHash(128);
    ResetHistory();
    InitCaptureValues();
    InitArrays();
    InitPawns();
    InitMaterialValue();
    InitStatic();
	InitRootPosition();
	RPInit();
    NewGame(RootPosition0, true);
    while( 1 )
        Input(RootPosition0);
    return 0;
    }
