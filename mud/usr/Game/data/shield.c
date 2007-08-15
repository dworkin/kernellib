# include <game/armor.h>
# include <game/attribute.h>

inherit LIB_ARMOR_PIECE;

static void create()
{
    ::create();
    set_armor_type(SHIELD_PIECE);

    set_attribute_bonus(DEFENSE_ATTRIBUTE, 2.0);
    set_attribute_bonus(PROTECTION_ATTRIBUTE, 2.0);
}
