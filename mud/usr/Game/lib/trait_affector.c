# include <game/thing.h>

mapping trait_bonuses_;

static void create()
{
    trait_bonuses_ = ([ ]);
}

static void set_trait_bonus(string name, float bonus)
{
    trait_bonuses_[name] = bonus ? bonus : nil;
}

float affect_trait(object LIB_CREATURE creature, string name)
{
    return trait_bonuses_[name] ? trait_bonuses_[name] : 0.0;
}
