/*
 * - prefers heavy armor
 * - prefers cave boars and mountain goats as mounts
 * - prefers tool weapons such as axes, hammers, and mattocks
 * - strong against trolls
 */

# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Dwarf");
    add_noun("dwarf", "dwarves");

    set_trait_bonus("strength",       1.0);
    set_trait_bonus("constitution",   1.0);
    set_trait_bonus("dexterity",     -1.0);
    set_trait_bonus("charisma",      -1.0);
    set_trait_bonus("intelligence",   0.0);
    set_trait_bonus("wisdom",         0.0);
}
