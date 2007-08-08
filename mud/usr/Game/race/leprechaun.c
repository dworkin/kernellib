# include <game/attribute.h>
# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Leprechaun");
    add_noun("leprechaun");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    0.6,
                            SPEED_ATTRIBUTE:       1.3,
                            PERCEPTION_ATTRIBUTE:  1.2,
                            CHARISMA_ATTRIBUTE:    0.9 ]));
}
