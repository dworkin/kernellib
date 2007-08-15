# include <game/thing.h>

inherit LIB_ROOM;

static void create()
{
    ::create();
    add_noun("crypt");
    set_description("a crypt");
    set_exit("up", "temple");
}
