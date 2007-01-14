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
            DRIVER->message("Abstract object cannot be privately "
                            + "inherited\n");
            return TRUE;
        }
    }

    return FALSE;
}

private int oid_to_uid(int oid)
{
    return -oid / 1000000;
}

int add_data(string owner, object env)
{
    object  node;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(env);
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
    return node->add_data(env);
}

object find_data(int oid)
{
    int     uid;
    object  node;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    node = uid_to_node_[uid];
    return node ? node->find_data(oid) : nil;
}

void move_data(int oid, object env)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(env);
    uid = oid_to_uid(oid);
    uid_to_node_[uid]->move_data(oid, env);
}

int data_callout(int oid, string func, mixed delay, mixed *args)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    DEBUG_ASSERT(uid_to_node_[uid]);
    return uid_to_node_[uid]->data_callout(oid, func, delay, args);
}

mixed remove_data_callout(int oid, int handle)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    DEBUG_ASSERT(uid_to_node_[uid]);
    return uid_to_node_[uid]->remove_data_callout(oid, handle);
}

mixed *query_data_callouts(string owner, int oid)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    uid = oid_to_uid(oid);
    DEBUG_ASSERT(uid_to_node_[uid]);
    return uid_to_node_[uid]->query_data_callouts(owner, oid);
}
