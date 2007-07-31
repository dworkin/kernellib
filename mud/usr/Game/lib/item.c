# include <game/thing.h>

inherit LIB_THING;

int allow_move(object destination)
{
    return !destination || destination <- LIB_THING;
}
