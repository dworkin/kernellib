# include <game/thing.h>

string look_;   /* brief look */
string vlook_;  /* verbose look */

static void create()
{
    look_ = nil;
    vlook_ = nil;
}

static void set_look(string look)
{
    look_ = look;
}

string query_look(varargs object LIB_THING observer)
{
    return look_;
}

static void set_verbose_look(string vlook)
{
    vlook_ = vlook;
}

string query_verbose_look(varargs object LIB_THING observer)
{
    return vlook_;
}
