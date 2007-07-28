# include <game/thing.h>

mapping adjectives_;
mapping singular_nouns_;
mapping plural_nouns_;
string name_;

static void create()
{
    adjectives_ = ([ ]);
    singular_nouns_ = ([ ]);
    plural_nouns_ = ([ ]);
}

static void add_adjective(string adj)
{
    adjectives_[adj] = TRUE;
}

static void add_singular_noun(string sing)
{
    singular_nouns_[sing] = TRUE;
}

static void add_plural_noun(string plur)
{
    plural_nouns_[plur] = TRUE;
}

static string plural_form(string sing)
{
    return sing + "s";
}

static void add_noun(string sing, varargs string plur)
{
    add_singular_noun(sing);
    add_plural_noun(plur ? plur : plural_form(sing));
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
    if (!size || !singular_nouns_[words[size - 1]] && words[size - 1] != name_)
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
    if (!size || !plural_nouns_[words[size - 1]] && words[size - 1] != name_) {
        return FALSE;
    }
    return !sizeof(words[.. size - 2] - map_indices(adjectives_)
                   - map_indices(singular_nouns_) - ({ name_ }));
}

int identify(string *words, varargs object LIB_CREATURE actor)
{
    int size;

    size = sizeof(words);
    if (!size
        || !singular_nouns_[words[size - 1]]
        && !plural_nouns_[words[size - 1]] && words[size - 1] != name_)
    {
        return FALSE;
    }
    return !sizeof(words[.. size - 2] - map_indices(adjectives_)
                   - map_indices(singular_nouns_) - ({ name_ }));
}
