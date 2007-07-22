# include <game/thing.h>

inherit LIB_THING;

mapping exits_;

static void create()
{
    exits_ = ([ ]);
}

static void set_exit(string dir, mixed dest)
{
    exits_[dir] = dest;
}

string query_exit(string dir)
{
    mixed dest;

    dest = exits_[dir];
    if (dest) {
        dest = find_object(dest);
    }
    return dest;
}

string *query_exits()
{
    return map_indices(exits_);
}
