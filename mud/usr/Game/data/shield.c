# include <game/thing.h>

inherit LIB_ITEM;

static void create()
{
    ::create();
    add_noun("shield");
    set_look("a shield");
}
