# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Bard");
    add_noun("bard");

    set_attribute_distribution(([ STRENGTH_ATTRIBUTE:   0.8,
                                  DEXTERITY_ATTRIBUTE:  1.0,
                                  CHARISMA_ATTRIBUTE:   1.3,
                                  WISDOM_ATTRIBUTE:     0.9 ]));
}
