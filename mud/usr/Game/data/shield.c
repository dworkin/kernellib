# include <game/armor.h>

inherit LIB_ARMOR_PIECE;

static void create()
{
    ::create();
    set_armor_type("shield");
    set_trait_bonus("defense", 2.0);
    set_trait_bonus("protection", 2.0);
}
