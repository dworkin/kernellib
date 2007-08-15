# include <game/armor.h>
# include <game/language.h>
# include <game/string.h>
# include <game/thing.h>

private inherit UTIL_LANGUAGE;
private inherit UTIL_STRING;

static string indefinite_description(object LIB_THING thing,
                                     varargs object LIB_THING observer)
{
    string description;

    description = thing->describe(observer);
    if (!description) {
        if (thing <- LIB_CREATURE) {
            description = "a creature";
        } else if (thing <- LIB_ITEM) {
            description = "an item";
        } else if (thing <- LIB_ROOM) {
            description = "a room";
        } else {
            description = "a thing";
        }
    }
    return description;
}

static string definite_description(object LIB_THING thing,
                                   varargs object LIB_THING observer)
{
    string description;

    description = indefinite_description(thing, observer);
    if (sscanf(description, "a %s", description)
        || sscanf(description, "an %s", description))
    {
        description = "the " + description;
    }
    return description;
}

static string describe_exits(object LIB_ROOM room)
{
    string *exits;

    exits = room->query_exits();
    switch (sizeof(exits)) {
    case 0:
        return "There are no obvious exits.";

    case 1:
        return "The only obvious exit is " + exits[0] + ".";

    default:
        return "The obvious exits are " + list_strings(exits) + ".";
    }
}

static object LIB_THING **split_creatures_and_items(object LIB_THING *things)
{
    int i, size, j, k;

    object LIB_CREATURE  *creatures;
    object LIB_ITEM      *items;

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
    string *descriptions;

    size = sizeof(things);
    descriptions = allocate(size);
    for (i = 0; i < size; ++i) {
        descriptions[i] = indefinite_description(things[i], observer);
    }
    return descriptions;
}

static string list_things(object LIB_THING *things, object LIB_THING observer)
{
    return sizeof(things)
        ? list_strings(describe_each_thing(things, observer)) : "nothing";
}

static string describe_creature_inventory(object LIB_CREATURE creature,
                                          object LIB_THING observer)
{
    object LIB_WEAPON       *wielded;
    object LIB_ARMOR_PIECE  *worn;
    object LIB_ITEM         *items;

    string it, it_is, it_has, it_also_has, *lists;

    wielded = creature->query_wielded();
    worn = creature->query_worn();
    items = inventory(creature) - wielded - worn;

    it = (creature == observer) ? "You"
        : capitalize(nominative_pronoun(creature->query_gender()));
    it_has = (creature == observer) ? "You have" : it + " has";
    if (!sizeof(wielded) && !sizeof(worn)) {
        return it_has + " " + list_things(items, observer) + ".";
    }
    it_is = (creature == observer) ? "You are" : it + " is";
    it_also_has = (creature == observer) ? "You also have" : it + " also has";

    lists = ({ });
    if (sizeof(wielded)) {
        lists += ({ it_is + " wielding " + list_things(wielded, observer)
                        + "." });
    }
    if (sizeof(worn)) {
        lists += ({ it_is + " wearing " + list_things(worn, observer) + "." });
    }
    if (sizeof(items)) {
        lists += ({ it_also_has + " " + list_things(items, observer) + "." });
    }
    return implode(lists, " ");
}

static string describe_container_inventory(object LIB_CONTAINER container,
                                           object LIB_THING observer)
{
    object LIB_ITEM *items;

    items = inventory(container);
    if (sizeof(items)) {
        return "It contains "
            + list_strings(describe_each_thing(items, observer)) + ".";
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
                                     object LIB_THING observer,
                                     int also)
{
    int     size;
    string  description;

    size = sizeof(items);
    description = "There " + (size <= 1 ? "is " : "are ");
    if (also) {
        description += "also ";
    }
    description += list_things(items, observer);
    description += observer && room == (object LIB_ROOM) environment(observer)
        ? " here." : " there.";
    return description;
        
}

static string describe_room_inventory(object LIB_ROOM room,
                                      object LIB_THING observer)
{
    object LIB_THING **split;

    split = split_creatures_and_items(inventory(room));
    if (sizeof(split[0]) && sizeof(split[1])) {
        return describe_creatures_in_room(room, split[0], observer) + " "
            + describe_items_in_room(room, split[1], observer,
                                     sizeof(split[0]) >= 2
                                     || split[0][0] != observer);
    } else if (sizeof(split[0])) {
        return describe_creatures_in_room(room, split[0], observer);
    } else {
        return describe_items_in_room(room, split[1], observer, FALSE);
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
    string description;

    description = thing->describe_verbose(observer);
    if (!description) {
        description = "You see nothing special.";
    }
    if (thing <- LIB_ROOM) {
        description += " " + describe_exits(thing);
    }
    if (thing <- LIB_ROOM) {
        description = break_string(description)
            + indent_string(describe_inventory(thing, observer));
    } else if (thing <- LIB_CREATURE || thing <- LIB_CONTAINER) {
        description += " " + describe_inventory(thing, observer);
    }
    return description;
}
