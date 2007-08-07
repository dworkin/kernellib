# include <game/attribute.h>

mapping attribute_shares_;

static void create()
{
    attribute_shares_ = ([ STRENGTH_ATTRIBUTE:   1.0,
                           DEXTERITY_ATTRIBUTE:  1.0,
                           CHARISMA_ATTRIBUTE:   1.0,
                           WISDOM_ATTRIBUTE:     1.0 ]);
}

static void set_attribute_shares(mapping shares)
{
    attribute_shares_ = shares;
}

mapping query_attribute_shares()
{
    return attribute_shares_[..];
}
