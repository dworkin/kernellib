# include <game/action.h>
# include <game/direction.h>
# include <game/string.h>
# include <system/system.h>

inherit UTIL_DIRECTION;
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

static mixed *parse_go_command(mixed *tree)
{
    return ({ new_object(GO_ACTION, tree[1]) });
}

static mixed *parse_look_command(mixed *tree)
{
    return ({ new_object(LOOK_ACTION) });
}

static mixed *parse_look_at_command(mixed *tree)
{
    return ({ new_object(LOOK_AT_ACTION, tree[2]) });
}

static mixed *parse_say_command(mixed *tree)
{
    return ({ new_object(SAY_ACTION, tree[sizeof(tree) - 1]) });
}

static mixed *parse_say_to_command(mixed *tree)
{
    return ({ new_object(SAY_TO_ACTION, tree[sizeof(tree) - 3],
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

static mixed *parse_selector(mixed *tree)
{
    tree -= ({ "the" });
    return ({ ([ "words": tree[0] ]) });
}

static mixed *parse_ordinal_selector(mixed *tree)
{
    tree -= ({ "of", "the" });
    return ({ ([ "ord": tree[0], "words": tree[1] ]) });
}

static mixed *parse_a_selector(mixed *tree)
{
    return ({ ([ "count": 1, "words": tree[1] ]) });
}

static mixed *parse_count_selector(mixed *tree)
{
    tree -= ({ "of", "the" });
    return ({ ([ "count": tree[0], "words": tree[1] ]) });
}

static mixed *parse_all_selector(mixed *tree)
{
    return ({ ([ "count": -1 ]) });
}

static mixed *parse_all_of_selector(mixed *tree)
{
    tree -= ({ "of", "the" });
    return ({ ([ "count": -1, "words": tree[1] ]) });
}

static mixed *parse_selector_2(mixed *tree)
{
    tree -= ({ ",", "and", "or" });
    return ({ tree });
}

static mixed *parse_except_selector(mixed *tree)
{
    return ({ tree });
}

static mixed *parse_top_selector(mixed *tree)
{
    return tree;
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
