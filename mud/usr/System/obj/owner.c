# include <status.h>
# include <system/assert.h>
# include <system/object.h>

int      next_onumber_;  /* next object number */
object   objectd_;       /* object manager */
mapping  data_;          /* ([ int onumber: object env ]) */

/*
 * NAME:        create()
 * DESCRIPTION: initialize owner object
 */
static void create(int clone)
{
    if (clone) {
	next_onumber_ = 2;
        objectd_ = find_object(OBJECTD);
	data_ = ([ ]);
    }
}

/*
 * NAME:        add_data()
 * DESCRIPTION: register an LWO for management
 */
int add_data(int uid, object env)
{
    int onumber;

    ASSERT_ACCESS(previous_object() == objectd_);
    DEBUG_ASSERT(uid);
    DEBUG_ASSERT(env);
    onumber = -(uid * 1000000 + next_onumber_++);
    data_[onumber] = env;
    return onumber;
}

/*
 * NAME:        find_data()
 * DESCRIPTION: find a managed LWO by number
 */
object find_data(int onumber)
{
    object env;

    ASSERT_ACCESS(previous_object() == objectd_);
    DEBUG_ASSERT(onumber <= -2);
    env = data_[onumber];
    return env ? env->_F_find(onumber) : nil;
}

/*
 * NAME:        move_data()
 * DESCRIPTION: move a managed LWO to another environment
 */
void move_data(int onumber, object env)
{
    ASSERT_ACCESS(previous_object() == objectd_);
    DEBUG_ASSERT(onumber <= -2 && data_[onumber]);
    data_[onumber] = env;
}

/*
 * NAME:        data_callout()
 * DESCRIPTION: schedule a call-out for a managed LWO
 */
int data_callout(int onumber, string func, mixed delay, mixed *args)
{
    ASSERT_ACCESS(previous_object() == objectd_);
    DEBUG_ASSERT(onumber);
    DEBUG_ASSERT(func);
    DEBUG_ASSERT(args);
    return call_out("call_data", delay, onumber, func, args);
}

/*
 * NAME:        remove_data_callout()
 * DESCRIPTION: remove a call-out for a managed LWO
 */
mixed remove_data_callout(int onumber, int handle)
{
    int      i, size;
    mixed  **callouts;

    ASSERT_ACCESS(previous_object() == objectd_);
    DEBUG_ASSERT(onumber);

    callouts = status(this_object())[O_CALLOUTS];
    for (i = sizeof(callouts) - 1; i >= 0; --i) {
        if (callouts[i][CO_HANDLE] == handle) {
            /*
             * Found the call-out. Remove it only if it belongs to the managed
             * LWO.
             */
            return (callouts[i][CO_FIRSTXARG] == onumber)
                ? remove_call_out(handle) : -1;
        }
    }

    return -1;
}

/*
 * NAME:        query_data_callouts()
 * DESCRIPTION: return the call-outs of a managed LWO
 */
mixed *query_data_callouts(string owner, int onumber)
{
    int      i, j, size, owned;
    mixed  **callouts;

    ASSERT_ACCESS(previous_object() == objectd_);

    /* filter call-outs by object number */
    callouts = status(this_object())[O_CALLOUTS];
    size = sizeof(callouts);
    owned = (owner && owner == query_owner());
    for (i = j = 0; i < size; ++i) {
        if (callouts[i][CO_FIRSTXARG] == onumber) {
            mixed *callout;

            callout = ({ callouts[i][CO_HANDLE],
                         callouts[i][CO_FIRSTXARG + 1],
                         callouts[i][CO_DELAY] });
            if (owned) {
                /* include arguments */
                callout += callouts[i][CO_FIRSTXARG + 2];
            }
            callouts[j++] = callout;
        }
    }
    return callouts[.. j - 1];
}

/*
 * NAME:        call_data()
 * DESCRIPTION: dispatch a call-out to a managed LWO
 */
static void call_data(int onumber, string func, mixed *args)
{
    object env;

    env = data_[onumber];
    if (env) {
        object obj;

        obj = env->_F_find(onumber);
        if (obj) {
            obj->_F_call_data(func, args);
        }
    }
}
