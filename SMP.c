#include "firebird.h"
#define RPperCPU 8
int thread_arg[MaxCPUs];

void ThreadInit( int cpu )
    {
    DoLocked(init_threads++);

    while( init_threads < NumThreads );

    WaitForSingleObject(Wakeup[cpu], INFINITE);
    
	Lock(SMP);
    active_threads++;
    if( active_threads == NumThreads )
        SetEvent(Wakeup[0]);
    UnLock(SMP);

    ThreadStall(NullParent, cpu);
    }
void EndSMP()
    {
    int cpu;
    int rp;
    int sp;
    int cause;

    for ( cpu = 1; cpu < NumThreads; cpu++ )
        {
        PThreadExit[cpu] = true;
        SetEvent(WaitEvent[cpu]);
        }

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        for ( rp = 0; rp < RPperCPU; rp++ )
            RootPosition[cpu][rp].stop = true;

    if( active_threads > 1 )
        {
        cause = WaitForSingleObject(Wakeup[0], 1000);

        if( cause == WAIT_TIMEOUT )
		    {
            for ( cpu = 1; cpu < NumThreads; cpu++ )
                {
                ResetEvent(Wakeup[cpu]);
                SetEvent(WaitEvent[cpu]);
                }
            WaitForSingleObject(Wakeup[0], 5000);

            if( active_threads > 1 )
                {
                for ( cpu = 1; cpu < NumThreads; cpu++ )
                    ResetEvent(WaitEvent[cpu]);
                }
            }
        }
    ResetEvent(Wakeup[0]);

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        for ( rp = 0; rp < RPperCPU; rp++ )
            RootPosition[cpu][rp].used = false;

    for ( sp = 0; sp < MaxSP; sp++ )
        RootSP[sp].active = false;
    }
	
DWORD WINAPI ivan_thread( LPVOID A )
    {
	ThreadInit(*(int *)A);
    return (DWORD)NULL;
    }

static boolean Init0 = false;
void InitSMP()
    {
    int cpu;
    int rp;
    int sp;
    typePos *RP00;
    int h;
    int cause;

    SMPfree = 0;

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        for ( rp = 0; rp < RPperCPU; rp++ )
            {
            RootPosition[cpu][rp].used = false;
            RootPosition[cpu][rp].nodes = 0;
            }
    for ( sp = 0; sp < MaxSP; sp++ )
        RootSP[sp].active = false;

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        {
        Working[cpu] = NULL;
        PThreadExit[cpu] = false;
        }
    NullParent->child_count = 123;

    if( !Init0 )
        {
        Init0 = true;

        for ( cpu = 1; cpu < NumThreads; cpu++ )
            {
            thread_arg[cpu] = cpu;
            PThreadCreate(&PThread[cpu], NULL, ivan_thread, &thread_arg[cpu]);
            }

        while( init_threads < NumThreads );

        for ( cpu = 1; cpu < NumThreads; cpu++ )
            SetEvent(Wakeup[cpu]);
        }
    else
        for ( cpu = 1; cpu < NumThreads; cpu++ )
            SetEvent(Wakeup[cpu]);

    RP00 = &RootPosition[0][0];
    RP00->used = true;
    RP00->stop = false;
    memcpy(RP00, RootPosition0, 356);
    memcpy(RP00->Root, RootPosition0->Root, 2 * sizeof(typePosition));
    RP00->Current = RP00->Root + 1;
#undef StackHeight
#undef Stack

    h = RootPosition0->StackHeight;
    memcpy(RP00->Stack, RootPosition0->Stack, h * sizeof(uint64));
    RP00->StackHeight = h;
    RP00->child_count = 0;
    RP00->parent = NULL;

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        RP00->children[cpu] = NULL;
    RP00->SplitPoint = NULL;
    Working[0] = RP00;

    if( active_threads < NumThreads )
        {
        cause = WaitForSingleObject(Wakeup[0], 1000);

        if( cause == WAIT_TIMEOUT )
		    {
            for ( cpu = 1; cpu < NumThreads; cpu++ )
                SetEvent(Wakeup[cpu]);
            WaitForSingleObject(Wakeup[0], 5000);
            }
        }
	for ( cpu = 0; cpu < NumThreads; cpu++ )
        ResetEvent(Wakeup[cpu]);
    }
