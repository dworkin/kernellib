# include <game/selector.h>
# include <game/thing.h>

inherit LIB_SELECTOR;

int      ordinal_;
string  *words_;

static void create(int ordinal, string *words)
{
    ordinal_ = ordinal;
    words_ = words;
}

object LIB_THING *select(object LIB_THING *objs,
                         varargs object LIB_CREATURE actor)
{
    int i, j, size;

    size = sizeof(objs);
    for (i = j = 0; i < size; ++i) {
        if (objs[i]->identify(words_, actor) && ++j == ordinal_) {
            return ({ objs[i] });
        }
    }
    return ({ });
}
