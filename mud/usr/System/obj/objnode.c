# include <status.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/path.h>
# include <system/system.h>

private inherit path UTIL_PATH;

int      uid_;
int      next_oid_;
mapping  data_;

static void create(int clone)
{
    if (clone) {
	next_oid_ = 1;
	data_ = ([ ]);
    }
}

void set_uid(int uid)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    uid_ = uid;
}

int query_uid()
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    return uid_;
}

int add_data(object env)
{
    int oid;

    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(uid_);
    DEBUG_ASSERT(env);
    oid = -(uid_ * 1000000 + next_oid_++);
    data_[oid] = env;
    return oid;
}

object find_data(int oid)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    return data_[oid] ? data_[oid]->_F_find(oid) : nil;
}

void move_data(int oid, object env)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(oid && data_[oid]);
    data_[oid] = env;
}

int data_callout(int oid, string func, mixed delay, mixed *args)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(func);
    DEBUG_ASSERT(args);
    return call_out("call_data", delay, oid, func, args);
}

mixed remove_data_callout(int oid, int handle)
{
    int      i, size;
    mixed  **callouts;

    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(oid);

    callouts = status(this_object())[O_CALLOUTS];
    for (i = sizeof(callouts) - 1; i >= 0; --i) {
        if (callouts[i][CO_HANDLE] == handle) {
            return (callouts[i][CO_FIRSTXARG] == oid)
                ? remove_call_out(handle) : -1;
        }
    }

    return -1;
}

mixed *query_data_callouts(string owner, int oid)
{
    int      i, j, size, owned;
    mixed  **callouts;

    ASSERT_ACCESS(previous_program() == OBJECTD);

    /* filter call-outs by OID */
    callouts = status(this_object())[O_CALLOUTS];
    size = sizeof(callouts);
    owned = (owner && owner == query_owner());
    for (i = j = 0; i < size; ++i) {
        if (callouts[i][CO_FIRSTXARG] == oid) {
            mixed *callout;

            callout = ({ callouts[i][CO_HANDLE],
                         callouts[i][CO_FIRSTXARG + 1],
                         callouts[i][CO_DELAY] });
            if (owned) {
                callout += callouts[i][CO_FIRSTXARG + 2];
            }
            callouts[j++] = callout;
        }
    }
    return callouts[.. j - 1];
}

static void call_data(int oid, string func, mixed *args)
{
    object obj;

    obj = data_[oid] ? data_[oid]->_F_find(oid) : nil;
    if (obj) {
        obj->_F_call(func, args);
    }
}
