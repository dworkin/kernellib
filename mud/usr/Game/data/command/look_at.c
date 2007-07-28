# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR thing_;

static void create(object LIB_SELECTOR thing)
{
    thing_ = thing;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_THING *things;

    things = thing_->select(inventory(actor) + inventory(environment(actor)));
    if (!sizeof(things)) {
        tell_object(actor, "That is not here.");
        return;
    }
    tell_object(actor, verbose_description(things[0], actor));
}
