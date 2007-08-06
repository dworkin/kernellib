# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Priest");
    add_noun("priest");

    set_attribute_distribution(([ STRENGTH_ATTRIBUTE:   0.9,
                                  DEXTERITY_ATTRIBUTE:  0.8,
                                  CHARISMA_ATTRIBUTE:   1.2,
                                  WISDOM_ATTRIBUTE:     1.1 ]));
}
