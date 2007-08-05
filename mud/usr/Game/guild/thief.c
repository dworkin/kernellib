# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Thief");
    add_noun("thief", "thieves");

    set_trait_bonus("strength",      -1.0);
    set_trait_bonus("constitution",   0.0);
    set_trait_bonus("dexterity",      2.0);
    set_trait_bonus("charisma",       0.0);
    set_trait_bonus("intelligence",   0.0);
    set_trait_bonus("wisdom",        -1.0);
}
