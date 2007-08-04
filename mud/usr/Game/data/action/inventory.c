# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

void perform(object LIB_CREATURE actor)
{
    tell_object(actor, describe_creature_inventory(actor, actor));
}
