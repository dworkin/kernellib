# include <game/thing.h>

inherit LIB_THING;

mapping exits_;

static void create()
{
    ::create();
    exits_ = ([ ]);
}

static void set_exit(string dir, mixed dest)
{
    exits_[dir] = dest;
}

object query_exit(string dir)
{
    string dest;

    dest = exits_[dir];
    return dest ? find_object(dest) : nil;
}

string *query_exits()
{
    return map_indices(exits_);
}

int allow_enter(object obj)
{
    return obj <- LIB_THING;
}
