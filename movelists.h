
typeMoveList *CaptureMoves( typePos *, typeMoveList *, uint64 );
typeMoveList *OrdinaryMoves( typePos *, typeMoveList * );
typeMoveList *EvasionMoves( typePos *, typeMoveList *, uint64 );

typeMoveList *WhiteCaptures( typePos *, typeMoveList *, uint64 );
typeMoveList *BlackCaptures( typePos *, typeMoveList *, uint64 );

typeMoveList *WhiteOrdinary( typePos *, typeMoveList * );
typeMoveList *BlackOrdinary( typePos *, typeMoveList * );

typeMoveList *QuietChecksWhite( typePos *, typeMoveList *, uint64 );
typeMoveList *QuietChecksBlack( typePos *, typeMoveList *, uint64 );

typeMoveList *PositionalMovesWhite( typePos *, typeMoveList *, int );
typeMoveList *PositionalMovesBlack( typePos *, typeMoveList *, int );

typeMoveList *BlackEvasions( typePos *, typeMoveList *, uint64 );
typeMoveList *WhiteEvasions( typePos *, typeMoveList *, uint64 );