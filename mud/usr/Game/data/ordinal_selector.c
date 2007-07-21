# include <game/selector.h>
# include <game/thing.h>

inherit LIB_SELECTOR;

int     ord_;
string *words_;

static void create(int ord, string *words)
{
    ord_ = ord;
    words_ = words;
}

object LIB_THING *select(object LIB_THING *objs,
                         varargs object LIB_CREATURE actor)
{
    int i, j, size;

    size = sizeof(objs);
    for (i = j = 0; i < size; ++i) {
        if (objs[i]->identify(words_, actor) && ++j == ord_) {
            return ({ objs[i] });
        }
    }
    return ({ });
}
