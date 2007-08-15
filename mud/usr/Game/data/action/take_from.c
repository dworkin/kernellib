# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int item_;
int cont_;

static void create(object LIB_ITEM item, object LIB_CONTAINER cont)
{
    item_ = object_number(item);
    cont_ = object_number(cont);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ITEM       item;
    object LIB_CONTAINER  cont;
    object LIB_THING      env;

    cont = find_object(cont_);
    env = cont ? environment(cont) : nil;
    if (!env || env != actor && env != (object LIB_ROOM) environment(actor))
    {
        tell_object(actor, "That is not here.");
        return;
    }

    item = find_object(item_);
    if (!item || (object LIB_THING) environment(item) != cont) {
        tell_object(actor, "You cannot find that.");
        return;
    }

    if (item->move(actor)) {
        tell_object(actor, "You take " + definite_description(item)
                    + " from " + definite_description(cont));
        tell_audience(actor, definite_description(actor)
                      + " takes " + indefinite_description(item)
                      + " from " + indefinite_description(cont));
    } else {
        tell_object(actor, "You cannot take " + definite_description(item)
                    + " from " + definite_description(cont));
    }
}
