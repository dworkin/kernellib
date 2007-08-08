# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Wizard");
    add_noun("wizard");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    0.6,
                            SPEED_ATTRIBUTE:       0.8,
                            PERCEPTION_ATTRIBUTE:  1.5,
                            CHARISMA_ATTRIBUTE:    1.1 ]));
}
