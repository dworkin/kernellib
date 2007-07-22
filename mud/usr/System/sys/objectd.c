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
 * NAME:        join_oid()
 * DESCRIPTION: join a user ID and an object index into an object ID
 */
int join_oid(int uid, int index)
{
    ASSERT_ARG_1(uid >= 0);
    if (index <= -2) {
        return -uid * 1000000 + index;
    } else if (index >= 0) {
        return uid * 1000000 + index;
    } else {
        ASSERT_ARG_2(0);
    }
}

/*
 * NAME:        split_oid()
 * DESCRIPTION: split an object ID into a user ID and an object index
 */
int *split_oid(int oid)
{
    if (oid <= -2) {
        return ({ -oid / 1000000, -(-oid % 1000000) });
    } else if (oid >= 1) {
        return ({ oid / 1000000, oid % 1000000 });
    } else {
        ASSERT_ARG(0);
    }
}

/*
 * NAME:        compile()
 * DESCRIPTION: the given object has just been compiled
 */
void compile(string owner, object obj, string *source, string inherited...)
{
    int uid;

    ASSERT_ACCESS(previous_object() == driver || previous_object() == initd);
    if (object_name(obj) != DRIVER && sizeof(inherited) == 0) {
        inherited = ({ AUTO });
    }
    uid = add_owner(owner);
    DEBUG_ASSERT(ownerobjs[uid]);
    ownerobjs[uid]->compile(obj, inherited);
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
    if (path != AUTO && sizeof(inherited) == 0) {
        inherited = ({ AUTO });
    }
    uid = add_owner(owner);
    DEBUG_ASSERT(ownerobjs[uid]);
    ownerobjs[uid]->compile(path, inherited);
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
    DEBUG_ASSERT(oid >= 1);
    uid = split_oid(oid)[0];
    DEBUG_ASSERT(ownerobjs[uid] != nil);
    return ownerobjs[uid]->find_program(oid);
}

static mapping program_dir_map(string *dirs)
{
    int      i, size;
    mapping  map;

    map = ([ ]);
    size = sizeof(dirs);
    for (i = 0; i < size; ++i) {
        map[dirs[i]] = -2;
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

    ASSERT_ACCESS(previous_object() == initd);
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
    DEBUG_ASSERT(ownerobjs[uid] != nil);
    return ownerobjs[uid]->add_data(env);
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
    uid = split_oid(oid)[0];
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
    uid = split_oid(oid)[0];
    DEBUG_ASSERT(ownerobjs[uid] != nil);
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
    uid = split_oid(oid)[0];
    DEBUG_ASSERT(ownerobjs[uid] != nil);
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
    uid = split_oid(oid)[0];
    DEBUG_ASSERT(ownerobjs[uid] != nil);
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
    uid = split_oid(oid)[0];
    DEBUG_ASSERT(ownerobjs[uid] != nil);
    return ownerobjs[uid]->query_data_callouts(owner, oid);
}
