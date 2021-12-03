#include "firebird.h"
#define DO_LOCKED(x) { LOCK (SMP_IVAN); (x); UNLOCK (SMP_IVAN); }
#define MAX_SPLIT 8

volatile int SMP_FREE;
typePOS* volatile WORKING[MAX_CPUS];
boolean volatile PTHREAD_EXIT[MAX_CPUS];
void thread_halt (typePOS *POS);
boolean INIT[MAX_CPUS];

typedef struct
{
  int cpu;
}
t_args;

t_args ARGS[MAX_CPUS];

volatile int init_threads;
volatile boolean IVAN_END_SMP;
jmp_buf DEC_JMP[MAX_CPUS]; /* Decembrist bugfix */
volatile int active_threads;

void thread_stall (typePOS *PARENT, int cpu);
void thread_init (int cpu)
{
  int z;
  DO_LOCKED (init_threads++);
  while (init_threads < NUM_THREADS)
    {
    }
  z = setjmp (DEC_JMP[cpu]);
  if (z > 0)
    DO_LOCKED (active_threads--);
  while (IVAN_END_SMP)
    {
    }
  WAIT_CON_LOCK (WAKEUP[cpu], WAKEUP_LOCK[cpu]);
  DO_LOCKED (active_threads++);
  PTHREAD_EXIT[cpu] = false;
  thread_stall (NULL_PARENT, cpu);
}

void ivan_end_smp()
{
  int cpu;
  int rp;
  int sp;

  IVAN_END_SMP = true;
  LOCK (SMP_IVAN);
  for (cpu = 1; cpu < NUM_THREADS; cpu++)
    PTHREAD_EXIT[cpu] = true;
  UNLOCK (SMP_IVAN);
  for (cpu = 1; cpu < NUM_THREADS; cpu++)
    {
      PTHREAD_EXIT[cpu] = true;
      SIGNAL_CON_LOCK (WAIT_EVENT[cpu], PTHREAD_COND_MUTEX[cpu]);
    }
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    for (rp = 0; rp < RP_PER_CPU ; rp++)
      ROOT_POSITION[cpu][rp].stop = true;
  while (active_threads > 1)
    {
      for (cpu = 0; cpu < NUM_THREADS; cpu++)
	for (rp = 0; rp < RP_PER_CPU ; rp++)
	  ROOT_POSITION[cpu][rp].stop = true;
    }
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    for (rp = 0; rp < RP_PER_CPU ; rp++)
      ROOT_POSITION[cpu][rp].used = false;
  for (sp = 0; sp < MAX_SP; sp++)
    ROOT_SP[sp].active = false;
  IVAN_END_SMP = false;
}

#ifdef WINDOWS
#define VOID_STAR_TYPE DWORD
#else
#define VOID_STAR_TYPE void*
#endif

IVAN_THREAD(A)
{
  t_args *AttB;
  AttB = (t_args*) A;
  INIT[AttB->cpu] = true;
  thread_init (AttB->cpu);
  return (VOID_STAR_TYPE) NULL;
}

static boolean INIT0 = false;
void ivan_init_smp()
{
  int cpu;
  int rp;
  int sp;
  typePOS *RP00;
  int h;

  SMP_FREE = 0;
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    for (rp = 0; rp < RP_PER_CPU ; rp++)
      {
	ROOT_POSITION[cpu][rp].used = false;
	ROOT_POSITION[cpu][rp].nodes = 0;
	ROOT_POSITION[cpu][rp].tbhits = 0;
      }
  for (sp = 0; sp < MAX_SP; sp++)
    ROOT_SP[sp].active = false;
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    {
      WORKING[cpu] = NULL;
      PTHREAD_EXIT[cpu] = false;
    }
  NULL_PARENT->child_count = 123;
  if (!INIT0)
    {
      for (cpu = 1; cpu < NUM_THREADS; cpu++)
	{
	  ARGS[cpu].cpu = cpu;
	  PTHREAD_CREATE (&PTHREAD[cpu], NULL, ivan_thread, &ARGS[cpu]);
	}
      while (init_threads < NUM_THREADS)
	{
	}
#ifndef WINDOWS
      {
	volatile int c;
	for (c = 0; c < 1000000; c++)
	  {
	  }
      }
#endif
      for (cpu = 1; cpu < NUM_THREADS; cpu++)
	{
	  SIGNAL_CON_LOCK (WAKEUP[cpu], WAKEUP_LOCK[cpu]);
	}
    }
  else for (cpu = 1; cpu < NUM_THREADS; cpu++)
    {
      SIGNAL_CON_LOCK (WAKEUP[cpu], WAKEUP_LOCK[cpu]);
    }
  while (active_threads < NUM_THREADS)
    {
#ifndef WINDOWS
      if (!INIT0)
	{
	  for (cpu = 1; cpu < NUM_THREADS; cpu++)
	    {
	      SIGNAL_CON_LOCK (WAKEUP[cpu], WAKEUP_LOCK[cpu]);
	    }
	  volatile int c;
	  for (c = 0; c < 1000000; c++)
	    {
	    }
	}
#endif
    }
  INIT0 = true;
  RP00 = &ROOT_POSITION[0][0];
  RP00->used = true;
  RP00->stop = false;
  memcpy (RP00, ROOT_POSITION0, 356);
  memcpy (RP00->DYN_ROOT, ROOT_POSITION0->DYN_ROOT, 2 * sizeof (typeDYNAMIC));
  RP00->DYN = RP00->DYN_ROOT + 1;
  h = ROOT_POSITION0->StackHeight;
  memcpy (RP00->STACK, ROOT_POSITION0->STACK, h * sizeof (uint64));
  RP00->StackHeight = h;
  RP00->child_count = 0;
  RP00->parent = NULL;
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    RP00->children[cpu] = NULL;
  RP00->SplitPoint = NULL;
  WORKING[0] = RP00;
}

