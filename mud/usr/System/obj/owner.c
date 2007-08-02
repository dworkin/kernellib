# include <status.h>
# include <type.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>

object   driver;     /* driver object */
object   objectd;    /* object manager */
int      uid;        /* user ID of owner */
mapping  progents;   /* ([ int oid: mixed *progent ]) */
mapping  prognames;  /* ([ string progname: ({ int oid, ... }) ]) */
int      nextindex;  /* index for the next managed LWO */

mapping persistent_;   /* ([ int oid: object obj ]) */
mapping lightweight_;  /* ([ int oid: object environment ]) */

/*
 * NAME:        create()
 * DESCRIPTION: initialize owner object
 */
static void create(int clone)
{
    if (clone) {
        driver = find_object(DRIVER);
        objectd = find_object(OBJECTD);
        uid = objectd->query_uid(query_owner());
        progents = ([ ]);
        prognames = ([ ]);
	nextindex = 1;

        persistent_ = ([ ]);
        lightweight_ = ([ ]);
    }
}

private void link_child(int child_oid, mixed *child_entry, string parent_name)
{
    int      parent_uid, index, parent_oid;
    string   creator;
    mixed   *parent_entry;

    /* get program entry for parent */
    creator = driver->creator(parent_name);
    parent_uid = objectd->query_uid(creator);
    DEBUG_ASSERT(parent_uid >= 1);
    index = status(parent_name)[O_INDEX] + 1;
    parent_oid = OID_MASTER | (parent_uid << OID_OWNER_OFFSET)
        | (index << OID_INDEX_OFFSET);
    parent_entry = objectd->find_program(parent_oid);
    DEBUG_ASSERT(parent_entry);

    /* link child */
    if (!parent_entry[PROG_FIRSTCHILD]) {
        /* first child: create chain */
        parent_entry[PROG_FIRSTCHILD] = child_oid;
        child_entry[PROG_PREVSIB][parent_oid] = child_oid;
        child_entry[PROG_NEXTSIB][parent_oid] = child_oid;
    } else {
        mixed *first_entry, *previous_entry;
        
        /* add to chain */
        first_entry = objectd->find_program(parent_entry[PROG_FIRSTCHILD]);
        previous_entry
            = objectd->find_program(first_entry[PROG_PREVSIB][parent_oid]);
        first_entry[PROG_PREVSIB][parent_oid] = child_oid;
        previous_entry[PROG_NEXTSIB][parent_oid] = child_oid;
    }
}

/*
 * NAME:        compile()
 * DESCRIPTION: the given object has just been compiled
 */
void compile(mixed obj, string *source, string *inherited)
{
    string   path;
    int      oid, i, size;
    mixed   *entry;

    ASSERT_ACCESS(previous_object() == objectd);
    path = (typeof(obj) == T_STRING) ? obj : object_name(obj);
    if (path != DRIVER && path != AUTO && !sizeof(inherited)) {
        inherited = ({ AUTO });
    }

    oid = OID_MASTER | (uid << OID_OWNER_OFFSET)
        | ((status(path)[O_INDEX] + 1) << OID_INDEX_OFFSET);
    entry = progents[oid] = ({ path, ([ ]), ([ ]), nil });
    size = sizeof(inherited);
    for (i = 0; i < size; ++i) {
        link_child(oid, entry, inherited[i]);
    }

    if (prognames[path]) {
        prognames[path] += ({ oid });
    } else {
        prognames[path] = ({ oid });
    }
}

void clone(object obj)
{
    int oid, index;

    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(obj);
    sscanf(object_name(obj), "%*s#%d", index);
    DEBUG_ASSERT(index >= 1);
    oid = OID_CLONE | (uid << OID_OWNER_OFFSET) | (index << OID_INDEX_OFFSET);
    DEBUG_ASSERT(!persistent_[oid]);
    persistent_[oid] = obj;
}

void destruct(object obj)
{
    int category, index, oid;

    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(obj);
    if (sscanf(object_name(obj), "%*s#%d", index)) {
        category = OID_CLONE;
    } else {
        category = OID_MASTER;
        index = status(obj)[O_INDEX] + 1;
    }
    DEBUG_ASSERT(index >= 1);
    oid = category | (uid << OID_OWNER_OFFSET) | (index << OID_INDEX_OFFSET);
    DEBUG_ASSERT(persistent_[oid]);
    persistent_[oid] = nil;
}

/*
 * NAME:        find()
 * DESCRIPTION: find a persistent object or managed LWO by number
 */
object find(int oid)
{
    ASSERT_ACCESS(previous_object() == objectd);
    if ((oid & OID_CATEGORY_MASK) == OID_LIGHTWEIGHT) {
        object environment;

        environment = lightweight_[oid];
        return environment ? environment->_F_find(oid) : nil;
    } else {
        return persistent_[oid];
    }
}

private void unlink_child(int child_oid, mixed *child_entry, int parent_oid)
{
    int     previous_oid;
    mixed  *parent_entry;

    parent_entry = objectd->find_program(parent_oid);
    DEBUG_ASSERT(parent_entry);
    previous_oid = child_entry[PROG_PREVSIB][parent_oid];
    if (child_oid == previous_oid) {
        /* only link: remove chain */
        DEBUG_ASSERT(child_oid == parent_entry[PROG_FIRSTCHILD]);
        parent_entry[PROG_FIRSTCHILD] = nil;
    } else {
        int     next_oid;
        mixed  *previous_entry, *next_entry;

        /* unlink child */
        next_oid = child_entry[PROG_NEXTSIB][parent_oid];
        previous_entry = objectd->find_program(previous_oid);
        DEBUG_ASSERT(previous_entry);
        next_entry = objectd->find_program(next_oid);
        DEBUG_ASSERT(next_entry);
        previous_entry[PROG_NEXTSIB][parent_oid] = next_oid;
        next_entry[PROG_PREVSIB][parent_oid] = previous_oid;

        if (parent_entry[PROG_FIRSTCHILD] == child_oid) {
            /* child was first in chain: appoint next child instead */
            parent_entry[PROG_FIRSTCHILD] = next_oid;
        }
    }
}

