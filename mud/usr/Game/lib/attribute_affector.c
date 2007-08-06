# include <game/thing.h>

mapping attribute_bonuses_;

static void create()
{
    attribute_bonuses_ = ([ ]);
}

static void set_attribute_bonus(string name, float bonus)
{
    attribute_bonuses_[name] = bonus ? bonus : nil;
}

float affect_attribute(object LIB_CREATURE creature, string name)
{
    return attribute_bonuses_[name] ? attribute_bonuses_[name] : 0.0;
}
