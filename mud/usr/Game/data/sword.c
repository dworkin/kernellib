# include <status.h>
# include <system/assert.h>

int class_;

static void create(int class)
{
    class_ = class;
}

int query_class()
{
    return class_;
}

void start()
{
    ASSERT(!sizeof(status(this_object())[O_CALLOUTS]));
    call_out("stop", 0, this_object());
    ASSERT(sizeof(status(this_object())[O_CALLOUTS]) == 1);
}

static void stop(object obj)
{
    ASSERT(!sizeof(status(this_object())[O_CALLOUTS]));
    ASSERT(obj != this_object());
}
