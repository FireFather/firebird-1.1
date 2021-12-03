int InitPawnHash( int );
void InitArrays();
void InitCaptureValues();

void HaltSearch( int );
void CheckDone( typePOS *, int );
void InitSearch( typePOS *, char * );
void Info( sint64 );

void Eval( typePOS *, int, int, int );
void Mobility( typePOS * );
void EvalHashClear();

typeMoveList *CaptureMoves( typePOS *, typeMoveList *, uint64 );
typeMoveList *OrdinaryMoves( typePOS *, typeMoveList * );
typeMoveList *EvasionMoves( typePOS *, typeMoveList *, uint64 );
void InitCaptureValues();

typeMoveList *WhiteCaptures( typePOS *, typeMoveList *, uint64 );
typeMoveList *BlackCaptures( typePOS *, typeMoveList *, uint64 );
typeMoveList *WhiteOrdinary( typePOS *, typeMoveList * );
typeMoveList *BlackOrdinary( typePOS *, typeMoveList * );
void SortOrdinary( typeMoveList *, typeMoveList *, uint32, uint32, uint32 );

typeMoveList *QuietChecksWhite( typePOS *, typeMoveList *, uint64 );
typeMoveList *QuietChecksBlack( typePOS *, typeMoveList *, uint64 );

typeMoveList *PositionalMovesWhite( typePOS *, typeMoveList *, int );
typeMoveList *PositionalMovesBlack( typePOS *, typeMoveList *, int );
typeMoveList *BlackEvasions( typePOS *, typeMoveList *, uint64 );
typeMoveList *WhiteEvasions( typePOS *, typeMoveList *, uint64 );

void IncrementAge();
void HashClear();
int InitHash( int );

void HashLowerALL( typePOS *, int, int, int );
void HashUpperCUT( typePOS *, int, int );
void HashLower( uint64, int, int, int );
void HashUpper( uint64, int, int );
void HashExact( typePOS *, int, int, int, int );

void Input( typePOS * );

void ResetHistory();
void ResetPositionalGain();

void Make( typePOS *, uint32 );
void Undo( typePOS *, uint32 );
void MakeWhite( typePOS *, uint32 );
void UndoWhite( typePOS *, uint32 );
void MakeBlack( typePOS *, uint32 );
void UndoBlack( typePOS *, uint32 );

void InitMaterialValue();

uint32 NextWhite( typePOS *, typeNEXT * );
uint32 NextBlack( typePOS *, typeNEXT * );

boolean WhiteOK( typePOS *, uint32 );
boolean BlackOK( typePOS *, uint32 );

void InitPawns();
void PawnEval( typePOS *, typePawnEval * );

int PVQsearchWhite( typePOS *, int, int, int );
int PVQsearchWhiteCheck( typePOS *, int, int, int );
int PVQsearchBlack( typePOS *, int, int, int );
int PVQsearchBlackCheck( typePOS *, int, int, int );

void TopWhite( typePOS * );
void TopBlack( typePOS * );
int RootWhite( typePOS *, int, int, int );
int RootBlack( typePOS *, int, int, int );

int PVNodeWhite( typePOS *, int, int, int, int );
int PVNodeBlack( typePOS *, int, int, int, int );

int ExcludeWhite( typePOS *, int, int, uint32 );
int ExcludeWhiteCheck( typePOS *, int, int, uint32 );
int ExcludeBlack( typePOS *, int, int, uint32 );
int ExcludeBlackCheck( typePOS *, int, int, uint32 );

int CutNodeWhite( typePOS *, int, int );
int CutNodeBlack( typePOS *, int, int );
int CutNodeWhiteCheck( typePOS *, int, int );
int CutNodeBlackCheck( typePOS *, int, int );

int AllNodeWhite( typePOS *, int, int );
int AllNodeBlack( typePOS *, int, int );
int AllNodeWhiteCheck( typePOS *, int, int );
int AllNodeBlackCheck( typePOS *, int, int );

int LowDepthWhite( typePOS *, int, int );
int LowDepthBlack( typePOS *, int, int );
int LowDepthWhiteCheck( typePOS *, int, int );
int LowDepthBlackCheck( typePOS *, int, int );

int QsearchWhite( typePOS *, int, int );
int QsearchBlack( typePOS *, int, int );
int QsearchWhiteCheck( typePOS *, int, int );
int QsearchBlackCheck( typePOS *, int, int );

void OutputBestMove();
void Search( typePOS * );
void Information( typePOS *, sint64, int, int, int );

boolean WhiteSEE( typePOS *, uint32 );
boolean BlackSEE( typePOS *, uint32 );

char *ReadFEN( typePOS *, char * );
void InitPosition( typePOS *, char * );

void InitStatic();

void ERROR_END( char *, ... );
void FEN_ERROR( char *, ... );
void SEND( char *, ... );
char *Notate( uint32, char * );
uint64 GetClock();
uint64 ProcessClock();
void InitBitboards( typePOS * );
void NewGame( typePOS *, boolean );
boolean TryInput();

boolean IVAN_SPLIT( typePOS *, typeNEXT *, int, int, int, int, int * );
void ivan_fail_high( SPLITPOINT *, typePOS *, uint32 );

void WhitePVNodeSMP( typePOS * );
void BlackPVNodeSMP( typePOS * );
void WhiteAllSMP( typePOS * );
void BlackAllSMP( typePOS * );
void WhiteCutSMP( typePOS * );
void BlackCutSMP( typePOS * );
void ivan_init_smp();
void ivan_end_smp();
void rp_init();
void PawnHashReset();
void ponderhit();
void ShowBanner();
void GetSysInfo();

void WhiteTopAnalysis( typePOS * );
void BlackTopAnalysis( typePOS * );
int WhiteAnalysis( typePOS *, int, int, int );
int BlackAnalysis( typePOS *, int, int, int );

int WhiteMultiPV( typePOS *, int );
int BlackMultiPV( typePOS *, int );

void BenchMark( typePOS *, char * );

char *EmitFen( typePOS *, char * );

