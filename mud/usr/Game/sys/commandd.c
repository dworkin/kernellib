# include <game/action.h>
# include <game/command.h>
# include <game/description.h>
# include <game/direction.h>
# include <game/language.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/string.h>
# include <game/thing.h>
# include <system/system.h>

inherit UTIL_DESCRIPTION;
inherit UTIL_DIRECTION;
inherit UTIL_LANGUAGE;
inherit UTIL_MESSAGE;
inherit UTIL_STRING;

string grammar_;

static void create()
{
    grammar_ = read_file("command.grammar");
}

object LIB_ACTION parse(object LIB_CREATURE actor, string command)
{
    mixed *tree;

    tree = parse_string(grammar_, command);
    if (!tree) {
        tell_object(actor, "Invalid command: " + command);
        return nil;
    }
    return call_other(this_object(), "create_" + tree[0] + "_action", actor,
                      tree[1 ..]...);
}

static mixed *parse_drop_command(mixed *tree)
{
    tree -= ({ "down" }); /* put down */
    return ({ "drop", tree[1] });
}

static mixed *parse_give_command(mixed *tree)
{
    return ({ "give", tree[1], tree[3] });
}

static mixed *parse_go_command(mixed *tree)
{
    return ({ "go", tree[sizeof(tree) - 1] });
}

static mixed *parse_inventory_command(mixed *tree)
{
    return ({ "inventory" });
}

static mixed *parse_look_command(mixed *tree)
{
    return ({ "look" });
}

static mixed *parse_look_at_command(mixed *tree)
{
    return ({ "look_at", tree[2] });
}

static mixed *parse_look_to_command(mixed *tree)
{
    return ({ "look_to", tree[1] });
}

static mixed *parse_put_in_command(mixed *tree)
{
    return ({ "put_in", tree[1], tree[3] });
}

static mixed *parse_release_command(mixed *tree)
{
    return ({ "release", tree[1] });
}

static mixed *parse_remove_command(mixed *tree)
{
    tree -= ({ "off" }); /* take off */
    return ({ "remove", tree[1] });
}

static mixed *parse_say_command(mixed *tree)
{
    return ({ "say", tree[0] });
}

static mixed *parse_score_command(mixed *tree)
{
    return ({ "score" });
}

static mixed *parse_take_command(mixed *tree)
{
    tree -= ({ "up" }); /* pick up */
    return ({ "take", tree[1] });
}

static mixed *parse_take_from_command(mixed *tree)
{
    return ({ "take_from", tree[1], tree[sizeof(tree) - 1] });
}

static mixed *parse_wear_command(mixed *tree)
{
    tree -= ({ "on" }); /* put on */
    return ({ "wear", tree[1] });
}

static mixed *parse_wield_command(mixed *tree)
{
    return ({ "wield", tree[1] });
}

static mixed *parse_word(mixed *tree)
{
    return tree;
}

static mixed *parse_words(mixed *tree)
{
    return ({ tree });
}

static mixed *parse_simple_selector(mixed *tree)
{
    return ({ new_object(SIMPLE_SELECTOR, tree[sizeof(tree) - 1]) });
}

static mixed *parse_ordinal_selector(mixed *tree)
{
    tree -= ({ "of", "the" });
    return ({ new_object(ORDINAL_SELECTOR, tree[0], tree[1]) });
}

static mixed *parse_a_selector(mixed *tree)
{
    return ({ new_object(COUNT_SELECTOR, 1, tree[1]) });
}

static mixed *parse_count_selector(mixed *tree)
{
    return ({ new_object(COUNT_SELECTOR, tree[0], tree[sizeof(tree) - 1]) });
}

static mixed *parse_all_selector(mixed *tree)
{
    return ({ new_object(ALL_SELECTOR) });
}

static mixed *parse_all_of_selector(mixed *tree)
{
    tree -= ({ "of", "the" });
    return ({ new_object(ALL_OF_SELECTOR, tree[1]) });
}

static mixed *parse_list_selector(mixed *tree)
{
    return ({ new_object(LIST_SELECTOR, tree - ({ ",", "and", "or" })) });
}

static mixed *parse_except_selector(mixed *tree)
{
    return ({ new_object(EXCEPT_SELECTOR, tree[0], tree[sizeof(tree) - 1]) });
}

static mixed *parse_count(mixed *tree)
{
    mixed count;

    count = ::parse_count(tree[0]);
    return count == nil ? nil : ({ count });
}

static mixed *parse_ordinal(mixed *tree)
{
    mixed ordinal;

    ordinal = ::parse_ordinal(tree[0]);
    return ordinal == nil ? nil : ({ ordinal });
}

static mixed *parse_direction(mixed *tree)
{
    return is_direction(tree[0]) ? tree : nil;
}

static mixed *parse_say(mixed *tree)
{
    string message;

    message = tree[0];
    sscanf(message, "say %s", message);
    return ({ message });
}

static object LIB_ACTION create_drop_action(object LIB_CREATURE actor,
                                            object LIB_SELECTOR items_selector)
{
    object LIB_ROOM   room;
    object LIB_ITEM  *items;

    int i, size;

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return nil;
    }

    items = items_selector->select(inventory(actor));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "You do not have that.");
        return nil;
    }
    return new_object(DROP_ACTION, items);
}

static object LIB_ACTION create_go_action(object LIB_CREATURE actor,
                                          string direction)
{
    return new_object(GO_ACTION, direction);
}

static object LIB_ACTION create_inventory_action(object LIB_CREATURE actor)
{
    return new_object(INVENTORY_ACTION);
}

static object LIB_ACTION create_look_action(object LIB_CREATURE actor)
{
    return new_object(LOOK_ACTION);
}

static object LIB_ACTION
create_put_in_action(object LIB_CREATURE actor,
                     object LIB_SELECTOR items_selector,
                     object LIB_SELECTOR containers_selector)
{
    object LIB_ITEM   *items;
    object LIB_THING  *containers;
    object LIB_ROOM    room;

    int i, size;

    room = environment(actor);
    containers = containers_selector->select(room ? inventory(actor)
                                             + inventory(room)
                                             : inventory(actor));
    size = sizeof(containers);
    if (!size) {
        tell_object(actor, "That is not here.");
        return nil;
    }
    for (i = 0; i < size; ++i) {
        if (!(containers[i] <- LIB_CONTAINER)) {
            tell_object(actor, "You cannot put anything in "
                        + definite_description(containers[i]));
            return nil;
        }
    }

    items = items_selector->select(inventory(actor));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "You do not have that.");
        return nil;
    }
    return new_object(PUT_IN_ACTION, items, containers);
}

static object LIB_ACTION create_say_action(object LIB_CREATURE actor,
                                           string message)
{
    return new_object(SAY_ACTION, message);
}

static object LIB_ACTION create_score_action(object LIB_CREATURE actor)
{
    return new_object(SCORE_ACTION);
}

static object LIB_ACTION create_take_action(object LIB_CREATURE actor,
                                            object LIB_SELECTOR items_selector)
{
    object LIB_ROOM    room;
    object LIB_THING  *items;

    int i, size;

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return nil;
    }

    items = items_selector->select(inventory(room));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "That is not here.");
        return nil;
    }
    for (i = 0; i < size; ++i) {
        if (!(items[i] <- LIB_ITEM)) {
            tell_object(actor, "You cannot take "
                        + definite_description(items[i]));
            return nil;
        }
    }
    return new_object(TAKE_ACTION, items);
}
