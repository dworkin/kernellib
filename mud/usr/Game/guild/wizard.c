# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Wizard");
    add_noun("wizard");

    set_attribute_distribution(([ STRENGTH_ATTRIBUTE:   0.7,
                                  DEXTERITY_ATTRIBUTE:  0.9,
                                  CHARISMA_ATTRIBUTE:   1.0,
                                  WISDOM_ATTRIBUTE:     1.3 ]));
}
