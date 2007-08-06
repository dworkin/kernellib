# include <game/identity.h>
# include <game/trait.h>

inherit id   LIB_IDENTIFIABLE;
inherit aff  LIB_TRAIT_AFFECTOR;

static void create()
{
    id::create();
    aff::create();
}