static void SMPGoodHistory( typePos *Pos, uint32 m, SplitPoint *sp )
    {
    int sv = HistoryValue(Pos, m);
    HistoryValue(Pos, m) = sv + (((0xff00 - sv) * sp->depth) >> 8);
    }
void SMPFailHigh( SplitPoint *sp, typePos *Pos, uint32 m )
    {
    int cpu;
    Lock(sp->splock);

    if( sp->tot || Pos->stop )
        {
        UnLock(sp->splock);
        return;
        }
    sp->tot = true;
    sp->move = m;
    sp->value = sp->beta;
    UnLock(sp->splock);

    if( Pos->sq[To(m)] == 0 && MoveHistory(m) )
        SMPGoodHistory(Pos, m, sp);

    if( sp->node_type == NodeTypeAll )
        HashLowerAll(Pos, m, sp->depth, sp->beta);
    else
        HashLower(Pos->Current->Hash, m, sp->depth, sp->beta);
    Lock(SMP);
    Lock(Pos->parent->padlock);

    if( !Pos->stop )
        {
        for ( cpu = 0; cpu < NumThreads; cpu++ )
            if( Pos->parent->children[cpu] && cpu != Pos->cpu )
                ThreadHalt(Pos->parent->children[cpu]);
        }
    UnLock(Pos->parent->padlock);
    UnLock(SMP);
    }
static INLINE void SMPBadHistory( typePos *Pos, uint32 m, SplitPoint *sp )
    {
    if( (Pos->Current + 1)->cp == 0 && MoveHistory(m) )
        {
        int sv = HistoryValue(Pos, m);

        if( Pos->Current->Value > sp->alpha - 50 )
            HistoryValue(Pos, m) = sv - ((sv * sp->depth) >> 8);
        }
    }
static void SMPSearchCutNode( typePos *Pos )
    {
    SplitPoint *sp;
    sp = Pos->SplitPoint;
    Lock(sp->splock);
    sp->childs++;
    UnLock(sp->splock);
    Pos->wtm ? WhiteCutSMP(Pos) : BlackCutSMP(Pos);
    Lock(sp->splock);
    sp->childs--;

    if( !sp->tot && !sp->childs && !Pos->stop )
        {
        HashUpperCut(Pos, sp->depth, sp->value);
        }
    UnLock(sp->splock);
    }
static void SMPSearchAllNode( typePos *Pos )
    {
    SplitPoint *sp;
    sp = Pos->SplitPoint;
    Lock(sp->splock);
    sp->childs++;
    UnLock(sp->splock);
    Pos->wtm ? WhiteAllSMP(Pos) : BlackAllSMP(Pos);
    Lock(sp->splock);
    sp->childs--;

    if( !sp->tot && !sp->childs && !Pos->stop )
        {
        HashUpper(Pos->Current->Hash, sp->depth, sp->value);
        }
    UnLock(sp->splock);
    }
void SMPSearch( typePos *Pos )
    {
    SplitPoint *sp;
    sp = Pos->SplitPoint;

    if( sp->node_type == NodeTypeAll )
        {
        SMPSearchAllNode(Pos);
        return;
        }

    if( sp->node_type == NodeTypeCut )
        {
        SMPSearchCutNode(Pos);
        return;
        }
    Lock(sp->splock);
    sp->childs++;
    UnLock(sp->splock);
    Pos->wtm ? WhitePVNodeSMP(Pos) : BlackPVNodeSMP(Pos);
    Lock(sp->splock);
    sp->childs--;

    if( !sp->tot && !sp->childs && !Pos->stop )
        {
        uint32 m = sp->good_move;

        if( m )
            {
            HashExact(Pos, m, sp->depth, sp->value, FlagExact);

            if( Pos->sq[To(m)] == 0 && MoveHistory(m) )
                SMPGoodHistory(Pos, m, sp);
            }
        else
            HashUpper(Pos->Current->Hash, sp->depth, sp->value);
        }
    UnLock(sp->splock);
    }
