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
    object LIB_ITEM    *items;
    object LIB_WEAPON  *wielded;

    int i, size;

    items = item_->select(inventory(actor));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "You do not have that.");
        return;
    }

    wielded = items & actor->query_wielded();
    items -= wielded;
    if (sizeof(items)) {
        tell_object(actor, "You are not wielding "
                    + definite_description(items[0]));
        return;
    }

    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(RELEASE_ACTION, wielded[i]));
    }
}
