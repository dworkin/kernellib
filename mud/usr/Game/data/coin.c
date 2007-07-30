# include <game/language.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_HEAP;
inherit UTIL_LANGUAGE;

string material_;

static void create(string material, varargs int count)
{
    ASSERT_ARG_1(material);
    ASSERT_ARG_2(count >= 0);
    ::create(count ? count : 1);
    material_ = material;
    add_noun(material);
    add_noun("coin");
}

string query_material()
{
    return material_;
}

string query_look(varargs object LIB_THING observer)
{
    int count;
   
    count = query_count();
    if (count == 1) {
        return indefinite_article(material_) + " " + material_ + " coin";
    } else {
        return format_count(count) + " " + material_ + " coins";
    }
}
