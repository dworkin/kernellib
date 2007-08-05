# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Leprechaun");
    add_noun("leprechaun");

    set_trait_bonus("strength",      -2.0);
    set_trait_bonus("constitution",  -2.0);
    set_trait_bonus("dexterity",      1.0);
    set_trait_bonus("charisma",       1.0);
    set_trait_bonus("intelligence",   1.0);
    set_trait_bonus("wisdom",         1.0);
}
