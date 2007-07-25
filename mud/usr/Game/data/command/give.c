# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR obj_;
object LIB_SELECTOR ind_;

static void create(object LIB_SELECTOR obj, object LIB_SELECTOR ind)
{
    obj_ = obj;
    ind_ = ind;
}

static object give(object obj, object *inds)
{
    int i, size;

    size = sizeof(inds);
    for (i = 0; i < size; ++i) {
        if (move_object(obj, inds[i])) {
            return inds[i];
        }
    }
    return nil;
}

void perform(object LIB_CREATURE actor)
{
    object *objs, env, *inds;
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

    env = environment(actor);
    if (!env) {
        tell_object(actor, "You are in the void.");
        return;
    }

    inds = ind_->select(inventory(env));
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

    size = sizeof(objs);
    for (i = 0; i < size; ++i) {
        object ind;

        ind = give(objs[i], inds);
        if (ind) {
            tell_object(actor, "You give " + definite_description(objs[i])
                        + " to " + definite_description(ind));
            tell_object(ind, definite_description(actor) + " gives "
                        + indefinite_description(objs[i]) + " to you.");
            tell_audience_except(actor, ({ ind }), definite_description(actor)
                                 + " gives " + indefinite_description(objs[i])
                                 + " to " + indefinite_description(ind));
        } else {
            tell_object(actor, "You cannot give away "
                        + definite_description(objs[i]));
        }
    }
}
