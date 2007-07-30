# include <game/thing.h>

inherit LIB_WEAPON;

static void create()
{
    ::create();
    add_noun("sword");
    set_look("a sword");
}
