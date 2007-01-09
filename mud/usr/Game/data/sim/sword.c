# include <system/assert.h>

int class_;

static void create(int class)
{
    class_ = class;
    call_out("test", 1, 42);
}

int query_class()
{
    return class_;
}

static void test(int arg)
{
    ASSERT(arg == 42);
    message("Tested sword with class " + class_ + ".");
}