static void CopyFromChild( typePos *Parent, typePos *Child )
    {
    if( Child->SplitPoint->value >= Child->SplitPoint->beta )
        Parent->Current->move = Child->SplitPoint->move;
    else
        Parent->Current->move = 0;
    }
void ThreadStall( typePos *Parent, int cpu )
    {
    typePos *W;

    while( true )
        {
        DoLocked(SMPfree++);

        while( !Working[cpu] && Parent->child_count && !PThreadExit[cpu] )
            {
            WaitForSingleObject(WaitEvent[cpu], INFINITE);
            }

        if( PThreadExit[cpu] )
            {
            Working[cpu] = NULL;
            Parent = NullParent;

            Lock(SMP);
            SMPfree--;
            active_threads--;
            if( active_threads == 1 )
                SetEvent(Wakeup[0]);
            UnLock(SMP);

            WaitForSingleObject(Wakeup[cpu], INFINITE);
            PThreadExit[cpu] = false;
            Lock(SMP);
            active_threads++;

            if( active_threads == NumThreads )
                SetEvent(Wakeup[0]);
            else
                ResetEvent(Wakeup[0]);
            UnLock(SMP);
            continue;
            }

        Lock(SMP);
        SMPfree--;
        W = Working[cpu];

        if( !W )
            {
            Working[cpu] = Parent;
            SetEvent(WaitEvent[cpu]);
            UnLock(SMP);
            return;
            }
        UnLock(SMP);

        SMPSearch(W);
        Lock(SMP);
        Lock(W->parent->padlock);
        CopyFromChild(W->parent, W);
        W->parent->child_count--;

        if( W->parent->child_count == 0 )
            {
            int icpu = W->parent->cpu;
            SetEvent(WaitEvent[icpu]);
            }

        W->parent->children[cpu] = NULL;
        UnLock(W->parent->padlock);
        Working[cpu] = NULL;
        W->used = false;
        UnLock(SMP);
        }
    }
void ThreadHalt( typePos *Pos )
    {
    int n;
    Lock(Pos->padlock);
    Pos->stop = true;

    for ( n = 0; n < NumThreads; n++ )
        {
        if( Pos->children[n] != NULL )
            ThreadHalt(Pos->children[n]);
        }
    UnLock(Pos->padlock);
    }
static void SPInit()
    {
    int sp;

    for ( sp = 0; sp < MaxSP; sp++ )
        {
        RootSP[sp].active = false;
        InitLock(RootSP[sp].splock);
        }
    }
void RPInit()
    {
    int cpu;
    int rp;
    Init0 = false;
    InitLock(SMP);

    for ( cpu = 0; cpu < MaxCPUs; cpu++ )
        for ( rp = 0; rp < RPperCPU; rp++ )
            {
            memset((void *) &RootPosition[cpu][rp], 0, sizeof(typePos));
            RootPosition[cpu][rp].Root = malloc(MaxPly * sizeof(typePosition));
            RootPosition[cpu][rp].used = false;
            RootPosition[cpu][rp].parent = NULL;
            RootPosition[cpu][rp].Current = RootPosition[cpu][rp].Root;
            RootPosition[cpu][rp].cpu = cpu;
            InitLock(RootPosition[cpu][rp].padlock);
            }

    for ( cpu = 0; cpu < MaxCPUs; cpu++ )
        Wakeup[cpu] = CreateEvent(0, false, false, 0);

    for ( cpu = 0; cpu < MaxCPUs; cpu++ )
        WaitEvent[cpu] = CreateEvent(0, false, false, 0);
    init_threads = 1;
    active_threads = 1;
    SPInit();
    }
