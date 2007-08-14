# include <game/attribute.h>
# include <game/thing.h>

inherit aff   LIB_ATTRIBUTE_AFFECTOR;
inherit item  LIB_ITEM;

static void create()
{
    item::create();
    aff::create();
}

object LIB_CREATURE query_wielder()
{
    object LIB_THING environment;

    environment = environment(this_object());
    return environment && environment <- LIB_CREATURE
            && sizeof(environment->query_wielded() & ({ this_object() }))
        ? environment : nil;
}

int move(object destination)
{
    return query_wielder() ? FALSE : ::move(destination);
}
