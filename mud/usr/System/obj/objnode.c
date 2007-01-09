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
    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(oid);

    /* TODO: remove call-out only for the specified object */
    return remove_call_out(handle);
}

mixed *query_sim_callouts(int oid)
{
    ASSERT_ACCESS(previous_program() == OBJECTD);

    /* TODO: to be implemented */
    return ({ });
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
