# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Priest");
    add_noun("priest");

    set_trait_bonus("strength",       0.0);
    set_trait_bonus("constitution",  -1.0);
    set_trait_bonus("dexterity",     -1.0);
    set_trait_bonus("charisma",       1.0);
    set_trait_bonus("intelligence",  -1.0);
    set_trait_bonus("wisdom",         2.0);
}
