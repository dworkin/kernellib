# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Goblin");
    add_noun("goblin");

    set_trait_bonus("strength",       0.0);
    set_trait_bonus("constitution",   0.0);
    set_trait_bonus("dexterity",      1.0);
    set_trait_bonus("charisma",      -1.0);
    set_trait_bonus("intelligence",   0.0);
    set_trait_bonus("wisdom",         0.0);
}
