# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM room;

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You stare into the void.");
        return;
    }

    tell_object(actor, verbose_description(room, actor));
}
