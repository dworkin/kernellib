# include <game/thing.h>

inherit LIB_CREATURE;

static void create()
{
    ::create();
    add_noun("orc");
    set_look("an orc");
}