static void SMP_GOOD_HISTORY (typePOS *POS, uint32 m, SPLITPOINT *sp)
{
  int sv = HISTORY_VALUE (POS, m);
  HISTORY_VALUE (POS, m) = sv + (( (0xff00 - sv) * sp->depth) >> 8);
}

void ivan_fail_high (SPLITPOINT *sp, typePOS *POS, uint32 m)
{
  int cpu;
  LOCK (sp->splock);
  if (sp->tot || POS->stop)
    {
      UNLOCK (sp->splock);
      return;
    }
  sp->tot = true;
  sp->move = m;
  sp->value = sp->beta;
  UNLOCK (sp->splock);
  if (POS->sq[TO (m)] == 0 && MoveHistory (m))
    SMP_GOOD_HISTORY (POS, m, sp);
  if (sp->node_type == NODE_TYPE_ALL)
    HashLowerALL (POS, m, sp->depth, sp->beta);
  else
    HashLower (POS->DYN->HASH, m, sp->depth, sp->beta);
  LOCK (SMP_IVAN);
  LOCK (POS->parent->padlock);
  if (!POS->stop)
    {
      for (cpu = 0; cpu < NUM_THREADS; cpu++)
	if (POS->parent->children[cpu] && cpu != POS->cpu)
	  thread_halt (POS->parent->children[cpu]);
    }
  UNLOCK (POS->parent->padlock);
  UNLOCK (SMP_IVAN);
}

static INLINE void SMP_BAD_HISTORY (typePOS *POS, uint32 m, SPLITPOINT *sp)
{
  if ((POS->DYN + 1)->cp == 0 && MoveHistory (m))
    {
      int sv = HISTORY_VALUE (POS, m);
      if (POS->DYN->Value > sp->alpha - 50)
	HISTORY_VALUE (POS, m) = sv - ((sv * sp->depth) >> 8);
    }
}

static void ivan_search_cut_node (typePOS *POS)
{
  SPLITPOINT *sp;
  sp = POS->SplitPoint;
  LOCK (sp->splock);
  sp->childs++;
  UNLOCK (sp->splock);
  POS->wtm ? WhiteCutSMP (POS) : BlackCutSMP (POS);
  LOCK (sp->splock);
  sp->childs--;
  if (!sp->tot && !sp->childs && !POS->stop)
    {
      HashUpperCUT (POS, sp->depth, sp->value);
    }
  UNLOCK (sp->splock);
}

static void ivan_search_all_node (typePOS *POS)
{
  SPLITPOINT *sp;
  sp = POS->SplitPoint;
  LOCK (sp->splock);
  sp->childs++;
  UNLOCK (sp->splock);
  POS->wtm ? WhiteAllSMP (POS) : BlackAllSMP (POS);
  LOCK (sp->splock);
  sp->childs--;
  if (!sp->tot && !sp->childs && !POS->stop)
    {
      HashUpper (POS->DYN->HASH, sp->depth, sp->value);
    }
  UNLOCK (sp->splock);
}


void ivan_search (typePOS *POS)
{
  SPLITPOINT *sp;
  sp = POS->SplitPoint;
  if (sp->node_type == NODE_TYPE_ALL)
    {
      ivan_search_all_node (POS);
      return;
    }
  if (sp->node_type == NODE_TYPE_CUT)
    {
      ivan_search_cut_node (POS);
      return;
    }
  LOCK (sp->splock);
  sp->childs++;
  UNLOCK (sp->splock);
  POS->wtm ? WhitePVNodeSMP (POS) : BlackPVNodeSMP (POS);
  LOCK (sp->splock);
  sp->childs--;
  if (!sp->tot && !sp->childs && !POS->stop)
    {
      uint32 m = sp->good_move;
      if (m)
	{
	  HashExact (POS, m, sp->depth, sp->value, FLAG_EXACT);
	  if (POS->sq[TO (m)] == 0 && MoveHistory (m))
	    SMP_GOOD_HISTORY (POS, m, sp);
	}
      else
	  HashUpper (POS->DYN->HASH, sp->depth, sp->value);
    }
  UNLOCK (sp->splock);
}

