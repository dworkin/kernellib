# include <game/description.h>
# include <game/language.h>
# include <game/race.h>
# include <game/string.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ITEM;
inherit UTIL_DESCRIPTION;
inherit UTIL_LANGUAGE;
inherit UTIL_STRING;

object LIB_RACE race_;

static void create(object LIB_RACE race)
{
    string race_name;

    ASSERT_ARGUMENT(race);
    ::create();
    race_ = race;
    race_name = lower_case(race->query_name());
    add_noun(race_name);
    add_noun("corpse");
    set_look(indefinite_article(race_name) + " " + race_name + " corpse");
}

object LIB_RACE query_race()
{
    return race_;
}
