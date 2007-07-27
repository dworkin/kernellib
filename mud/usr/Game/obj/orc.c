# include <game/thing.h>

inherit LIB_CREATURE;

static void create()
{
    ::create();
    set_race("orc");
}
