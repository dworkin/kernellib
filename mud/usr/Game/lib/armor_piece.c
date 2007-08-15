# include <game/armor.h>
# include <game/attribute.h>
# include <game/language.h>
# include <game/thing.h>
# include <system/assert.h>

inherit aff   LIB_ATTRIBUTE_AFFECTOR;
inherit item  LIB_ITEM;

private inherit UTIL_ARMOR;
private inherit UTIL_LANGUAGE;

string armor_type_;

static void set_armor_type(string armor_type)
{
    ASSERT_ARGUMENT(armor_type && is_armor_type(armor_type));
    armor_type_ = armor_type;
    add_noun(armor_type);
}

static void create()
{
    item::create();
    aff::create();
    set_armor_type(BODY_ARMOR_PIECE);
}

string query_armor_type()
{
    DEBUG_ASSERT(armor_type_);
    return armor_type_;
}

string query_look(varargs object LIB_THING observer)
{
    string look;

    look = ::query_look(observer);
    if (look) {
        return look;
    }

    switch (armor_type_) {
    case BOOT_PIECE:
    case GLOVE_PIECE:
        return "a pair of " + armor_type_ + "s";

    default:
        return indefinite_article(armor_type_) + " " + armor_type_;
    }
}

object LIB_CREATURE query_wearer()
{
    object LIB_THING environment;

    environment = environment(this_object());
    return environment && environment <- LIB_CREATURE
            && sizeof(environment->query_worn() & ({ this_object() }))
        ? environment : nil;
}

int move(object destination)
{
    return query_wearer() ? FALSE : ::move(destination);
}
