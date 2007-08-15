# include <game/attribute.h>
# include <game/thing.h>

inherit LIB_WEAPON;

static void create()
{
    ::create();
    add_noun("sword");
    set_description("a sword");

    set_attribute_bonus(ATTACK_ATTRIBUTE,   2.0);
    set_attribute_bonus(DEFENSE_ATTRIBUTE,  1.0);
    set_attribute_bonus(DAMAGE_ATTRIBUTE,   2.0);
}
