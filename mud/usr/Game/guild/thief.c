# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Thief");
    add_noun("thief", "thieves");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    0.6,
                            SPEED_ATTRIBUTE:       1.5,
                            PERCEPTION_ATTRIBUTE:  1.2,
                            CHARISMA_ATTRIBUTE:    0.7 ]));
}
