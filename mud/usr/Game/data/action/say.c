# include <game/action.h>
# include <game/armor.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/string.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;
inherit UTIL_STRING;

string message_;

static void create(string message)
{
    ASSERT_ARG(message);
    message_ = capitalize(message);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM room;

    ASSERT_ARG(actor);

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    tell_object(actor, "You say: " + message_);
    tell_audience(actor, definite_description(actor) + " says: " + message_);
}
