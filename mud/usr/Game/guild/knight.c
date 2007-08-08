/*
 * A knight
 * - prefers shields and heavy armor
 * - prefers swords
 * - is strong against giants and monsters such as dragons
 * - is a skilled rider.
 */

# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Knight");
    add_noun("knight");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:    1.4,
                            SPEED_ATTRIBUTE:       0.8,
                            PERCEPTION_ATTRIBUTE:  0.7,
                            CHARISMA_ATTRIBUTE:    1.1 ]));
}
