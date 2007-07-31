# include <game/action.h>
# include <game/armor.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int weapon_;

static void create(object LIB_WEAPON weapon)
{
    ASSERT_ARG(weapon);
    weapon_ = object_number(weapon);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_WEAPON weapon;

    ASSERT_ARG(actor);

    weapon = find_object(weapon_);
    if (!weapon || (object LIB_THING) environment(weapon) != actor) {
        tell_object(actor, "You do not have that.");
        return;
    }

    if (sizeof(actor->query_wielded() & ({ weapon }))) {
        tell_object(actor, "You are already wielding that.");
        return;
    }

    actor->add_wielded(weapon);
    tell_object(actor, "You wield " + definite_description(weapon, actor));
    tell_audience(actor, definite_description(actor) + " wields "
                  + indefinite_description(weapon));
}
