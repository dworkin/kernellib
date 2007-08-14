# include <game/thing.h>
# include <system/assert.h>

inherit LIB_ITEM;

int count_;

static void create(int count)
{
    ASSERT_ARG(count >= 1);
    ::create();
    count_ = count;
}

atomic object LIB_HEAP split(int count)
{
    object LIB_HEAP heap;

    ASSERT_ARG(count < count_);
    heap = new_object(this_object());
    count_ -= count;
    heap->set_count(count);
    heap->move(environment(this_object()));
}

atomic void merge(object LIB_HEAP heap)
{
    ASSERT_ARG(heap);
    count_ += heap->query_count();
    heap->move(nil);
}

void set_count(int count)
{
    ASSERT_ARG(count >= 1);
    count_ = count;
}

int query_count()
{
    return count_;
}
