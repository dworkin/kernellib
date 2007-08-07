/*
 * - prefers heavy armor
 * - prefers cave boars and mountain goats as mounts
 * - prefers tool weapons such as axes, hammers, and mattocks
 * - strong against trolls
 */

# include <game/attribute.h>
# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Dwarf");
    add_noun("dwarf", "dwarves");

    set_attribute_shares(([ STRENGTH_ATTRIBUTE:   1.3,
                            DEXTERITY_ATTRIBUTE:  0.8,
                            CHARISMA_ATTRIBUTE:   0.9,
                            WISDOM_ATTRIBUTE:     1.0 ]));
}
