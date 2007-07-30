# include <game/language.h>
# include <game/string.h>
# include <game/thing.h>

private inherit UTIL_LANGUAGE;
private inherit UTIL_STRING;

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
            : capitalize(nominative_pronoun(creature->query_gender()))
            + " has ") + desc + ".";
}

static string describe_container_inventory(object LIB_CONTAINER cont,
                                           object LIB_THING observer)
{
    object LIB_THING *inv;

    inv = inventory(cont);
    if (sizeof(inv)) {
        return "It contains "
            + list_strings(describe_each_thing(inv, observer)) + ".";
    } else {
        return "It is empty.";
    }
}

static string describe_creatures_in_room(object LIB_ROOM room,
                                         object LIB_CREATURE *creatures,
                                         object LIB_THING observer)
{
    int local, size;

    local = (observer && room == (object LIB_ROOM) environment(observer));
    if (local) {
        creatures -= ({ observer });
    }
    size = sizeof(creatures);
    if (!size) {
        return local ? "You are alone here." : "Noone is there.";
    }
    return capitalize(list_strings(describe_each_thing(creatures, observer))
                      + ((size == 1) ? " is" : " are")
                      + (local ? " here with you." : " there."));
}

static string describe_items_in_room(object LIB_ROOM room,
                                     object LIB_ITEM *items,
                                     object LIB_THING observer)
{
    int size;

    size = sizeof(items);
    return ((size <= 1) ? "There is " : "There are ")
        + (size ? list_strings(describe_each_thing(items, observer))
           : "nothing")
        + (observer && (room == (object LIB_ROOM) environment(observer))
           ? " here." : " there.");
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
    if (thing <- LIB_CREATURE) {
        return describe_creature_inventory(thing, observer);
    } else if (thing <- LIB_CONTAINER) {
        return describe_container_inventory(thing, observer);
    } else  {
        return describe_room_inventory(thing, observer);
    }
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
    if (thing <- LIB_ROOM) {
        desc += "\n  " + describe_inventory(thing, observer) + "\n";
    } else if (thing <- LIB_CREATURE || thing <- LIB_CONTAINER) {
        desc += " " + describe_inventory(thing, observer);
    }
    return desc;
}
