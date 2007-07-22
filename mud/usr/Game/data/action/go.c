# include <game/action.h>
# include <game/description.h>
# include <game/direction.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_DIRECTION;
inherit UTIL_MESSAGE;

string dir_;

static void create(string dir)
{
    dir_ = dir;
}

static string arrival_direction(string dir)
{
    switch (dir) {
    case "up":   return "below";
    case "down": return "above";
    default:     return "the " + reverse_direction(dir);
    }
}

void perform(object LIB_CREATURE actor)
{
    object env, dest;

    env = environment(actor);
    if (!env) {
        tell_object(actor, "You are in the void.");
        return;
    }

    dest = env->query_exit(dir_);
    if (!dest) {
        tell_object(actor, "You cannot go " + dir_ + " from here.");
        return;
    }

    if (move_object(actor, dest)) {
        tell_inventory(env, definite_description(actor) + " leaves " + dir_);
        tell_object(actor, verbose_description(dest));
        tell_audience(actor, indefinite_description(actor) + " arrives from "
                      + arrival_direction(dir_));
    } else {
        tell_object(actor, "You cannot go there.");
    }
}
