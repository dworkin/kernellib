# include <game/action.h>
# include <game/description.h>
# include <game/direction.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_DIRECTION;
inherit UTIL_MESSAGE;

string direction_;

static void create(string direction)
{
    direction_ = direction;
}

static string arrival_direction(string direction)
{
    switch (direction) {
    case "up":    return "below";
    case "down":  return "above";
    default:      return "the " + reverse_direction(direction);
    }
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM room, destination;

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    destination = room->query_exit(direction_);
    if (!destination) {
        tell_object(actor, "You cannot go " + direction_ + " from here.");
        return;
    }

    if (actor->move(destination)) {
        tell_inventory(room, definite_description(actor) + " leaves "
                       + direction_);
        tell_object(actor, verbose_description(destination, actor));
        tell_audience(actor, indefinite_description(actor) + " arrives from "
                      + arrival_direction(direction_));
    } else {
        tell_object(actor, "You cannot go there.");
    }
}
