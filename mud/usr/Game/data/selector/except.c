# include <game/selector.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_SELECTOR;

object LIB_SELECTOR incl_;
object LIB_SELECTOR excl_;

static void create(object LIB_SELECTOR incl, object LIB_SELECTOR excl)
{
    ASSERT_ARG_1(incl);
    ASSERT_ARG_2(excl);
    incl_ = incl;
    excl_ = excl;
}

object LIB_THING *select(object LIB_THING *objs,
                         varargs object LIB_CREATURE actor)
{
    return incl_->select(objs, actor) - excl_->select(objs, actor);
}
