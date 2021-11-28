#include "firebird.h"
#ifdef BENCHMARK

void BenchMark( typePos *Position, char *GoString )
    {
    int i;
    char String[1024], GO_String[1024];
    int cpu, rp;
    uint64 Nodes, C, TIME;
    uint64 TotalTime = 0, TotalNodes = 0;

    for ( i = 0; i < 16; i++ )
        {
        sprintf(String, "%s %s\n", "fen", BenchmarkPositions[i]);
        InitPosition(Position, String);
        strcpy(GO_String, GoString);
        InitSearch(Position, GO_String);
        C = GetClock();
        Search(Position);
        TIME = GetClock() - C;
        Nodes = 0;

		for ( cpu = 0; cpu < NumThreads; cpu++ )
			for ( rp = 0; rp < RPperCPU; rp++ )
				Nodes += RootPosition[cpu][rp].nodes;
        Send("Position %d: Nodes: %lld Time: %lldms\n", 1 + i, Nodes, TIME / 1000);
        TotalNodes += Nodes;
        TotalTime += TIME;
        }
    Send("Total Nodes: %lld Time: %lldms\n", TotalNodes, TotalTime / 1000);
    Send("Total NPS: %lld\n", ((TotalNodes * 1000) / TotalTime) * 1000);
    }
#endif
