# include <game/action.h>
# include <game/command.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_COMMAND;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

object LIB_SELECTOR item_;
object LIB_SELECTOR container_;

static void create(object LIB_SELECTOR item, object LIB_SELECTOR container)
{
    item_ = item;
    container_ = container;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM       *items;
    object LIB_THING      *containers;
    object LIB_ROOM        room;
    object LIB_CONTAINER   container;

    int i, size;

    room = environment(actor);
    containers = container_->select(room ? inventory(actor) + inventory(room)
                                    : inventory(actor));
    size = sizeof(containers);
    if (!size) {
        tell_object(actor, "That is not here.");
        return;
    }
    for (i = 0; i < size; ++i) {
        if (!(containers[i] <- LIB_CONTAINER)) {
            tell_object(actor, "You cannot take anything from "
                        + definite_description(containers[i]));
            return;
        }
    }
    container = containers[0];

    items = item_->select(inventory(container));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "You cannot find that.");
        return;
    }
    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(TAKE_FROM_ACTION, items[i], container));
    }
}
