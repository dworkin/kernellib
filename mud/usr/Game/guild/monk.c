# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Monk");
    add_noun("monk");

    set_attribute_distribution(([ STRENGTH_ATTRIBUTE:   1.0,
                                  DEXTERITY_ATTRIBUTE:  1.0,
                                  CHARISMA_ATTRIBUTE:   1.0,
                                  WISDOM_ATTRIBUTE:     1.0 ]));
}
