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
object LIB_SELECTOR cont_;

static void create(object LIB_SELECTOR item, object LIB_SELECTOR cont)
{
    ASSERT_ARG_1(item);
    ASSERT_ARG_2(cont);
    item_ = item;
    cont_ = cont;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM       *items;
    object LIB_THING      *conts;
    object LIB_ROOM        room;
    object LIB_CONTAINER   cont;

    int i, size;

    ASSERT_ARG(actor);

    room = environment(actor);
    conts = cont_->select(room ? inventory(actor) + inventory(room)
                          : inventory(actor));
    size = sizeof(conts);
    if (!size) {
        tell_object(actor, "That is not here.");
        return;
    }
    for (i = 0; i < size; ++i) {
        if (!(conts[i] <- LIB_CONTAINER)) {
            tell_object(actor, "You cannot take anything from "
                        + definite_description(conts[i]));
            return;
        }
    }
    cont = conts[0];

    items = item_->select(inventory(cont));
    size = sizeof(items);
    if (!size) {
        tell_object(actor, "You cannot find that.");
        return;
    }
    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(TAKE_FROM_ACTION, items[i], cont));
    }
}