static typePos *GetPosition( int cpu )
    {
    int u;

    for ( u = 0; u < RPperCPU; u++ )
        if( !RootPosition[cpu][u].used )
            break;

    if( u == RPperCPU )
        return NULL;
    RootPosition[cpu][u].used = true;
    RootPosition[cpu][u].stop = false;
    return &RootPosition[cpu][u];
    }
static typePos *CopyToChild( int icpu, typePos *Parent )
    {
    typePos *Child;
    int cpu;
    int h;
    Child = GetPosition(icpu);

    if( !Child )
        return NULL;

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        Child->children[cpu] = NULL;
    memcpy(Child, Parent, 356);
    memcpy(Child->Root, Parent->Current - 1, 2 * sizeof(typePosition));
    Child->Current = Child->Root + 1;
#undef StackHeight
#undef Stack

    h = Parent->StackHeight;
    memcpy(Child->Stack, Parent->Stack, h * sizeof(uint64));
    Child->StackHeight = h;
    return Child;
    }
static void EndSplitPoint( SplitPoint *sp )
    {
    sp->active = false;
    }
static SplitPoint *new_splitpoint()
    {
    int sp;

    for ( sp = 0; sp < MaxSP; sp++ )
        if( !RootSP[sp].active )
            return &RootSP[sp];
    return NULL;
    }

boolean SMPSplit( typePos *Position, typeNext *NextMove,
	int depth, int beta, int alpha, int NodeType, int *r )
    {
    int cpu;
    int split;
    typePos *Child;
    SplitPoint *sp;

    Lock(SMP);

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        if( !Working[cpu] )
            break;

    if( Position->stop || cpu == NumThreads )
        {
        UnLock(SMP);
        return false;
        }
    Working[Position->cpu] = NULL;
    Position->child_count = 0;
    sp = new_splitpoint();

    if( sp == NULL )
        {
        Working[Position->cpu] = Position;
        UnLock(SMP);
        return false;
        }
    Lock(sp->splock);
    sp->alpha = alpha;
    sp->beta = beta;
    sp->depth = depth;
    sp->node_type = NodeType;

    if( NodeType != NodeTypePV )
        sp->value = sp->beta - 1;
    else
        sp->value = sp->alpha;
    sp->move = MoveNone;
    sp->good_move = MoveNone;
    sp->childs = 0;
    sp->MovePick = NextMove;
    sp->tot = false;
    sp->active = true;
    UnLock(sp->splock);
    split = 0;

    for ( cpu = 0; cpu < NumThreads && split < MaxSplit; cpu++ )
        {
        Position->children[cpu] = NULL;

        if( Working[cpu] == NULL )
            {
            Child = CopyToChild(cpu, Position);

            if( !Child )
                continue;

            split++;
            Position->children[cpu] = Child;
            Child->cpu = cpu;
            Child->parent = Position;
            Child->stop = false;
            Child->SplitPoint = sp;
            Position->child_count++;
            }
        }

    if( split == 0 )
        {
        Working[Position->cpu] = Position;
        Lock(sp->splock);
        EndSplitPoint(sp);
        UnLock(sp->splock);
        UnLock(SMP);
        return false;
        }

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        {
        if( Position->children[cpu] )
            {
            Working[cpu] = Position->children[cpu];
            SetEvent(WaitEvent[cpu]);
            }
        }
    UnLock(SMP);
    ThreadStall(Position, Position->cpu);
    Lock(SMP);
    Lock(sp->splock);
    *r = sp->value;
    EndSplitPoint(sp);
    UnLock(sp->splock);
    UnLock(SMP);
    return true;
    }
