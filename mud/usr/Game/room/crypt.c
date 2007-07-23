# include <game/thing.h>

inherit LIB_ROOM;

static void create()
{
    ::create();
    add_noun("crypt");
    set_look("a crypt");
    set_exit("up", "temple");
}
