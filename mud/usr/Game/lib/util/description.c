# include <game/thing.h>

static string list_strings(string *arr)
{
    int size;

    size = sizeof(arr);
    switch (size) {
    case 0:
        return "";

    case 1:
        return arr[0];

    case 2:
        return arr[0] + " and " + arr[1];

    default:
        return implode(arr[.. size - 2], ", ") + ", and " + arr[size - 1];
    }
}

static string describe_exits(string *dirs)
{
    switch (sizeof(dirs)) {
    case 0:
        return "There are no obvious exits.";

    case 1:
        return "The only obvious exit is " + dirs[0] + ".";

    default:
        return "The obvious exits are " + list_strings(dirs) + ".";
    }
}

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
    string desc;

    desc = "You notice nothing special.";
    if (obj <- LIB_ROOM) {
        desc += " " + describe_exits(obj->query_exits());
    }
    return desc;
}
