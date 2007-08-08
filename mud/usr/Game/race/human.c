/*
 * - prefers horses as mounts
 */

# include <game/attribute.h>
# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Human");
    add_noun("human");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    1.0,
                            SPEED_ATTRIBUTE:       1.0,
                            PERCEPTION_ATTRIBUTE:  1.0,
                            CHARISMA_ATTRIBUTE:    1.0 ]));
}
