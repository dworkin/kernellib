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
    return align_left(capitalize(name) + ":", 13) + "  "
        + align_right((string) (int) actor->query_trait(name), 2);
}

private string format_bar(object LIB_CREATURE actor, string name,
                          float fraction)
{
    int     percentage;
    string  bar;

    percentage = (int) (fraction * 100.0);
    if (percentage < 0) {
        percentage = 0;
    } else if (percentage > 100) {
        percentage = 100;
    }

    bar = "[" + align_left(repeat_string("#", percentage / 2), 50) + "]";
    return align_left(capitalize(name) + ":", 7) + "  " + bar + "  "
        + align_right(percentage + "%", 4);
}

void perform(object LIB_CREATURE actor)
{
    string message;

    message = "You are " + actor->query_name() + " the "
        + actor->query_gender() + " "
        + lower_case(actor->query_race()->query_name() + " "
                     + actor->query_guild()->query_name()) + ".\n\n"

        + format_trait(actor, "strength") + " | "
        + format_trait(actor, "charisma") + " | "
        + format_trait(actor, "attack") + " | "
        + format_trait(actor, "defense") + "\n"

        + format_trait(actor, "constitution") + " | "
        + format_trait(actor, "intelligence") + " | "
        + format_trait(actor, "damage") + " | "
        + format_trait(actor, "protection") + "\n"

        + format_trait(actor, "dexterity") + " | "
        + format_trait(actor, "wisdom") + " | "
        + format_trait(actor, "speed") + " |\n\n"

        + format_bar(actor, "health", actor->query_health()) + "\n"
        + format_bar(actor, "power", actor->query_power()) + "\n";

    tell_object(actor, message);
}
