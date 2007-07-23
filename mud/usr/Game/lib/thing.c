# include <game/description.h>
# include <game/identity.h>
# include <game/message.h>

inherit desc LIB_DESCRIBABLE;
inherit id LIB_IDENTIFIABLE;
inherit LIB_OBSERVER;

static void create()
{
    desc::create();
    id::create();
}
