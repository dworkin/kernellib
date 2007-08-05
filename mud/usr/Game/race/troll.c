# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Troll");
    add_noun("troll");

    set_trait_bonus("strength",       2.0);
    set_trait_bonus("constitution",   1.0);
    set_trait_bonus("dexterity",     -1.0);
    set_trait_bonus("charisma",      -2.0);
    set_trait_bonus("intelligence",   0.0);
    set_trait_bonus("wisdom",         0.0);
}
