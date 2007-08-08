# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Bard");
    add_noun("bard");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    0.9,
                            SPEED_ATTRIBUTE:       0.9,
                            PERCEPTION_ATTRIBUTE:  0.7,
                            CHARISMA_ATTRIBUTE:    1.5 ]));
}
