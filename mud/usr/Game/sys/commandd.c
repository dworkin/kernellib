# include <game/action.h>

string grammar_;

static void create()
{
    grammar_ = read_file("~/sys/command.grammar");
}

object *parse(string command)
{
    return parse_string(grammar_, command);
}

static mixed *cmd_go_direction(mixed *tree)
{
    return ({ new_object(GO_DIRECTION_ACTION, tree[1]) });
}

static mixed *cmd_look(mixed *tree)
{
    return ({ new_object(LOOK_ACTION) });
}

static mixed *cmd_look_at_thing(mixed *tree)
{
    return ({ new_object(LOOK_AT_THING_ACTION, tree[2]) });
}

static mixed *word_list_to_words(mixed *tree)
{
    return ({ tree });
}

static mixed *words_to_thing(mixed *tree)
{
    return tree;
}

static mixed *thing_list_to_things(mixed *tree)
{
    return ({ tree });
}

