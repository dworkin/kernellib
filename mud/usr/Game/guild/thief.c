# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Thief");
    add_noun("thief", "thieves");
}
