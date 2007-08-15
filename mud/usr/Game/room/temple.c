# include <game/thing.h>

inherit LIB_ROOM;

static void create()
{
    ::create();
    add_noun("temple");
    set_description("a temple");
    set_verbose_description("The temple is a gateway to other worlds. A stair "
                            + "leads down.");
    set_exit("down", "crypt");
}
