# include <game/attribute.h>

mapping attribute_distribution_;

static void create()
{
    attribute_distribution_ = ([ STRENGTH_ATTRIBUTE:   1.0,
                                 DEXTERITY_ATTRIBUTE:  1.0,
                                 CHARISMA_ATTRIBUTE:   1.0,
                                 WISDOM_ATTRIBUTE:     1.0 ]);
}

static void set_attribute_distribution(mapping attribute_distribution)
{
    attribute_distribution_ = attribute_distribution;
}

mapping query_attribute_distribution()
{
    return attribute_distribution_[..];
}
