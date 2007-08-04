# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Warrior");
    add_noun("warrior");

    set_trait_bonus("attack",      1.0);
    set_trait_bonus("charisma",   -1.0);
    set_trait_bonus("damage",      1.0);
    set_trait_bonus("defense",     1.0);
    set_trait_bonus("dexterity",  -1.0);
}
