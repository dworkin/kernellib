# include <game/string.h>
# include <game/thing.h>

private inherit UTIL_STRING;

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

static string indefinite_description(object LIB_THING obj,
                                     varargs object LIB_THING observer)
{
    string desc;

    desc = obj->query_look();
    if (!desc) {
        if (obj <- LIB_CREATURE) {
            desc = "a creature";
        } else if (obj <- LIB_ITEM) {
            desc = "an item";
        } else if (obj <- LIB_ROOM) {
            desc = "a room";
        } else {
            desc = "a thing";
        }
    }
    return desc;
}

static string definite_description(object LIB_THING obj,
                                   varargs object LIB_THING observer)
{
    string desc;

    desc = indefinite_description(obj, observer);
    if (sscanf(desc, "a %s", desc) || sscanf(desc, "an %s", desc)) {
        desc = "the " + desc;
    }
    return desc;
}

static string describe_exits(object LIB_ROOM room)
{
    string *dirs;

    dirs = room->query_exits();
    switch (sizeof(dirs)) {
    case 0:
        return "There are no obvious exits.";

    case 1:
        return "The only obvious exit is " + dirs[0] + ".";

    default:
        return "The obvious exits are " + list_strings(dirs) + ".";
    }
}

static object LIB_THING **split_creatures_and_items(object LIB_THING *things)
{
    int i, size, j, k;
    object LIB_CREATURE *creatures;
    object LIB_ITEM *items;

    size = sizeof(things);
    creatures = allocate(size);
    items = allocate(size);
    for (i = 0; i < size; ++i) {
        if (things[i] <- LIB_CREATURE) {
            creatures[j++] = things[i];
        } else if (things[i] <- LIB_ITEM) {
            items[k++] = things[i];
        }
    }
    return ({ creatures[.. j - 1], items[.. k - 1] });
}

static string *describe_each_thing(object LIB_THING *things,
                                   object LIB_THING observer)
{
    int i, size;
    string *descs;

    size = sizeof(things);
    descs = allocate(size);
    for (i = 0; i < size; ++i) {
        descs[i] = (things[i] == observer) ? "you"
            : indefinite_description(things[i], observer);
    }
    return descs;
}

static string describe_creature_inventory(object LIB_CREATURE creature,
                                          object LIB_THING observer)
{
    object LIB_THING *inv;
    string desc;

    inv = inventory(creature);
    desc = sizeof(inv) ? list_strings(describe_each_thing(inv, observer))
        : "nothing";
    return ((creature == observer) ? "You have "
            : capitalize(definite_description(creature)) + " has ") + desc
        + ".";
}

static string describe_creatures_in_room(object LIB_ROOM room,
                                         object LIB_CREATURE *creatures,
                                         object LIB_THING observer)
{
    int size;

    size = sizeof(creatures);
    if (!size) {
        return "Noone is there.";
    } else if (observer && size == 1 && creatures[0] == observer) {
        return "You are alone here.";
    }
    return capitalize(list_strings(describe_each_thing(creatures, observer)))
        + " " + ((size == 1) ? "is" : "are") + " "
        + ((observer && room == (object LIB_ROOM) environment(observer))
           ? "here" : "there") + ".";
}

static string describe_items_in_room(object LIB_ROOM room,
                                     object LIB_ITEM *items,
                                     object LIB_THING observer)
{
    int size;

    size = sizeof(items);
    return "There " + ((size <= 1) ? "is" : "are") + " "
        + (size ? list_strings(describe_each_thing(items, observer))
           : "nothing") + " "
        + (observer && (room == (object LIB_ROOM) environment(observer))
           ? "here" : "there") + ".";
}

static string describe_room_inventory(object LIB_ROOM room,
                                      object LIB_THING observer)
{
    object LIB_THING **split;

    split = split_creatures_and_items(inventory(room));
    if (sizeof(split[0]) && sizeof(split[1])) {
        return describe_creatures_in_room(room, split[0], observer) + " "
            + describe_items_in_room(room, split[1], observer);
    } else if (sizeof(split[0])) {
        return describe_creatures_in_room(room, split[0], observer);
    } else {
        return describe_items_in_room(room, split[1], observer);
    }
}

static string describe_inventory(object LIB_THING thing,
                                 object LIB_THING observer)
{
    return (thing <- LIB_CREATURE)
        ? describe_creature_inventory(thing, observer)
        : describe_room_inventory(thing, observer);
}

static string verbose_description(object LIB_THING thing,
                                  varargs object LIB_THING observer)
{
    string desc;

    desc = thing->query_verbose_look();
    if (!desc) {
        desc = "You see nothing special.";
    }
    if (thing <- LIB_ROOM) {
        desc += " " + describe_exits(thing);
    }
    if (thing <- LIB_ROOM || thing <- LIB_CREATURE) {
        desc += " " + describe_inventory(thing, observer);
    }
    return desc;
}
