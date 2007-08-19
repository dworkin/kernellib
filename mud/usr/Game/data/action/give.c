# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int item_;
int creature_;

static void create(object LIB_ITEM *items, object LIB_CREATURE *creatures)
{
    item_ = object_number(items[0]);
    creature_ = object_number(creatures[0]);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM      item;
    object LIB_ROOM      room;
    object LIB_CREATURE  creature;

    item = find_object(item_);
    if (!item || (object LIB_THING) environment(item) != actor) {
        tell_object(actor, "You do not have that.");
        return;
    }

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    creature = find_object(creature_);
    if (!creature || (object LIB_ROOM) environment(creature) != room) {
        tell_object(actor, "They are not here.");
        return;
    }

    if (item->move(creature)) {
        tell_object(actor, "You give " + definite_description(item)
                    + " to " + definite_description(creature) + ".");
        tell_object(creature, definite_description(actor) + " gives "
                    + indefinite_description(item) + " to you.");
        tell_audience_except(actor, ({ creature }), definite_description(actor)
                             + " gives " + indefinite_description(item)
                             + " to " + indefinite_description(creature)
                             + ".");
    } else {
        tell_object(actor, "You cannot give " + definite_description(item)
                    + " to " + definite_description(creature));
    }
}
