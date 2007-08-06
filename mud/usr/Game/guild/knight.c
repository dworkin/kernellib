/*
 * A knight
 * - prefers shields and heavy armor
 * - prefers swords
 * - is strong against giants and monsters such as dragons
 * - is a skilled rider.
 */

# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Knight");
    add_noun("knight");

    set_trait_bonus("strength",       1.0);
    set_trait_bonus("constitution",   1.0);
    set_trait_bonus("dexterity",     -2.0);
    set_trait_bonus("charisma",       1.0);
    set_trait_bonus("intelligence",  -1.0);
    set_trait_bonus("wisdom",         0.0);
}
