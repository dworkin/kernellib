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
object LIB_SELECTOR creature_;

static void create(object LIB_SELECTOR item, object LIB_SELECTOR creature)
{
    ASSERT_ARG_1(item);
    ASSERT_ARG_2(creature);
    item_ = item;
    creature_ = creature;
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM      *items;
    object LIB_ROOM       room;
    object LIB_THING     *creatures;
    object LIB_CREATURE   creature;

    int i, size;

    items = item_->select(inventory(actor));
    if (!sizeof(items)) {
        tell_object(actor, "You do not have that.");
        return;
    }

    room = environment(actor);
    if (!room) {
        tell_object(actor, "You are in the void.");
        return;
    }

    creatures = creature_->select(inventory(room) - ({ actor }));
    size = sizeof(creatures);
    if (!size) {
        tell_object(actor, "They are not here.");
        return;
    }
    for (i = 0; i < size; ++i) {
        if (!(creatures[i] <- LIB_CREATURE)) {
            tell_object(actor, "You cannot give anything to "
                        + definite_description(creatures[i]));
            return;
        }
    }
    creature = creatures[0];

    size = sizeof(items);
    for (i = 0; i < size; ++i) {
        actor->add_action(new_object(GIVE_ACTION, items[i], creature));
    }
}
