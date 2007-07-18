# include <game/action.h>
# include <system/system.h>

string grammar_;

static void create()
{
    grammar_ = read_file("command.grammar");
}

object *parse(string command)
{
    return parse_string(grammar_, command);
}

static mixed *parse_go_direction_command(mixed *tree)
{
    return tree;
    return ({ new_object(GO_DIRECTION_ACTION, tree[1]) });
}

static mixed *parse_look_command(mixed *tree)
{
    return tree;
    return ({ new_object(LOOK_ACTION) });
}

static mixed *parse_look_at_selection_command(mixed *tree)
{
    return tree;
    return ({ new_object(LOOK_AT_SELECTION_ACTION, tree[2]) });
}

static mixed *parse_say_quote_command(mixed *tree)
{
    return tree;
    return ({ new_object(SAY_QUOTE_ACTION, tree[1]) });
}

static mixed *parse_say_quote_to_selection_command(mixed *tree)
{
    return tree;
    return ({ new_object(SAY_QUOTE_TO_SELECTION_ACTION, tree[1], tree[2]) });
}

static mixed *parse_words(mixed *tree)
{
    return ({ tree });
}

static int is_adjective(string str)
{
    switch (str) {
    case "bald":
    case "big":
    case "black":
    case "blue":
    case "bright":
    case "brown":
    case "clean":
    case "clever":
    case "dark":
    case "dirty":
    case "dull":
    case "fat":
    case "fast":
    case "furry":
    case "gray":
    case "green":
    case "hairy":
    case "happy":
    case "heavy":
    case "light":
    case "loud":
    case "old":
    case "orange":
    case "pink":
    case "red":
    case "rusty":
    case "sharp":
    case "shiny":
    case "shy":
    case "slow":
    case "small":
    case "strong":
    case "stupid":
    case "thin":
    case "two-headed":
    case "weak":
    case "white":
    case "yellow":
    case "young":
        return TRUE;

    default:
        return FALSE;
    }
}

static mixed *parse_adjective(mixed *tree)
{
    return is_adjective(tree[0]) ? tree : nil;
}

static mixed *parse_adjectives(mixed *tree)
{
    return ({ tree - ({ ",", "and" }) });
}

static mixed *parse_optional_adjectives(mixed *tree)
{
    return ({ ({ }) });
}

static int is_noun(string str)
{
    switch (str) {
    case "amulet":
    case "arm":
    case "armor":
    case "axe":
    case "body":
    case "boot":
    case "boy":
    case "bracelet":
    case "cage":
    case "child":
    case "cloak":
    case "dragon":
    case "dwarf":
    case "ear":
    case "elf":
    case "eye":
    case "finger":
    case "foot":
    case "gauntlet":
    case "giant":
    case "girl":
    case "glove":
    case "goblin":
    case "greave":
    case "guard":
    case "head":
    case "helmet":
    case "hand":
    case "human":
    case "hunter":
    case "hydra":
    case "knife":
    case "leg":
    case "man":
    case "mouth":
    case "neck":
    case "nose":
    case "priest":
    case "ranger":
    case "ring":
    case "shield":
    case "shirt":
    case "shoe":
    case "spear":
    case "sword":
    case "throat":
    case "toe":
    case "troll":
    case "warrior":
    case "wizard":
    case "woman":
        return TRUE;

    default:
        return FALSE;
    }
}

static mixed *parse_noun(mixed *tree)
{
    return is_noun(tree[0]) ? tree : nil;
}

static mixed *parse_nouns(mixed *tree)
{
    return ({ tree });
}

static mixed *parse_optional_the(mixed *tree)
{
    return ({ });
}

static mixed *parse_selection(mixed *tree)
{
    return ({ tree });
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

static int is_direction(string str)
{
    switch (str) {
    case "east":
    case "north":
    case "northeast":
    case "northwest":
    case "south":
    case "southeast":
    case "southwest":
    case "west":

    case "down":
    case "up":
        return TRUE;

    default:
        return FALSE;
    }
}

static mixed *parse_direction(mixed *tree)
{
    return is_direction(tree[0]) ? tree : nil;
}

static string remove_suffix(string str, string suffix)
{
    int len, suffix_len;

    len = strlen(str);
    suffix_len = strlen(suffix);
    if (len >= suffix_len && str[len - suffix_len ..] == suffix) {
        return str[.. len - suffix_len - 1];
    } else {
        return str;
    }
}

static mixed *parse_quote(mixed *tree)
{
    string quote, q;

    quote = tree[0];
    q = quote[0 .. 0];
    quote = remove_suffix(quote, q)[1 ..];
    return ({ quote });
}
