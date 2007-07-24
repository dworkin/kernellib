# include <game/selector.h>
# include <game/thing.h>

inherit LIB_SELECTOR;

object LIB_SELECTOR *sels_;

static void create(object LIB_SELECTOR *sels)
{
    sels_ = sels;
}

object LIB_THING *select(object LIB_THING *objs,
                         varargs object LIB_CREATURE actor)
{
    object LIB_THING *res;
    int i, size;

    res = ({ });
    size = sizeof(sels_);
    for (i = 0; i < size; ++i) {
        res |= sels_[i]->select(objs, actor);
    }
    return res;
}
