# include <status.h>
# include <type.h>
# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/tls.h>
# include <system/object.h>
# include <system/system.h>
# include <system/rsrc.h>
# include <system/tls.h>

private inherit rsrc  API_RSRC;
private inherit tls   API_TLS;

object   driver_;    /* driver object */
object   initd_;     /* system initialization manager */
int      next_uid_;  /* next user ID */
mapping  uids_;      /* ([ string owner: int uid ]), where uid >= 1 */
mapping  nodes_;     /* ([ int uid: object owner_node ]), where uid >= 1 */

/*
 * NAME:        add_owner()
 * DESCRIPTION: register a new or existing owner
 */
private int add_owner(string owner)
{
    mixed uid;

    uid = uids_[owner];
    if (!uid) {
        /* register new owner and create owner node */
        uid = uids_[owner] = next_uid_++;
        nodes_[uid] = clone_object(OWNER_NODE, owner);
    }
    return uid;
}

/*
 * NAME:        program_dir_map()
 * DESCRIPTION: return a mapping for a directory array
 */
private mapping program_dir_map(string *directories)
{
    int      i, size;
    mapping  map;

    map = ([ ]);
    size = sizeof(directories);
    for (i = 0; i < size; ++i) {
        map[directories[i]] = ({ -2 });
    }
    return map;
}

/*
 * NAME:        _demote_mwo()
 * DESCRIPTION: demote a middle-weight object
 */
private void _demote_mwo(int oid) {
    int     uid;
    object  node;

    uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
    node = nodes_[uid];
    node->demote_mwo(oid);
    rsrc_incr(node->query_owner(), MWO_RESOURCE, nil, -1);
}

/*
 * NAME:        create()
 * DESCRIPTION: initialize object manager
 */
static void create()
{
    rsrc::create();
    tls::create();
    driver_ = find_object(DRIVER);
    initd_ = find_object(INITD);
    next_uid_ = 1; /* use 1-based user IDs */
    uids_ = ([ ]);
    nodes_ = ([ ]);
    set_rsrc(MWO_RESOURCE, -1, 0, 0);
}

/*
 * NAME:        query_uid()
 * DESCRIPTION: return the user ID of an owner
 */
int query_uid(string owner)
{
    mixed uid;

    uid = uids_[owner];
    return uid ? uid : 0;
}

/*
 * NAME:        compile()
 * DESCRIPTION: the given object has just been compiled
 */
void compile(string owner, object obj, string *source, string inherited...)
{
    if (previous_object() == driver_ || previous_object() == initd_) {
        int uid;

        uid = add_owner(owner);
        nodes_[uid]->compile(obj, source, inherited);
    }
}

/*
 * NAME:        compile_lib()
 * DESCRIPTION: the given inheritable object has just been compiled
 */
void compile_lib(string owner, string path, string *source,
                 string inherited...)
{
    if (previous_object() == driver_ || previous_object() == initd_) {
        int uid;

        uid = add_owner(owner);
        nodes_[uid]->compile(path, source, inherited);
    }
}

/*
 * NAME:        clone()
 * DESCRIPTION: the given object has just been cloned
 */
void clone(string owner, object obj)
{
    if (previous_object() == driver_) {
        int uid;

        if (sscanf(object_name(obj), OWNER_NODE + "#%*s")) {
            return;
        }
        uid = add_owner(owner);
        nodes_[uid]->clone(obj);
    }
}

/*
 * NAME:        destruct()
 * DESCRIPTION: the given object is about to be destructed
 */
void destruct(string owner, object obj)
{
    if (previous_object() == driver_) {
        int uid;

        if (obj <- SYSTEM_AUTO) {
            obj->_F_system_destruct();
        }
        uid = query_uid(owner);
        nodes_[uid]->destruct(obj);
    }
}

/*
 * NAME:        remove_program()
 * DESCRIPTION: the last reference to the given program has been removed
 */
void remove_program(string owner, string path, int timestamp, int index)
{
    if (previous_object() == driver_) {
        int uid;

        uid = uids_[owner];
        nodes_[uid]->remove_program(path, index);
    }
}

/*
 * NAME:        include_file()
 * DESCRIPTION: returns an include path that depends on the compiled path
 */
mixed include_file(string compiled, string from, string path)
{
    if (previous_object() == driver_) {
        if (from == "/include/std.h" && path == "/include/AUTO"
            && driver_->creator(compiled) != "System")
        {
            return ({ "inherit \"" + SYSTEM_AUTO + "\";\n" });
        } else {
            return path;
        }
    }
}

/*
 * NAME:        forbid_call()
 * DESCRIPTION: returns true if a call is forbidden
 */
int forbid_call(string path)
{
    if (previous_object() == driver_) {
        return sscanf(path, "%*s" + CLONABLE_SUBDIR)
            || sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR);
    }
}

/*
 * NAME:        forbid_inherit()
 * DESCRIPTION: returns true if an inheritance is forbidden
 */
