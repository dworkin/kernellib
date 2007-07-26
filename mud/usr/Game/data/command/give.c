# include <game/action.h>
# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR obj_;
object LIB_SELECTOR ind_;

static void create(object LIB_SELECTOR obj, object LIB_SELECTOR ind)
{
    ASSERT_ARG_1(obj);
    ASSERT_ARG_2(ind);
    obj_ = obj;
    ind_ = ind;
}

void perform(object LIB_CREATURE actor)
{
    object *objs, *inds;
    object LIB_ROOM room;
    object LIB_CREATURE ind;
    int i, size;

    objs = obj_->select(inventory(actor));
    if (!sizeof(objs)) {
        tell_object(actor, "You do not have that.");
        return;
    }
    size = sizeof(objs);
    for (i = 0; i < size; ++i) {
        if (!(objs[i] <- LIB_ITEM)) {
            tell_object(actor, "You cannot give away "
                        + definite_description(objs[i]));
            return;
        }
    }

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    inds = ind_->select(inventory(room));
    size = sizeof(inds);
    if (!size) {
        tell_object(actor, "They are not here.");
        return;
    }
    for (i = 0; i < size; ++i) {
        if (!(inds[i] <- LIB_CREATURE)) {
            tell_object(actor, "You cannot give anything to "
                        + definite_description(inds[i]));
            return;
        }
    }
    ind = inds[0];

    size = sizeof(objs);
    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(GIVE_ACTION, objs[i], ind));
    }
}