static void COPY_FROM_CHILD (typePOS *PARENT, typePOS *CHILD)
{
  if (CHILD->SplitPoint->value >= CHILD->SplitPoint->beta)
    PARENT->DYN->move = CHILD->SplitPoint->move;
  else
    PARENT->DYN->move = 0;
}

void thread_stall (typePOS *PARENT, int cpu)
{
  typePOS *W;
  while (true)
    {
      DO_LOCKED (SMP_FREE++);
      while (!WORKING[cpu] && PARENT->child_count && !PTHREAD_EXIT[cpu])
	{
#ifdef WINDOWS
          WAIT_CON_LOCK (WAIT_EVENT[cpu], PTHREAD_COND_MUTEX[cpu]);
#else
          LOCK (&PTHREAD_COND_MUTEX[cpu]);
          if (WORKING[cpu] || !PARENT->child_count || PTHREAD_EXIT[cpu])
            {
              UNLOCK (&PTHREAD_COND_MUTEX[cpu]);
              break; // doble wakeup ?
            }
          WAIT (&WAIT_EVENT[cpu], &PTHREAD_COND_MUTEX[cpu]);
          UNLOCK (&PTHREAD_COND_MUTEX[cpu]);
#endif
	}
      if (PTHREAD_EXIT[cpu])
	{
	  WORKING[cpu] = NULL;
	  PARENT = NULL_PARENT;
	  longjmp (DEC_JMP[cpu], 1);
	}
      LOCK (SMP_IVAN);
      SMP_FREE--;
      W = WORKING[cpu];
      if (!W)
	{
	  WORKING[cpu] = PARENT;
	  SIGNAL_CON_LOCK (WAIT_EVENT[cpu], PTHREAD_COND_MUTEX[cpu]);
	  UNLOCK (SMP_IVAN); 
	  return;
	}
      UNLOCK (SMP_IVAN);
      ivan_search (W);
      LOCK (SMP_IVAN);
      LOCK (W->parent->padlock);
      COPY_FROM_CHILD (W->parent, W);
      W->parent->child_count--;

      if (W->parent->child_count == 0)
	{
	  int icpu = W->parent->cpu;
	  SIGNAL_CON_LOCK (WAIT_EVENT[icpu], PTHREAD_COND_MUTEX[icpu]);
	}

      W->parent->children[cpu] = NULL;
      UNLOCK (W->parent->padlock);
      WORKING[cpu] = NULL;
      W->used = false;
      UNLOCK (SMP_IVAN);
    }
}

void thread_halt (typePOS *POS)
{
  int n;  
  LOCK (POS->padlock);
  POS->stop = true;
  for (n = 0; n < NUM_THREADS; n++)
    {
      if (POS->children[n] != NULL)
	thread_halt (POS->children[n]);
    }
  UNLOCK (POS->padlock);
}

static void sp_init()
{
  int sp;
  for (sp = 0; sp < MAX_SP; sp++)
    {
      ROOT_SP[sp].active = false;
      LOCK_INIT (ROOT_SP[sp].splock);
    }
}

void rp_init()
{
  int cpu;
  int rp;
  LOCK_INIT (SMP_IVAN);
  for (cpu = 0; cpu < MAX_CPUS; cpu++)
    for (rp = 0; rp < RP_PER_CPU ; rp++)
    {
      memset( (void*) &ROOT_POSITION[cpu][rp], 0, sizeof (typePOS));
      ROOT_POSITION[cpu][rp].DYN_ROOT =
	malloc (MAXIMUM_PLY * sizeof(typeDYNAMIC));
      ROOT_POSITION[cpu][rp].used =  false;
      ROOT_POSITION[cpu][rp].parent = NULL;
      ROOT_POSITION[cpu][rp].DYN = ROOT_POSITION[cpu][rp].DYN_ROOT;
      ROOT_POSITION[cpu][rp].cpu = cpu;
      LOCK_INIT (ROOT_POSITION[cpu][rp].padlock);
    }
  for (cpu = 0; cpu < MAX_CPUS; cpu++)
    COND_INIT (WAIT_EVENT[cpu], PTHREAD_COND_MUTEX[cpu]);
  for (cpu = 0; cpu < MAX_CPUS; cpu++)
    COND_INIT (WAKEUP[cpu], WAKEUP_LOCK[cpu]);
  for (cpu = 0; cpu < MAX_CPUS; cpu++)
    INIT[cpu] = false;
  init_threads = 1;
  active_threads = 1;
  sp_init();
}

