# include <game/thing.h>

inherit LIB_WEAPON;

static void create()
{
    ::create();
    add_noun("sword");
    set_look("a sword");

    set_attribute_bonus("attack", 2.0);
    set_attribute_bonus("defense", 1.0);
    set_attribute_bonus("damage", 2.0);
}
