# include <game/action.h>
# include <game/description.h>
# include <game/direction.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_DIRECTION;
inherit UTIL_MESSAGE;

string direction_;

static void create(string direction)
{
    ASSERT_ARG(direction && is_direction(direction));
    direction_ = direction;
}

static string origin_direction(string direction)
{
    switch (direction) {
    case "up":   return "below";
    case "down": return "above";
    default:     return "the " + reverse_direction(direction);
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
        tell_object(actor, "You cannot look there.");
        return;
    }

    tell_object(actor, verbose_description(destination, actor));
    tell_inventory(destination, indefinite_description(actor)
                   + " looks in from " + origin_direction(direction_));
}
