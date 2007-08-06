# include <game/attribute.h>
# include <game/identity.h>

inherit id    LIB_IDENTIFIABLE;
inherit aff   LIB_ATTRIBUTE_AFFECTOR;
inherit dist  LIB_ATTRIBUTE_DISTRIBUTOR;

static void create()
{
    id::create();
    aff::create();
    dist::create();
}
