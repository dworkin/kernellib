# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Monk");
    add_noun("monk");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    1.0,
                            SPEED_ATTRIBUTE:       1.0,
                            PERCEPTION_ATTRIBUTE:  1.0,
                            CHARISMA_ATTRIBUTE:    1.0 ]));
}
