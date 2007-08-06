# include <game/thing.h>

inherit LIB_WEAPON;

static void create()
{
    ::create();
    add_noun("sword");
    set_look("a sword");

    set_trait_bonus("attack", 2.0);
    set_trait_bonus("defense", 1.0);
    set_trait_bonus("damage", 2.0);
}
