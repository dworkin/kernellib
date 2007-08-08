# include <game/action.h>
# include <game/attribute.h>
# include <game/description.h>
# include <game/message.h>
# include <game/string.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;
inherit UTIL_STRING;

private string format_attribute(object LIB_CREATURE actor, string name)
{
    return align_left(capitalize(name), 10) + "  "
        + align_right((string) (int) actor->query_attribute(name), 2);
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

    bar = repeat_string("o", percentage / 2)
        + repeat_string("-", 50 - percentage / 2);
    return align_left(capitalize(name), 10) + "  " + bar + "  "
        + align_right(percentage + "%", 4);
}

private float experience_progress(object LIB_CREATURE actor)
{
    return modf(actor->query_level())[0];
}

void perform(object LIB_CREATURE actor)
{
    string message;

    message = "You are " + actor->query_name() + ", a level "
        + (int) actor->query_level() + " " + actor->query_gender() + " "
        + lower_case(actor->query_race()->query_name() + " "
                     + actor->query_guild()->query_name()) + ".\n\n"

        + format_attribute(actor, STRENGTH_ATTRIBUTE) + " | "
        + format_attribute(actor, SPEED_ATTRIBUTE) + " | "
        + format_attribute(actor, PERCEPTION_ATTRIBUTE) + " | "
        + format_attribute(actor, CHARISMA_ATTRIBUTE) + "\n"
        + "---------------+----------------+----------------+---------------\n"

        + format_attribute(actor, ATTACK_ATTRIBUTE) + " | "
        + format_attribute(actor, DEFENSE_ATTRIBUTE) + " | "
        + format_attribute(actor, MAGIC_ATTRIBUTE) + " | "
        + format_attribute(actor, MUSIC_ATTRIBUTE) + "\n"

        + format_attribute(actor, DAMAGE_ATTRIBUTE) + " | "
        + format_attribute(actor, AGILITY_ATTRIBUTE) + " | "
        + format_attribute(actor, PRAYER_ATTRIBUTE) + " | "
        + format_attribute(actor, LEADERSHIP_ATTRIBUTE) + "\n"

        + format_attribute(actor, PROTECTION_ATTRIBUTE) + " | "
        + format_attribute(actor, DEXTERITY_ATTRIBUTE) + " | "
        + format_attribute(actor, STEALTH_ATTRIBUTE) + " | "
        + format_attribute(actor, BARGAIN_ATTRIBUTE) + "\n\n"

        + format_bar(actor, "health", actor->query_health()) + "\n"
        + format_bar(actor, "power", actor->query_power()) + "\n"
        + format_bar(actor, "experience", experience_progress(actor)) + "\n";

    tell_object(actor, message);
}
