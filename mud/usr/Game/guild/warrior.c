# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Warrior");
    add_noun("warrior");

    set_trait_bonus("strength",       1.0);
    set_trait_bonus("constitution",   1.0);
    set_trait_bonus("dexterity",      0.0);
    set_trait_bonus("charisma",       0.0);
    set_trait_bonus("intelligence",  -1.0);
    set_trait_bonus("wisdom",        -1.0);
}
