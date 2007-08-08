# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Priest");
    add_noun("priest");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    0.8,
                            SPEED_ATTRIBUTE:       0.7,
                            PERCEPTION_ATTRIBUTE:  1.2,
                            CHARISMA_ATTRIBUTE:    1.3 ]));
}