static typePOS* GetPosition (int cpu)
{
  int u;
  for (u = 0; u < RP_PER_CPU; u++)
    if (!ROOT_POSITION[cpu][u].used)
      break;
  if (u == RP_PER_CPU)
    return NULL;
  ROOT_POSITION[cpu][u].used = true;
  ROOT_POSITION[cpu][u].stop = false;
  return &ROOT_POSITION[cpu][u];
}

static typePOS* COPY_TO_CHILD (int icpu, typePOS *PARENT)
{
  typePOS *CHILD;
  int cpu;
  int h;
  CHILD = GetPosition (icpu);
  if (!CHILD)
    return NULL;
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    CHILD->children[cpu] = NULL;
  memcpy (CHILD, PARENT, 356);
  memcpy (CHILD->DYN_ROOT, PARENT->DYN - 1, 2 * sizeof (typeDYNAMIC));
  CHILD->DYN = CHILD->DYN_ROOT + 1;
  h = PARENT->StackHeight;
  memcpy (CHILD->STACK, PARENT->STACK, h * sizeof (uint64));
  CHILD->StackHeight = h;
  return CHILD;
}

static void end_splitpunkt (SPLITPOINT *sp) /* LOCKed ? */
{
  sp->active = false;
}

static SPLITPOINT* new_splitpunkt()
{
  int sp;
  for (sp = 0; sp < MAX_SP; sp++)
    if (!ROOT_SP[sp].active)
      return &ROOT_SP[sp];
  return NULL;
}

boolean IVAN_SPLIT (typePOS *POSITION, typeNEXT *NextMove,
	int depth, int beta, int alpha, int NODE_TYPE, int *r)
{
  int cpu;
  int split;
  typePOS *CHILD;
  SPLITPOINT *sp;

  LOCK (SMP_IVAN);
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    if (!WORKING[cpu])
      break;
  if (POSITION->stop || cpu == NUM_THREADS)
    {
      UNLOCK (SMP_IVAN);
      return false;
    }
  WORKING[POSITION->cpu] = NULL;
  POSITION->child_count = 0;
  sp = new_splitpunkt();
  if (sp == NULL)
    {
      WORKING[POSITION->cpu] = POSITION;      
      UNLOCK (SMP_IVAN);
      return false;
    }
  LOCK (sp->splock); /* ? */
  sp->alpha = alpha;
  sp->beta = beta;
  sp->depth = depth;
  sp->node_type = NODE_TYPE;
  if (NODE_TYPE != NODE_TYPE_PV)
    sp->value = sp->beta - 1;
  else
    sp->value = sp->alpha;
  sp->move = MOVE_NONE;
  sp->good_move = MOVE_NONE;
  sp->childs = 0;
  sp->MOVE_PICK = NextMove;
  sp->tot = false;
  sp->active = true;
  UNLOCK (sp->splock); /* ? */
  split = 0;
  for (cpu = 0; cpu < NUM_THREADS && split < MAX_SPLIT; cpu++)
    {
      POSITION->children[cpu] = NULL;
      if (WORKING[cpu] == NULL)
	{
	  CHILD = COPY_TO_CHILD (cpu, POSITION);
	  if (!CHILD)
	    continue;
	  split++;
	  POSITION->children[cpu] = CHILD;
	  CHILD->cpu = cpu;
	  CHILD->parent = POSITION;
	  CHILD->stop = false;
	  CHILD->SplitPoint = sp;
	  POSITION->child_count++;
	}
    }
  if (split == 0)
    {
      WORKING[POSITION->cpu] = POSITION;
      LOCK (sp->splock);
      end_splitpunkt (sp);
      UNLOCK (sp->splock);
      UNLOCK (SMP_IVAN);
      return false;
    }
  /* cuando "split" es uno ? */
  for (cpu = 0; cpu < NUM_THREADS; cpu++)
    {
      if (POSITION->children[cpu])
	{
	  WORKING[cpu] = POSITION->children[cpu];
	  SIGNAL_CON_LOCK (WAIT_EVENT[cpu], PTHREAD_COND_MUTEX[cpu]);
	}
    }
  UNLOCK (SMP_IVAN);
  thread_stall (POSITION, POSITION->cpu);
  LOCK (SMP_IVAN);
  LOCK (sp->splock); /* ? */
  *r = sp->value;
  end_splitpunkt (sp);
  UNLOCK (sp->splock); /* ? */
  UNLOCK (SMP_IVAN);
  return true;
}

