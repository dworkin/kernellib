# include <game/command.h>
# include <game/direction.h>
# include <game/selector.h>
# include <game/string.h>
# include <system/system.h>

inherit UTIL_DIRECTION;
inherit UTIL_STRING;

string grammar_;

static void create()
{
    grammar_ = read_file("command.grammar");
}

object LIB_COMMAND parse(string command)
{
    mixed *tree;

    tree = parse_string(grammar_, command);
    return tree ? tree[0] : nil;
}

static mixed *parse_inventory_command(mixed *tree)
{
    return ({ new_object(INVENTORY_COMMAND) });
}

static mixed *parse_look_command(mixed *tree)
{
    return ({ new_object(LOOK_COMMAND) });
}

static mixed *parse_look_at_command(mixed *tree)
{
    return ({ new_object(LOOK_AT_COMMAND, tree[2]) });
}

static mixed *parse_go_command(mixed *tree)
{
    return ({ new_object(GO_COMMAND, tree[sizeof(tree) - 1]) });
}

static mixed *parse_pick_up_command(mixed *tree)
{
    tree -= ({ "up" });
    return ({ new_object(PICK_UP_COMMAND, tree[1]) });
}

static mixed *parse_put_down_command(mixed *tree)
{
    tree -= ({ "down" });
    return ({ new_object(PUT_DOWN_COMMAND, tree[1]) });
}

static mixed *parse_say_command(mixed *tree)
{
    return ({ new_object(SAY_COMMAND, tree[sizeof(tree) - 1]) });
}

static mixed *parse_say_to_command(mixed *tree)
{
    return ({ new_object(SAY_TO_COMMAND, tree[sizeof(tree) - 3],
                         tree[sizeof(tree) - 1]) });
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
    string *words;

    words = tree[sizeof(tree) - 1];
    return ({ new_object(SIMPLE_SELECTOR, words) });
}

static mixed *parse_ordinal_selector(mixed *tree)
{
    int      ord;
    string  *words;

    tree -= ({ "of", "the" });
    ord = tree[0];
    words = tree[1];
    return ({ new_object(ORDINAL_SELECTOR, ord, words) });
}

static mixed *parse_a_selector(mixed *tree)
{
    string *words;

    words = tree[1];
    return ({ new_object(COUNT_SELECTOR, 1, words) });
}

static mixed *parse_count_selector(mixed *tree)
{
    int      count;
    string  *words;

    count = tree[0];
    words = tree[sizeof(tree) - 1];
    return ({ new_object(COUNT_SELECTOR, count, words) });
}

static mixed *parse_all_selector(mixed *tree)
{
    return ({ new_object(ALL_SELECTOR) });
}

static mixed *parse_all_of_selector(mixed *tree)
{
    string *words;

    tree -= ({ "of", "the" });
    words = tree[1];
    return ({ new_object(ALL_OF_SELECTOR, words) });
}

static mixed *parse_list_selector(mixed *tree)
{
    object LIB_SELECTOR *sels;

    sels = tree - ({ ",", "and", "or" });
    return ({ new_object(LIST_SELECTOR, sels) });
}

static mixed *parse_except_selector(mixed *tree)
{
    object LIB_SELECTOR incl, excl;

    incl = tree[0];
    excl = tree[sizeof(tree) - 1];
    return ({ new_object(EXCEPT_SELECTOR, incl, excl) });
}

static mixed *parse_count(mixed *tree)
{
    int count;

    switch (tree[0]) {
    case "one":    return ({  1 });
    case "two":    return ({  2 });
    case "three":  return ({  3 });
    case "four":   return ({  4 });
    case "five":   return ({  5 });
    case "six":    return ({  6 });
    case "seven":  return ({  7 });
    case "eight":  return ({  8 });
    case "nine":   return ({  9 });
    case "ten":    return ({ 10 });
    case "eleven": return ({ 11 });
    case "twelve": return ({ 12 });
    }

    if (sscanf(tree[0], "%d%*c", count) == 1 && count >= 1) {
        return ({ count });
    } else {
        return nil;
    }
}

static mixed *parse_ordinal(mixed *tree)
{
    int ord;
    string suffix;

    switch (tree[0]) {
    case "first":    return ({  1 });
    case "second":   return ({  2 });
    case "third":    return ({  3 });
    case "fourth":   return ({  4 });
    case "fifth":    return ({  5 });
    case "sixth":    return ({  6 });
    case "seventh":  return ({  7 });
    case "eighth":   return ({  8 });
    case "ninth":    return ({  9 });
    case "tenth":    return ({ 10 });
    case "eleventh": return ({ 11 });
    case "twelveth": return ({ 12 });
    }

    if (sscanf(tree[0], "%d%s", ord, suffix) == 2 && ord >= 1
        && (suffix == "st" || suffix == "nd" || suffix == "rd"))
    {
        return ({ ord });
    } else {
        return nil;
    }
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
