# include <game/race.h>

inherit LIB_RACE;

static void create()
{
    ::create();
    set_name("Human");
    add_noun("human");

    set_trait_bonus("intelligence",   1.0);
    set_trait_bonus("wisdom",        -1.0);
}
