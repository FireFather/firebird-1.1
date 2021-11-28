int NumCPUs;

boolean AnalysisMode;
boolean BoardIsOk;
boolean DoInfinite;
boolean DoPonder;
boolean DoSearchMoves;
boolean ExtraInfo;
boolean MultiHistory;
boolean MultiPosGain;
boolean IsNewGame;
boolean PonderHit;
boolean Stop;
boolean UCIPonder;
boolean JumpIsSet;
boolean EasyMove;
boolean BadMove;
boolean BattleMove;
boolean Analysing;

char String1[64];
char String2[64];
char String3[64];
char String4[64];

int PValue;
int NValue;
int BValue;
int RValue;
int QValue;
int BPValue;

int MPH;
int MultiPV;
int UCIMaxThreads;
int PawnHashSize;

int RootScore;
int RootPrevious;
int RootDepth;
int PreviousDepth;
int PreviousFast;

uint32 RootBestMove;

uint64 BufferTime;
uint64 HashMask;
uint64 Age;
uint64 NodeCheck;
uint64 volatile NumThreads;
uint64 StartClock;

sint64 DesiredTime;
extern jmp_buf J;

sint64 LastMessage;
sint64 AbsoluteTime;
sint64 Increment;
sint64 BattleTime;
sint64 EasyTime;
sint64 OrdinaryTime;
sint64 total_wtime;
sint64 total_btime;
sint64 TotalTime;

int TotalMTG;
int MTGPrev;
int Depth;
boolean NewPonderHit;

static uint64 HashSize = 0x400000;
static boolean FlagHashInit = 0;

boolean Init[MaxCPUs];
boolean volatile PThreadExit[MaxCPUs];
volatile int SMPfree;
volatile int init_threads;
volatile boolean SMPEnded;
volatile int active_threads;

typePos* volatile Working[MaxCPUs];
int PreviousDepth;
int PreviousFast;