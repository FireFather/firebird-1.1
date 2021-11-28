
static INLINE void MakeNull( typePos *Position )
    {
    Position->nodes++;
    Position->Current->SavedFlags = Position->Current->flags;
    memcpy(Position->Current + 1, Position->Current, 64);
    Position->Current++;
    Position->Current->Hash ^= ZobristWTM;
    Position->wtm ^= 1;
    Position->height++;
    Position->Current->reversible++;

    if( Position->Current->ep )
        {
        Position->Current->Hash ^= ZobristEP[Position->Current->ep & 7];
        Position->Current->ep = 0;
        }
    Position->Current->Value = -((Position->Current - 1)->Value + TempoValue);
    Position->Current->PositionalValue = (Position->Current - 1)->PositionalValue;
    Position->Current->lazy = (Position->Current - 1)->lazy;
    Position->Current->flags &= ~3;
    Position->Current->move = 0;
    Position->Stack[++(Position->StackHeight)] = Position->Current->Hash;
    }
static INLINE void UndoNull( typePos *Position )
    {
    Position->Current--;
    Position->StackHeight--;
    Position->height--;
    Position->wtm ^= 1;
    Position->Current->flags = Position->Current->SavedFlags;
    }


