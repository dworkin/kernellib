# include <game/thing.h>

inherit LIB_ITEM;

object LIB_CREATURE query_wielder()
{
    object LIB_THING environment;

    environment = environment(this_object());
    return environment && environment <- LIB_CREATURE
            && sizeof(environment->query_wielded() & ({ this_object() }))
        ? environment : nil;
}

int allow_move(object destination)
{
    return query_wielder() ? FALSE : ::allow_move(destination);
}
