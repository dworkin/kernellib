# include <status.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/path.h>
# include <system/system.h>

private inherit path UTIL_PATH;

int      uid_;
int      next_oid_;
mapping  sims_;

static void create(int clone)
{
    if (clone) {
	next_oid_ = 1;
	sims_ = ([ ]);
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

int new_sim(object obj)
{
    int oid;

    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(uid_);
    oid = -(uid_ * 1000000 + next_oid_++);
    sims_[oid] = obj;
    return oid;
}

void destruct_sim(int oid)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(oid && sims_[oid]);
    sims_[oid] = nil;
}

object find_sim(int oid)
{
    object obj;

    ASSERT_ACCESS(previous_program() == OBJECTD);
    obj = sims_[oid];
    if (obj && path::number(object_name(obj)) != -1) {
	/* find simulated object in environment */
	obj = obj->_F_find(oid);
    }
    return obj;
}

void move_sim(int oid, object obj)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(oid && sims_[oid]);
    DEBUG_ASSERT(obj);
    sims_[oid] = obj;
}

int sim_callout(int oid, string func, mixed delay, mixed *args)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(func);
    DEBUG_ASSERT(args);
    return call_out("call_sim", delay, oid, func, args);
}

mixed remove_sim_callout(int oid, int handle)
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

mixed *query_sim_callouts(string owner, int oid)
{
    int      i, j, size, owns;
    mixed  **callouts;

    ASSERT_ACCESS(previous_program() == OBJECTD);

    /* filter call-outs by OID */
    callouts = status(this_object())[O_CALLOUTS];
    size = sizeof(callouts);
    owns = (owner && owner == query_owner());
    for (i = j = 0; i < size; ++i) {
        if (callouts[i][CO_FIRSTXARG] == oid) {
            callouts[j] = ({ callouts[i][CO_HANDLE],
                             callouts[i][CO_FIRSTXARG + 1],
                             callouts[i][CO_DELAY] });
            if (owns) {
                callouts[j] += callouts[i][CO_FIRSTXARG + 2];
            }
            ++j;
        }
    }
    return callouts[.. j - 1];
}

static void call_sim(int oid, string func, mixed *args)
{
    object obj;

    obj = sims_[oid];
    if (obj && path::number(object_name(obj)) != -1) {
	/* find simulated object in environment */
	obj = obj->_F_find(oid);
    }
    if (obj) {
        obj->_F_call(func, args);
    }
}
