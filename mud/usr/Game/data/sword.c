# include <game/thing.h>

inherit LIB_ITEM;

static void create()
{
    ::create();
    add_noun("sword");
    set_look("a sword");
}
