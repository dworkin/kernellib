# include <status.h>
# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/tls.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/system.h>

private inherit rsrc  API_RSRC;
private inherit tls   API_TLS;

object   driver;     /* driver object */
object   initd;      /* system initialization manager */
int      nextuid;    /* next user ID */
mapping  uids;       /* ([ string owner: int uid ]) */
mapping  ownerobjs;  /* ([ int uid: object ownerobj ]) */
mapping  oids;       /* ([ int num: object obj ]), where obj is persistent */

/*
 * NAME:        create()
 * DESCRIPTION: initialize object manager
 */
static void create()
{
    rsrc::create();
    tls::create();
    driver = find_object(DRIVER);
    initd = find_object(INITD);
    nextuid = 1;
    uids = ([ ]);
    ownerobjs = ([ ]);
    oids = ([ ]);
}

/*
 * NAME:        add_owner()
 * DESCRIPTION: register a new or existing owner
 */
private int add_owner(string owner)
{
    mixed uid;

    uid = uids[owner];
    if (uid == nil) {
	/* new owner: register and create owner object */
	uid = uids[owner] = nextuid++;
        ownerobjs[uid] = clone_object(OWNEROBJ, owner);
        driver->message("OBJECTD: added owner " + owner + " with UID " + uid
                        + "\n");
    }
    return uid;
}

/*
 * NAME:        query_uid()
 * DESCRIPTION: return the user ID of an owner
 */
int query_uid(string owner)
{
    mixed uid;

    uid = uids[owner];
    return uid ? uid : 0;
}

/*
 * NAME:        compile()
 * DESCRIPTION: the given object has just been compiled
 */
void compile(string owner, object obj, string *source, string inherited...)
{
    int uid;

    ASSERT_ACCESS(previous_object() == driver || previous_object() == initd);
    uid = add_owner(owner);
    DEBUG_ASSERT(ownerobjs[uid]);
    ownerobjs[uid]->compile(obj, source, inherited);
}

/*
 * NAME:        compile_lib()
 * DESCRIPTION: the given inheritable object has just been compiled
 */
void compile_lib(string owner, string path, string *source,
                 string inherited...)
{
    int uid;

    ASSERT_ACCESS(previous_object() == driver || previous_object() == initd);
    uid = add_owner(owner);
    DEBUG_ASSERT(ownerobjs[uid]);
    ownerobjs[uid]->compile(path, source, inherited);
}

/*
 * NAME:        clone()
 * DESCRIPTION: the given object has just been cloned
 */
void clone(string owner, object obj)
{
    int uid;

    ASSERT_ACCESS(previous_object() == driver);
    DEBUG_ASSERT(obj);
    if (sscanf(object_name(obj), OWNEROBJ + "#%*s")) {
        return;
    }
    uid = add_owner(owner);
    DEBUG_ASSERT(uid >= 1);
    DEBUG_ASSERT(ownerobjs[uid]);
    ownerobjs[uid]->clone(obj);
}

/*
 * NAME:        destruct()
 * DESCRIPTION: the given object is about to be destructed
 */
void destruct(string owner, object obj)
{
    int uid;

    ASSERT_ACCESS(previous_object() == driver);
    DEBUG_ASSERT(obj);
    uid = query_uid(owner);
    DEBUG_ASSERT(uid && ownerobjs[uid]);
    ownerobjs[uid]->destruct(obj);
}

/*
 * NAME:        remove_program()
 * DESCRIPTION: the last reference to the given program has been removed
 */
void remove_program(string owner, string path, int timestamp, int index)
{
    int uid;

    ASSERT_ACCESS(previous_object() == driver);

    DEBUG_ASSERT(uids[owner]);
    uid = uids[owner];
    DEBUG_ASSERT(ownerobjs[uid]);
    ownerobjs[uid]->remove_program(index);
}

/*
 * NAME:        include_file()
 * DESCRIPTION: returns an include path that depends on the compiled path
 */
mixed include_file(string compiled, string from, string path)
{
    ASSERT_ACCESS(previous_object() == driver);
    if (from == "/include/std.h" && path == "/include/AUTO"
        && driver->creator(compiled) != "System")
    {
        return ({ "inherit \"" + SYSTEM_AUTO + "\";\n" });
    } else {
        return path;
    }
}

