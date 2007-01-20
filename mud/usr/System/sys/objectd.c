# include <status.h>
# include <kernel/kernel.h>
# include <kernel/tls.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/system.h>

private inherit tls API_TLS;

int      nextuid;    /* next user ID */
object   driver;     /* driver object */
mapping  uids;       /* ([ string owner: int uid ]) */
mapping  ownerobjs;  /* ([ int uid: object ownerobj ]) */

/*
 * NAME:        create()
 * DESCRIPTION: initialize object manager
 */
static void create()
{
    tls::create();
    nextuid = 1;
    driver = find_object(DRIVER);
    uids = ([ ]);
    ownerobjs = ([ ]);
}

/*
 * NAME:        path_special()
 * DESCRIPTION: returns an include path that depends on the compiled path
 */
string path_special(string compiled)
{
    ASSERT_ACCESS(previous_object() == driver);
    return "/include/system/auto.h";
}

/*
 * NAME:        forbid_inherit()
 * DESCRIPTION: returns true if an inheritance is forbidden
 */
int forbid_inherit(string from, string path, int priv)
{
    ASSERT_ACCESS(previous_object() == driver);

    /*
     * user objects cannot inherit system objects, except for objects in
     * ~System/open
     */
    if (driver->creator(from) != "System" && driver->creator(path) == "System"
        && !sscanf(path, "/usr/System/open/%*s"))
    {
        return TRUE;
    }

    /* forbid private inheritance of objects with undefined functions */
    if (priv) {
        mixed *status;
    
        status = status(path);
        if (status && status[O_UNDEFINED]) {
            driver->message("Abstract object cannot be privately "
                            + "inherited\n");
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * NAME:        get_tlvar()
 * DESCRIPTION: proxies TLS reads for the system auto object
 */
mixed get_tlvar(int index)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    return tls::get_tlvar(index);
}

/*
 * NAME:        set_tlvar()
 * DESCRIPTION: proxies TLS writes for the system auto object
 */
void set_tlvar(int index, mixed value)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    tls::set_tlvar(index, value);
}

/*
 * NAME:        uid()
 * DESCRIPTION: return user ID for object ID
 */
private int uid(int oid)
{
    return -oid / 1000000;
}

/*
 * NAME:        add_data()
 * DESCRIPTION: add a managed LWO
 */
int add_data(string owner, object env)
{
    mixed   uid;
    object  ownerobj;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(env);
    uid = uids[owner];
    if (uid) {
        ownerobj = ownerobjs[uid];
        DEBUG_ASSERT(ownerobj);
    } else {
	/* create owner object for owner */
	uid = uids[owner] = nextuid++;
        ownerobj = ownerobjs[uid] = clone_object(OWNEROBJ, owner);
    }
    return ownerobj->add_data(uid, env);
}

/*
 * NAME:        find_data()
 * DESCRIPTION: find a managed LWO
 */
object find_data(int oid)
{
    int     uid;
    object  ownerobj;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid <= -2);
    uid = uid(oid);
    ownerobj = ownerobjs[uid];
    return ownerobj ? ownerobj->find_data(oid) : nil;
}

/*
 * NAME:        move_data()
 * DESCRIPTION: move or remove a managed LWO
 */
void move_data(int oid, object env)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid <= -2);
    DEBUG_ASSERT(env);
    uid = uid(oid);
    DEBUG_ASSERT(ownerobjs[uid]);
    ownerobjs[uid]->move_data(oid, env);
}

/*
 * NAME:        data_callout()
 * DESCRIPTION: make a call-out for a managed LWO
 */
int data_callout(int oid, string func, mixed delay, mixed *args)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid <= -2);
    uid = uid(oid);
    DEBUG_ASSERT(ownerobjs[uid]);
    return ownerobjs[uid]->data_callout(oid, func, delay, args);
}

/*
 * NAME:        remove_data_callout()
 * DESCRIPTION: remove a call-out for a managed LWO
 */
mixed remove_data_callout(int oid, int handle)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid <= -2);
    uid = uid(oid);
    DEBUG_ASSERT(ownerobjs[uid]);
    return ownerobjs[uid]->remove_data_callout(oid, handle);
}

/*
 * NAME:        query_data_callouts()
 * DESCRIPTION: return the call-outs for a managed LWO
 */
mixed *query_data_callouts(string owner, int oid)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid <= -2);
    uid = uid(oid);
    DEBUG_ASSERT(ownerobjs[uid]);
    return ownerobjs[uid]->query_data_callouts(owner, oid);
}
