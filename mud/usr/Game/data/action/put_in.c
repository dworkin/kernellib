# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int item_;
int container_;

static void create(object LIB_ITEM *items, object LIB_CONTAINER *containers)
{
    item_ = object_number(items[0]);
    container_ = object_number(containers[0]);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM       item;
    object LIB_CONTAINER  container;
    object LIB_THING      environment;

    container = find_object(container_);
    environment = container ? environment(container) : nil;
    if (!environment || environment != actor
        && environment != (object LIB_ROOM) environment(actor))
    {
        tell_object(actor, "That is not here.");
        return;
    }

    item = find_object(item_);
    if (!item || (object LIB_THING) environment(item) != actor) {
        tell_object(actor, "You do not have that.");
        return;
    }

    if (item->move(container)) {
        tell_object(actor, "You put " + definite_description(item)
                    + " in " + definite_description(container));
        tell_audience(actor, definite_description(actor)
                      + " puts " + indefinite_description(item)
                      + " in " + indefinite_description(container));
    } else {
        tell_object(actor, "You cannot put " + definite_description(item)
                    + " in " + definite_description(container));
    }
}
