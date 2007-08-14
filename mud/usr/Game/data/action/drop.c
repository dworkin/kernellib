# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int item_;

static void create(object LIB_ITEM item)
{
    ASSERT_ARG(item);
    item_ = object_number(item);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ROOM room;
    object LIB_ITEM item;

    ASSERT_ARG(actor);

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    item = find_object(item_);
    if (!item || (object LIB_CREATURE) environment(item) != actor) {
        tell_object(actor, "You do not have that.");
        return;
    }

    if (item->move(room)) {
        tell_object(actor, "You drop "
                    + definite_description(item, actor));
        tell_audience(actor, definite_description(actor) + " drops "
                      + indefinite_description(item));
    } else {
        tell_object(actor, "You cannot drop "
                    + definite_description(item, actor));
    }
}
