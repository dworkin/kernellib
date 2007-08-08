/*
 * Rangers
 * - are stealthy
 * - are skilled at setting and disarming traps
 * - are skilled trackers
 * - prefer hunting weapons such as spears, knives, and bows
 * - prefer light armor.
 */

# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Ranger");
    add_noun("ranger");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    0.9,
                            SPEED_ATTRIBUTE:       1.4,
                            PERCEPTION_ATTRIBUTE:  1.2,
                            CHARISMA_ATTRIBUTE:    0.5 ]));
}
