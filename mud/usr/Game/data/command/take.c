# include <game/action.h>
# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR obj_;

static void create(object LIB_SELECTOR obj)
{
    obj_ = obj;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM    env;
    object LIB_THING  *objs;
    int                i, size;

    env = environment(actor);
    if (!env) {
        tell_object(actor, "You are in the void.");
        return;
    }

    objs = obj_->select(inventory(env));
    size = sizeof(objs);
    if (!size) {
        tell_object(actor, "That is not here.");
        return;
    }
    for (i = 0; i < size; ++i) {
        if (!(objs[i] <- LIB_ITEM)) {
            tell_object(actor, "You cannot take "
                        + definite_description(objs[i]));
            return;
        }
    }

    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(TAKE_ACTION, objs[i]));
    }
}
