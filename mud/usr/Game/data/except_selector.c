# include <game/selector.h>
# include <game/thing.h>

inherit LIB_SELECTOR;

object LIB_SELECTOR incl_;
object LIB_SELECTOR excl_;

static void construct(object LIB_SELECTOR incl, object LIB_SELECTOR excl)
{
    incl_ = incl;
    excl_ = excl;
}

object LIB_THING *select(object LIB_THING *objs,
                         varargs object LIB_CREATURE actor)
{
    return incl_->select(actor, objs) - excl_->select(objs, actor);
}
