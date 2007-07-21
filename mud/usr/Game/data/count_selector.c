# include <game/selector.h>
# include <game/thing.h>

inherit LIB_SELECTOR;

int     count_;
string *words_;

static void create(int count, string *words)
{
    count_ = count;
    words_ = words;
}

object LIB_THING *select(object LIB_THING *objs,
                         varargs object LIB_CREATURE actor)
{
    int i, j, size;
    object LIB_THING *res;

    size = sizeof(objs);
    res = allocate(size);
    for (i = j = 0; j < count_ && i < size; ++i) {
        if (objs[i]->identify(words_, actor)) {
            res[j++] = objs[i];
        }
    }
    return res[.. j - 1];
}
