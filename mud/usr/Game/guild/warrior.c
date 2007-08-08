# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Warrior");
    add_noun("warrior");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    1.5,
                            SPEED_ATTRIBUTE:       1.1,
                            PERCEPTION_ATTRIBUTE:  0.7,
                            CHARISMA_ATTRIBUTE:    0.7 ]));
}
