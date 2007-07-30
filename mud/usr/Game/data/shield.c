# include <game/armor.h>

inherit LIB_ARMOR_PIECE;

static void create()
{
    ::create();
    set_armor_type("shield");
}
