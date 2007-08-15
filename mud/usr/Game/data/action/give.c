# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int obj_;
int ind_;

static void create(object LIB_ITEM obj, object LIB_CREATURE ind)
{
    obj_ = object_number(obj);
    ind_ = object_number(ind);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM      obj;
    object LIB_ROOM      room;
    object LIB_CREATURE  ind;

    obj = find_object(obj_);
    if (!obj || (object LIB_THING) environment(obj) != actor) {
        tell_object(actor, "You do not have that.");
        return;
    }

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    ind = find_object(ind_);
    if (!ind || (object LIB_ROOM) environment(ind) != room) {
        tell_object(actor, "They are not here.");
        return;
    }

    if (obj->move(ind)) {
        tell_object(actor, "You give " + definite_description(obj)
                    + " to " + definite_description(ind));
        tell_object(ind, definite_description(actor) + " gives "
                    + indefinite_description(obj) + " to you.");
        tell_audience_except(actor, ({ ind }), definite_description(actor)
                             + " gives " + indefinite_description(obj)
                             + " to " + indefinite_description(ind));
    } else {
        tell_object(actor, "You cannot give " + definite_description(obj)
                    + " to " + definite_description(ind));
    }
}
