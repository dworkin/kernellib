# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR obj_;

static void create(varargs object LIB_SELECTOR obj)
{
    obj_ = obj;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_THING *objs;

    objs = obj_->select(inventory(actor) + inventory(environment(actor)));
    if (!sizeof(objs)) {
        tell_object(actor, "That is not here.");
        return;
    }

    tell_object(actor, verbose_description(objs[0], actor));
}
