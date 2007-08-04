# include <game/race.h>
# include <game/thing.h>

inherit LIB_CREATURE;

static void create()
{
    ::create();
    set_race(find_object(DWARF_RACE));
}
