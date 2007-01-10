# include <status.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/path.h>
# include <system/system.h>

private inherit path UTIL_PATH;

int      next_uid_;
mapping  uid_to_node_;
mapping  owner_to_node_;

static void create()
{
    next_uid_ = 1;
    uid_to_node_ = ([ ]);
    owner_to_node_ = ([ ]);
}

void compiling(string path)
{
    ASSERT_ACCESS(previous_program() == DRIVER);
}

string path_special(string compiled)
{
    ASSERT_ACCESS(previous_program() == DRIVER);
    return "/include/system/auto.h";
}

int forbid_inherit(string from, string path, int priv)
{
    ASSERT_ACCESS(previous_program() == DRIVER);

    /*
     * user objects cannot inherit system objects, except for objects in
     * ~System/open
     */
    if (path::creator(from) != "System" && path::creator(path) == "System"
        && !sscanf(path, "/usr/System/open/%*s"))
        {
            return TRUE;
        }

    /* forbid private inheritance of objects with undefined functions */
    if (priv) {
        mixed *status;
    
        status = status(path);
        if (status && status[O_UNDEFINED]) {
            DRIVER->message("Cannot privately inherit object with undefined "
                            + "functions\n");
            return TRUE;
        }
    }

    return FALSE;
}

private int oid_to_uid(int oid)
{
    return -oid / 1000000;
}

int new_sim(object obj)
{
    object  node;
    string  owner;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(obj);
    owner = obj->query_owner();
    node = owner_to_node_[owner];
    if (!node) {
	int uid;

	/* create node for owner */
	uid = next_uid_++;
	node = clone_object(OBJNODE, owner);
	node->set_uid(uid);
	uid_to_node_[uid] = node;
	owner_to_node_[owner] = node;
    }
    return node->new_sim(obj);
}

void destruct_sim(int oid)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    uid_to_node_[uid]->destruct_sim(oid);
}

object find_sim(int oid)
{
    int uid;

    ASSERT_ACCESS(previous_program() == PROXY
                  || previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    return uid_to_node_[uid]->find_sim(oid);
}

void move_sim(int oid, object obj)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(obj);
    uid = oid_to_uid(oid);
    uid_to_node_[uid]->move_sim(oid, obj);
}

int sim_callout(int oid, string func, mixed delay, mixed *args)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    DEBUG_ASSERT(uid_to_node_[uid]);
    return uid_to_node_[uid]->sim_callout(oid, func, delay, args);
}

mixed remove_sim_callout(int oid, int handle)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    DEBUG_ASSERT(uid_to_node_[uid]);
    return uid_to_node_[uid]->remove_sim_callout(oid, handle);
}

mixed *query_sim_callouts(string owner, int oid)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    DEBUG_ASSERT(uid_to_node_[uid]);
    return uid_to_node_[uid]->query_sim_callouts(owner, oid);
}
