# include <game/description.h>
# include <game/language.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ITEM;
inherit UTIL_DESCRIPTION;
inherit UTIL_LANGUAGE;

static void create(string race)
{
    ASSERT_ARG(race);
    ::create();
    add_noun(race);
    add_noun("corpse");
    set_look(indefinite_article(race) + " " + race + " corpse");
}
