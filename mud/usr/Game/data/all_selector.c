# include <game/selector.h>
# include <game/thing.h>

inherit LIB_SELECTOR;

static void create() { }

object LIB_THING *select(object LIB_THING *objs,
                         varargs object LIB_CREATURE actor)
{
    return objs;
}
