# include <game/identity.h>

inherit LIB_IDENTIFIABLE;

mapping trait_bonuses_;

static void create()
{
    ::create();
    trait_bonuses_ = ([ ]);
}

static void set_trait_bonus(string name, float bonus)
{
    trait_bonuses_[name] = bonus ? bonus : nil;
}

float query_trait_bonus(string name)
{
    return trait_bonuses_[name] ? trait_bonuses_[name] : 0.0;
}
