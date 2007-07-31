# include <game/thing.h>

inherit LIB_ITEM;

int allow_enter(object obj)
{
    return obj <- LIB_ITEM;
}
