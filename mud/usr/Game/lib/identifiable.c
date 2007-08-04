# include <game/language.h>
# include <game/string.h>
# include <game/thing.h>

private inherit UTIL_LANGUAGE;
private inherit UTIL_STRING;

string   name_;
mapping  adjectives_;
mapping  singular_nouns_;
mapping  plural_nouns_;

static void create()
{
    adjectives_ = ([ ]);
    singular_nouns_ = ([ ]);
    plural_nouns_ = ([ ]);
}

static void add_adjective(string str)
{
    adjectives_[str] = TRUE;
}

static void add_singular_noun(string str)
{
    singular_nouns_[str] = TRUE;
}

static void remove_singular_noun(string str)
{
    singular_nouns_[str] = nil;
}

int has_singular_noun(string str)
{
    return singular_nouns_[str] || name_ && str == lower_case(name_);
}

static void add_plural_noun(string str)
{
    plural_nouns_[str] = TRUE;
}

static void remove_plural_noun(string str)
{
    plural_nouns_[str] = nil;
}

int has_plural_noun(string str)
{
    return !!plural_nouns_[str];
}

static void add_noun(string singular, varargs string plural)
{
    add_singular_noun(singular);
    add_plural_noun(plural ? plural : plural_form(singular));
}

int has_noun(string str)
{
    return has_singular_noun(str) || has_plural_noun(str);
}

void set_name(string name)
{
    name_ = name;
}

string query_name()
{
    return name_;
}

int singular_identify(string *words, varargs object LIB_CREATURE actor)
{
    int size;

    size = sizeof(words);
    if (!size || !has_singular_noun(words[size - 1])
        && words[size - 1] != name_)
    {
        return FALSE;
    }
    return !sizeof(words[.. size - 2] - map_indices(adjectives_)
                   - map_indices(singular_nouns_) - ({ name_ }));
}

int plural_identify(string *words, varargs object LIB_CREATURE actor)
{
    int size;

    size = sizeof(words);
    if (!size || !has_plural_noun(words[size - 1])
        && words[size - 1] != name_)
    {
        return FALSE;
    }
    return !sizeof(words[.. size - 2] - map_indices(adjectives_)
                   - map_indices(singular_nouns_) - ({ name_ }));
}

int identify(string *words, varargs object LIB_CREATURE actor)
{
    return singular_identify(words, actor) || plural_identify(words, actor);
}
