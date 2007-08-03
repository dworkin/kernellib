# include <game/action.h>
# include <game/command.h>
# include <game/direction.h>
# include <game/language.h>
# include <game/selector.h>
# include <game/string.h>
# include <system/system.h>

inherit UTIL_DIRECTION;
inherit UTIL_LANGUAGE;
inherit UTIL_STRING;

string grammar_;

static void create()
{
    grammar_ = read_file("command.grammar");
}

object LIB_ACTION parse(string command)
{
    mixed *tree;

    tree = parse_string(grammar_, command);
    return tree ? tree[0] : nil;
}

static mixed *parse_drop_command(mixed *tree)
{
    tree -= ({ "down" });
    return ({ new_object(DROP_COMMAND, tree[1]) });
}

static mixed *parse_give_command(mixed *tree)
{
    return ({ new_object(GIVE_COMMAND, tree[1], tree[3]) });
}

static mixed *parse_go_command(mixed *tree)
{
    return ({ new_object(GO_ACTION, tree[sizeof(tree) - 1]) });
}

static mixed *parse_inventory_command(mixed *tree)
{
    return ({ new_object(INVENTORY_ACTION) });
}

static mixed *parse_look_command(mixed *tree)
{
    return ({ new_object(LOOK_ACTION) });
}

static mixed *parse_look_at_command(mixed *tree)
{
    return ({ new_object(LOOK_AT_COMMAND, tree[2]) });
}

static mixed *parse_look_to_command(mixed *tree)
{
    return ({ new_object(LOOK_TO_ACTION, tree[1]) });
}

static mixed *parse_put_in_command(mixed *tree)
{
    return ({ new_object(PUT_IN_COMMAND, tree[1], tree[3]) });
}

static mixed *parse_release_command(mixed *tree)
{
    return ({ new_object(RELEASE_COMMAND, tree[1]) });
}

static mixed *parse_remove_command(mixed *tree)
{
    tree -= ({ "off" });
    return ({ new_object(REMOVE_COMMAND, tree[1]) });
}

static mixed *parse_say_command(mixed *tree)
{
    return ({ new_object(SAY_ACTION, tree[sizeof(tree) - 1]) });
}

static mixed *parse_say_to_command(mixed *tree)
{
    return ({ new_object(SAY_TO_COMMAND, tree[sizeof(tree) - 3],
                         tree[sizeof(tree) - 1]) });
}

static mixed *parse_take_command(mixed *tree)
{
    tree -= ({ "up" });
    return ({ new_object(TAKE_COMMAND, tree[1]) });
}

static mixed *parse_take_from_command(mixed *tree)
{
    return ({ new_object(TAKE_FROM_COMMAND, tree[1],
                         tree[sizeof(tree) - 1]) });
}

static mixed *parse_wear_command(mixed *tree)
{
    tree -= ({ "on" });
    return ({ new_object(WEAR_COMMAND, tree[1]) });
}

static mixed *parse_wield_command(mixed *tree)
{
    return ({ new_object(WIELD_COMMAND, tree[1]) });
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

static mixed *parse_quote(mixed *tree)
{
    string quote, q;

    quote = tree[0];
    q = quote[0 .. 0];
    quote = remove_suffix(quote, q)[1 ..];
    return ({ quote });
}
