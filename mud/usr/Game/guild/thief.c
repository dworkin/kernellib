# include <game/attribute.h>
# include <game/guild.h>

inherit LIB_GUILD;

static void create()
{
    ::create();
    set_name("Thief");
    add_noun("thief", "thieves");

    set_attribute_distribution(([ STRENGTH_ATTRIBUTE:   0.8,
                                  DEXTERITY_ATTRIBUTE:  1.3,
                                  CHARISMA_ATTRIBUTE:   0.9,
                                  WISDOM_ATTRIBUTE:     1.0 ]));
}
