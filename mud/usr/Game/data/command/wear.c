# include <game/action.h>
# include <game/armor.h>
# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR item_;

static void create(object LIB_SELECTOR item)
{
    item_ = item;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM *items, *worn;

    int i, size;

    items = item_->select(inventory(actor));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "You do not have that.");
        return;
    }

    for (i = 0; i < size; ++i) {
        if (!(items[i] <- LIB_ARMOR_PIECE)) {
            tell_object(actor, "You cannot wear "
                        + definite_description(items[i]));
            return;
        }
    }

    worn = items & actor->query_worn();
    if (sizeof(worn)) {
        tell_object(actor, "You are already wearing "
                    + definite_description(worn[0]));
        return;
    }

    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(WEAR_ACTION, items[i]));
    }
}
