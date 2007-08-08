/*
 * - prefers light armor
 * - strong against dwarves
 * - prefers wolves as mounts
 */

# include <game/attribute.h>
# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Goblin");
    add_noun("goblin");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    1.2,
                            SPEED_ATTRIBUTE:       1.1,
                            PERCEPTION_ATTRIBUTE:  1.0,
                            CHARISMA_ATTRIBUTE:    0.7 ]));
}
