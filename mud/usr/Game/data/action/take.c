# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int obj_;

static void create(object LIB_ITEM obj)
{
    ASSERT_ARG(obj);
    obj_ = object_number(obj);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM   room;
    object LIB_THING  obj;

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    obj = find_object(obj_);
    if (!obj || (object LIB_ROOM) environment(obj) != room) {
        tell_object(actor, "That is not here.");
        return;
    }

    if (!(obj <- LIB_ITEM)) {
        tell_object(actor, "You cannot take " + definite_description(obj));
        return;
    }

    if (move_object(obj, actor)) {
        tell_object(actor, "You take "
                    + definite_description(obj, actor));
        tell_audience(actor, definite_description(actor) + " takes "
                      + indefinite_description(obj));
    } else {
        tell_object(actor, "You cannot take "
                    + definite_description(obj, actor));
    }
}
