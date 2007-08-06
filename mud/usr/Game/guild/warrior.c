# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Warrior");
    add_noun("warrior");

    set_attribute_distribution(([ STRENGTH_ATTRIBUTE:   1.3,
                                  DEXTERITY_ATTRIBUTE:  1.0,
                                  CHARISMA_ATTRIBUTE:   0.9,
                                  WISDOM_ATTRIBUTE:     0.8 ]));
}
