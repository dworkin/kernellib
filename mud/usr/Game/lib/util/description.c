# include <game/thing.h>

static string definite_description(object LIB_THING obj,
                                   varargs object LIB_THING obs)
{
    if (obj <- LIB_CREATURE) {
        return "the creature";
    } else if (obj <- LIB_ITEM) {
        return "the item";
    } else if (obj <- LIB_ROOM) {
        return "the room";
    } else {
        return "the thing";
    }
}

static string indefinite_description(object LIB_THING obj,
                                     varargs object LIB_THING obs)
{
    if (obj <- LIB_CREATURE) {
        return "a creature";
    } else if (obj <- LIB_ITEM) {
        return "an item";
    } else if (obj <- LIB_ROOM) {
        return "a room";
    } else {
        return "a thing";
    }
}

static string verbose_description(object LIB_THING obj,
                                  varargs object LIB_THING obs)
{
    return "You do not notice anything special.";
}
