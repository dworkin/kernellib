# include <game/actor.h>

mapping adj_;
mapping sing_;
mapping plur_;

static void create()
{
    adj_ = ([ ]);
    sing_ = ([ ]);
    plur_ = ([ ]);
}

static void add_adjective(string adj)
{
    adj_[adj] = TRUE;
}

static void add_singular_noun(string sing)
{
    sing_[sing] = TRUE;
}

static void add_plural_noun(string plur)
{
    plur_[plur] = TRUE;
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

int singular_identify(string *words, varargs object LIB_ACTOR actor)
{
    int size;

    size = sizeof(words);
    if (!size || !sing_[words[size - 1]]) {
        return FALSE;
    }
    return !sizeof(words[.. size - 2] - map_indices(adj_)
                   - map_indices(sing_));
}

int plural_identify(string *words, varargs object LIB_ACTOR actor)
{
    int size;

    size = sizeof(words);
    if (!size || !plur_[words[size - 1]]) {
        return FALSE;
    }
    return !sizeof(words[.. size - 2] - map_indices(adj_)
                   - map_indices(sing_));
}

int identify(string *words, varargs object LIB_ACTOR actor)
{
    int size;

    size = sizeof(words);
    if (!size || !sing_[words[size - 1]] && !plur_[words[size - 1]]) {
        return FALSE;
    }
    return !sizeof(words[.. size - 2] - map_indices(adj_)
                   - map_indices(sing_));
}
