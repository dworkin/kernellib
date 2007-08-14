# include <game/thing.h>

inherit LIB_ITEM;

int allow_move(object obj)
{
    return obj <- LIB_ITEM;
}
