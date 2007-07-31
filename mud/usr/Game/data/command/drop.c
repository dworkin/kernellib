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

object LIB_SELECTOR item_;

static void create(object LIB_SELECTOR item)
{
    ASSERT_ARG(item);
    item_ = item;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM   room;
    object LIB_ITEM  *items;
    int               i, size;

    ASSERT_ARG(actor);

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    items = item_->select(inventory(actor));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "You do not have that.");
        return;
    }

    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(DROP_ACTION, items[i]));
    }
}
