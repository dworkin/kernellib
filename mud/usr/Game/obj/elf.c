# include <game/thing.h>

inherit LIB_CREATURE;

int level_;

static void create(int level)
{
    level_ = level;
}

int query_level()
{
    return level_;
}
