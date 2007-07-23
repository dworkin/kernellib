# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR obj_;

static void create(object LIB_SELECTOR obj)
{
    obj_ = obj;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM    room;
    object LIB_THING  *objs;
    int                i, size;

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    objs = obj_->select(inventory(actor));
    size = sizeof(objs);
    if (!size) {
        tell_object(actor, "You do not have that.");
        return;
    }
    for (i = 0; i < size; ++i) {
        if (move_object(objs[i], room)) {
            tell_object(actor, "You put down "
                        + definite_description(objs[i], actor));
            tell_audience(actor, definite_description(actor) + " puts down "
                          + indefinite_description(objs[i]));
        } else {
            tell_object(actor, "You cannot put down "
                        + definite_description(objs[i], actor));
        }
    }
}
