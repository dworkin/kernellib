# include <game/message.h>

string description_;
string verbose_description_;

static void create() { }

static void set_description(string description)
{
    description_ = description;
}

static void set_verbose_description(string verbose_description)
{
    verbose_description_ = verbose_description;
}

string describe(varargs object LIB_OBSERVER observer)
{
    return description_;
}

string describe_verbose(varargs object LIB_OBSERVER observer)
{
    return verbose_description_;
}
