# include <game/thing.h>

inherit LIB_CREATURE;

static void create()
{
    ::create();
    add_noun("elf");
    set_look("an elf");
}