int forbid_inherit(string from, string path, int priv)
{
    if (previous_object() == driver_) {
        string from_creator, path_creator;

        from_creator = driver_->creator(from);
        path_creator = driver_->creator(path);
        
        /* system objects cannot inherit user objects */
        if (from_creator == "System" && path_creator != "System") {
            return TRUE;
        }
        
        /*
         * user objects cannot inherit system objects, except for objects in
         * ~System/open
         */
        if (from_creator != "System" && path_creator == "System"
            && !sscanf(path, "/usr/System/open/%*s"))
        {
            return TRUE;
        }
        
        /* forbid private inheritance of undefined functions */
        if (priv) {
            mixed *status;
            
            status = status(path);
            if (status && status[O_UNDEFINED]) {
                driver_->message("Undefined functions cannot be privately "
                                 + "inherited\n");
                return TRUE;
            }
        }
        
        return FALSE;
    }
}

/*
 * NAME:        store_create_arguments()
 * DESCRIPTION: store create() arguments in thread-local storage
 */
void store_create_arguments(mixed *arguments)
{
    if (previous_program() == SYSTEM_AUTO) {
        set_tlvar(TLS_CREATE_ARGUMENTS, arguments);
    }
}

/*
 * NAME:        fetch_create_arguments()
 * DESCRIPTION: fetch create() arguments from thread-local storage
 */
mixed *fetch_create_arguments()
{
    if (previous_program() == SYSTEM_AUTO) {
        mixed *arguments;
        
        arguments = get_tlvar(TLS_CREATE_ARGUMENTS);
        if (arguments) {
            set_tlvar(TLS_CREATE_ARGUMENTS, nil);
        }
        return arguments;
    }
}

/*
 * NAME:        query_entry()
 * DESCRIPTION: return the program entry for the specified object number
 */
mixed *query_entry(int oid)
{
    if (previous_program() == OWNER_NODE) {
        int uid;

        uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
        return nodes_[uid]->query_entry(oid);
    }
}

/*
 * NAME:        get_program_dir()
 * DESCRIPTION: return all programs within a parent directory
 */
mapping get_program_dir(string path)
{
    if (previous_program() == API_OBJECT) {
        string  creator;
        mixed   uid;

        if (path == "/") {
            return program_dir_map(({ "kernel", USR_DIR[1 ..] }));
        } else if (path == USR_DIR) {
            return program_dir_map(query_owners() - ({ nil }));
        }
        
        creator = driver_->creator(path + "/");
        uid = uids_[creator];
        if (uid == nil) {
            return ([ ]);
        }
        return nodes_[uid]->get_program_dir(path);
    }
}

/*
 * NAME:        find_by_number()
 * DESCRIPTION: find an object by number
 */
object find_by_number(int oid)
{
    if (previous_program() == SYSTEM_AUTO) {
        int     uid;
        object  node;

        uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
        node = nodes_[uid];
        return node ? node->find_by_number(oid) : nil;
    }
}

/*
 * NAME:        promote_mwo()
 * DESCRIPTION: promote a light-weight object to middle-weight
 */
int promote_mwo(string owner, object environment)
{
    if (previous_program() == SYSTEM_AUTO) {
        int uid;

        if (!rsrc_incr(owner, MWO_RESOURCE, nil, 1)) {
            error("Too many middle-weight objects");
        }
        uid = add_owner(owner);
        return nodes_[uid]->promote_mwo(environment);
    }
}

/*
 * NAME:        demote_mwo()
 * DESCRIPTION: demote a middle-weight object to light-weight
 */
void demote_mwo(int oid)
{
    if (previous_program() == SYSTEM_AUTO) {
        _demote_mwo(oid);
    }
}

/*
 * NAME:        move_mwo()
 * DESCRIPTION: move a middle-weight object
 */
void move_mwo(int oid, object environment)
{
    if (previous_program() == SYSTEM_AUTO) {
        int uid;

        uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
        nodes_[uid]->move_mwo(oid, environment);
    }
}

/*
 * NAME:        add_mwo_callout()
 * DESCRIPTION: add a callout for a middle-weight object
 */
int add_mwo_callout(int oid, string function, mixed delay, mixed *arguments)
{
    if (previous_program() == SYSTEM_AUTO) {
        int uid;

        uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
        return nodes_[uid]->add_mwo_callout(oid, function, delay, arguments);
    }
}

/*
 * NAME:        remove_mwo_callout()
 * DESCRIPTION: remove a callout for a middle-weight object
 */
mixed remove_mwo_callout(int oid, int handle)
{
    if (previous_program() == SYSTEM_AUTO) {
        int uid;

        uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
        return nodes_[uid]->remove_mwo_callout(oid, handle);
    }
}

/*
 * NAME:        query_mwo_callouts()
 * DESCRIPTION: return the callouts for a middle-weight object
 */
mixed *query_mwo_callouts(string owner, int oid)
{
    if (previous_program() == SYSTEM_AUTO) {
        int uid;

        uid = (oid & OID_OWNER_MASK) >> OID_OWNER_OFFSET;
        return nodes_[uid]->query_mwo_callouts(owner, oid);
    }
}
