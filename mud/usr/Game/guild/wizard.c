# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Wizard");
    add_noun("wizard");

    set_trait_bonus("strength",      -1.0);
    set_trait_bonus("constitution",  -1.0);
    set_trait_bonus("dexterity",     -1.0);
    set_trait_bonus("charisma",       0.0);
    set_trait_bonus("intelligence",   2.0);
    set_trait_bonus("wisdom",         1.0);
}
