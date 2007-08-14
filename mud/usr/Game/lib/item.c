# include <game/thing.h>

inherit LIB_THING;

int move(object destination)
{
    if (destination
        && (!(destination <- LIB_ROOM) && !(destination <- LIB_CREATURE)
            && !(destination <- LIB_CONTAINER)
            || !destination->allow_move(this_object())))
    {
        return FALSE;
    }
    move_object(destination);
    return TRUE;
}