/*
 * NAME:        forbid_call()
 * DESCRIPTION: returns true if a call is forbidden
 */
int forbid_call(string path)
{
    ASSERT_ACCESS(previous_object() == driver);
    return sscanf(path, "%*s" + CLONABLE_SUBDIR)
        || sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR);
}

/*
 * NAME:        forbid_inherit()
 * DESCRIPTION: returns true if an inheritance is forbidden
 */
int forbid_inherit(string from, string path, int priv)
{
    string fcreator, pcreator;

    ASSERT_ACCESS(previous_object() == driver);

    fcreator = driver->creator(from);
    pcreator = driver->creator(path);

    /* system objects cannot inherit user objects */
    if (fcreator == "System" && pcreator != "System") {
        return TRUE;
    }

    /*
     * user objects cannot inherit system objects, except for objects in
     * ~System/open
     */
    if (fcreator != "System" && pcreator == "System"
        && !sscanf(path, "/usr/System/open/%*s"))
    {
        return TRUE;
    }

    /* forbid private inheritance of undefined functions */
    if (priv) {
        mixed *status;
    
        status = status(path);
        if (status && status[O_UNDEFINED]) {
            driver->message("Undefined functions cannot be privately "
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
 * NAME:        find_program()
 * DESCRIPTION: find a program by object ID
 */
mixed *find_program(int oid)
{
    int uid;

    ASSERT_ACCESS(previous_program() == OWNEROBJ);
    DEBUG_ASSERT((oid & OID_CATEGORY_MASK) == OID_MASTER);
    uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
    DEBUG_ASSERT(ownerobjs[uid]);
    return ownerobjs[uid]->find_program(oid);
}

static mapping program_dir_map(string *dirs)
{
    int      i, size;
    mapping  map;

    map = ([ ]);
    size = sizeof(dirs);
    for (i = 0; i < size; ++i) {
        map[dirs[i]] = ({ -2 });
    }
    return map;
}

/*
 * NAME:        get_program_dir()
 * DESCRIPTION: return all programs within a parent directory
 */
mapping get_program_dir(string path)
{
    string  creator;
    mixed   uid;

    ASSERT_ACCESS(previous_program() == API_OBJECT);
    DEBUG_ASSERT(path);
    if (path == "/") {
        return program_dir_map(({ "kernel", USR_DIR[1 ..] }));
    } else if (path == USR_DIR) {
        return program_dir_map(query_owners() - ({ nil }));
    }

    creator = driver->creator(path + "/");
    uid = uids[creator];
    if (uid == nil) {
        return ([ ]);
    }
    DEBUG_ASSERT(ownerobjs[uid]);
    return ownerobjs[uid]->get_program_dir(path);
}

/*
 * NAME:        add_data()
 * DESCRIPTION: add a managed LWO
 */
int add_data(string owner, object env)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(env);
    uid = add_owner(owner);
    DEBUG_ASSERT(ownerobjs[uid]);
    return ownerobjs[uid]->add_data(env);
}

/*
 * NAME:        find()
 * DESCRIPTION: find a persistent object or managed LWO by number
 */
object find(int oid)
{
    int     uid;
    object  ownerobj;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
    ownerobj = ownerobjs[uid];
    return ownerobj ? ownerobj->find(oid) : nil;
}

/*
 * NAME:        move_data()
 * DESCRIPTION: move or remove a managed LWO
 */
void move_data(int oid, object env)
{
    int uid;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT((oid & OID_CATEGORY_MASK) == OID_LIGHTWEIGHT);
    uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
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
    DEBUG_ASSERT((oid & OID_CATEGORY_MASK) == OID_LIGHTWEIGHT);
    uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
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
    DEBUG_ASSERT((oid & OID_CATEGORY_MASK) == OID_LIGHTWEIGHT);
    uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
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
    DEBUG_ASSERT((oid & OID_CATEGORY_MASK) == OID_LIGHTWEIGHT);
    uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
    DEBUG_ASSERT(ownerobjs[uid]);
    return ownerobjs[uid]->query_data_callouts(owner, oid);
}
