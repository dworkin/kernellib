# include <status.h>
# include <kernel/kernel.h>
# include <kernel/tls.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/system.h>

private inherit tls   API_TLS;

int      next_uid_;   /* next user ID */
object   driver_;     /* driver object */
mapping  uids_;       /* ([ string owner: int uid ]) */
mapping  ownerobjs_;  /* ([ int uid: object ownerobj ]) */

/*
 * NAME:        create()
 * DESCRIPTION: initialize object manager
 */
static void create()
{
    tls::create();
    next_uid_ = 1;
    driver_ = find_object(DRIVER);
    uids_ = ([ ]);
    ownerobjs_ = ([ ]);
}

/*
 * NAME:        path_special()
 * DESCRIPTION: returns an include path that depends on the compiled path
 */
string path_special(string compiled)
{
    ASSERT_ACCESS(previous_object() == driver_);
    return "/include/system/auto.h";
}

/*
 * NAME:        forbid_inherit()
 * DESCRIPTION: returns true if an inheritance is forbidden
 */
int forbid_inherit(string from, string path, int priv)
{
    ASSERT_ACCESS(previous_object() == driver_);

    /*
     * user objects cannot inherit system objects, except for objects in
     * ~System/open
     */
    if (driver_->creator(from) != "System"
        && driver_->creator(path) == "System"
        && !sscanf(path, "/usr/System/open/%*s"))
    {
        return TRUE;
    }

    /* forbid private inheritance of objects with undefined functions */
    if (priv) {
        mixed *status;
    
        status = status(path);
        if (status && status[O_UNDEFINED]) {
            driver_->message("Abstract object cannot be privately "
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
    uid = uids_[owner];
    if (uid) {
        ownerobj = ownerobjs_[uid];
        DEBUG_ASSERT(ownerobj);
    } else {
	/* create owner object for owner */
	uid = uids_[owner] = next_uid_++;
        ownerobj = ownerobjs_[uid] = clone_object(OWNEROBJ, owner);
    }
    return ownerobj->add_data(uid, env);
}

/*
 * NAME:        find_data()
 * DESCRIPTION: find a managed LWO
 */
object find_data(int onumber)
{
    int     uid;
    object  ownerobj;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    uid = uid(onumber);
    ownerobj = ownerobjs_[uid];
    return ownerobj ? ownerobj->find_data(onumber) : nil;
}

/*
 * NAME:        move_data()
 * DESCRIPTION: move or remove a managed LWO
 */
void move_data(int onumber, object env)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    DEBUG_ASSERT(env);
    uid = uid(onumber);
    DEBUG_ASSERT(ownerobjs_[uid]);
    ownerobjs_[uid]->move_data(onumber, env);
}

/*
 * NAME:        data_callout()
 * DESCRIPTION: make a call-out for a managed LWO
 */
int data_callout(int onumber, string func, mixed delay, mixed *args)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    uid = uid(onumber);
    DEBUG_ASSERT(ownerobjs_[uid]);
    return ownerobjs_[uid]->data_callout(onumber, func, delay, args);
}

/*
 * NAME:        remove_data_callout()
 * DESCRIPTION: remove a call-out for a managed LWO
 */
mixed remove_data_callout(int onumber, int handle)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    uid = uid(onumber);
    DEBUG_ASSERT(ownerobjs_[uid]);
    return ownerobjs_[uid]->remove_data_callout(onumber, handle);
}

/*
 * NAME:        query_data_callouts()
 * DESCRIPTION: return the call-outs for a managed LWO
 */
mixed *query_data_callouts(string owner, int onumber)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    uid = uid(onumber);
    DEBUG_ASSERT(ownerobjs_[uid]);
    return ownerobjs_[uid]->query_data_callouts(owner, onumber);
}