/*
 * NAME:        remove_program()
 * DESCRIPTION: the last reference to the given program has been removed
 */
void remove_program(int index)
{
    int      oid, i, size, *parent_oids;
    string   name;
    mixed   *entry;

    ASSERT_ACCESS(previous_object() == objectd);
    oid = OID_MASTER | (uid << OID_OWNER_OFFSET)
        | (index << OID_INDEX_OFFSET);
    entry = progents[oid];
    DEBUG_ASSERT(entry);

    name = entry[PROG_OBJNAME];
    DEBUG_ASSERT(prognames[name]);
    prognames[name] -= ({ oid });
    if (!sizeof(prognames[name])) {
        prognames[name] = nil;
    }

    parent_oids = map_indices(entry[PROG_PREVSIB]);
    size = sizeof(parent_oids);
    for (i = 0; i < size; ++i) {
        unlink_child(oid, entry, parent_oids[i]);
    }
    progents[oid] = nil;
}

/*
 * NAME:        find_program()
 * DESCRIPTION: find a program by object ID
 */
mixed *find_program(int oid)
{
    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT((oid & OID_CATEGORY_MASK) == OID_MASTER);
    DEBUG_ASSERT(progents[oid]);
    return progents[oid];
}

/*
 * NAME:        get_program_dir()
 * DESCRIPTION: return all programs within a parent directory
 */
mapping get_program_dir(string path)
{
    int       i, size;
    string   *paths;
    mapping   names, dirs, progs;

    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(path);

    names = prognames[path + "/" .. path + "0"] - ({ path + "0" }); 
    paths = map_indices(names);
    dirs = ([ ]);
    progs = ([ ]);
    size = sizeof(paths);
    for (i = 0; i < size; ++i) {
        string base;

        base = paths[i][strlen(path) + 1 ..];
        if (sscanf(base, "%s/", base) == 1) {
            dirs[base] = TRUE;
        } else {
            progs[base] = names[paths[i]];
        }
    }

    paths = map_indices(dirs);
    size = sizeof(paths);
    for (i = 0; i < size; ++i) {
        if (progs[paths[i]] == nil) {
            progs[paths[i]] = ({ -2 });
        } else {
            progs[paths[i]] = ({ -2 }) + progs[paths[i]];
        }
    }
    return progs;
}

/*
 * NAME:        add_data()
 * DESCRIPTION: register an LWO for management
 */
int add_data(object env)
{
    int oid;

    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(uid);
    DEBUG_ASSERT(env);
    oid = OID_LIGHTWEIGHT | (uid << OID_OWNER_OFFSET)
        | (nextindex++ << OID_INDEX_OFFSET);
    lightweight_[oid] = env;
    return oid;
}

/*
 * NAME:        move_data()
 * DESCRIPTION: move a managed LWO to another environment
 */
void move_data(int oid, object environment)
{
    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT((oid & OID_CATEGORY_MASK) == OID_LIGHTWEIGHT);
    DEBUG_ASSERT(lightweight_[oid]);
    lightweight_[oid] = environment;
}

/*
 * NAME:        data_callout()
 * DESCRIPTION: schedule a call-out for a managed LWO
 */
int data_callout(int oid, string function, mixed delay, mixed *arguments)
{
    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(function);
    DEBUG_ASSERT(typeof(delay) == T_INT || typeof(delay) == T_FLOAT);
    DEBUG_ASSERT(arguments);
    return call_out("call_data", delay, oid, function, arguments);
}

/*
 * NAME:        remove_data_callout()
 * DESCRIPTION: remove a call-out for a managed LWO
 */
mixed remove_data_callout(int oid, int handle)
{
    int      i, size;
    mixed  **callouts;

    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(oid);

    callouts = status(this_object())[O_CALLOUTS];
    for (i = sizeof(callouts) - 1; i >= 0; --i) {
        if (callouts[i][CO_HANDLE] == handle) {
            /*
             * Found the call-out. Remove it only if it belongs to the managed
             * LWO.
             */
            return (callouts[i][CO_FIRSTXARG] == oid)
                ? remove_call_out(handle) : -1;
        }
    }

    return -1;
}

/*
 * NAME:        query_data_callouts()
 * DESCRIPTION: return the call-outs of a managed LWO
 */
mixed *query_data_callouts(string owner, int oid)
{
    int      i, j, size, owned;
    mixed  **callouts;

    ASSERT_ACCESS(previous_object() == objectd);

    /* filter call-outs by object number */
    callouts = status(this_object())[O_CALLOUTS];
    size = sizeof(callouts);
    owned = (owner && owner == query_owner());
    for (i = j = 0; i < size; ++i) {
        if (callouts[i][CO_FIRSTXARG] == oid) {
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
static void call_data(int oid, string function, mixed *arguments)
{
    object environment;

    environment = lightweight_[oid];
    if (environment) {
        object obj;

        obj = environment->_F_find(oid);
        if (obj) {
            obj->_F_call_data(function, arguments);
        }
    }
}
