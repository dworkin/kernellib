# include <game/action.h>
# include <game/description.h>
# include <game/message.h>
# include <game/string.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;
inherit UTIL_STRING;

private string format_trait(object LIB_CREATURE actor, string name)
{
    string value;

    value = (string) ((int) actor->query_trait(name) + 10);
    return capitalize(name)
        + ":                            "[.. 16 - strlen(name) - strlen(value)]
        + value;
}

void perform(object LIB_CREATURE actor)
{
    string str;

    str = "Name:    " + actor->query_name() + "\n"
        + "Gender:  " + capitalize(actor->query_gender()) + "\n"
        + "Race:    " + actor->query_race()->query_name() + "\n"
        + "Guild:   " + actor->query_guild()->query_name() + "\n\n";

    str += format_trait(actor, "strength") + "  "
        + format_trait(actor, "charisma") + "\n"
        + format_trait(actor, "constitution") + "  "
        + format_trait(actor, "intelligence") + "\n"
        + format_trait(actor, "dexterity") + "  "
        + format_trait(actor, "wisdom") + "\n";

    tell_object(actor, str);
}
