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
mapping  envs;       /* ([ int oid: object env ]) */

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
	nextindex = -2;
	envs = ([ ]);
    }
}

private void link_child(int childoid, mixed *childent, string parname)
{
    int      paruid, index, paroid;
    string   creator;
    mixed   *parent;

    /* get program entry for parent */
    creator = driver->creator(parname);
    paruid = objectd->query_uid(creator);
    DEBUG_ASSERT(paruid >= 1);
    index = status(parname)[O_INDEX];
    paroid = objectd->join_oid(paruid, index);
    parent = objectd->find_program(paroid);
    DEBUG_ASSERT(parent != nil);

    /* link child */
    if (parent[PROG_FIRSTCHILD] == nil) {
        /* first child: create chain */
        parent[PROG_FIRSTCHILD] = childoid;
        childent[PROG_PREVSIB][paroid] = childoid;
        childent[PROG_NEXTSIB][paroid] = childoid;
    } else {
        mixed *firstent, *prevent;
        
        /* add to chain */
        firstent = objectd->find_program(parent[PROG_FIRSTCHILD]);
        prevent = objectd->find_program(firstent[PROG_PREVSIB][paroid]);
        firstent[PROG_PREVSIB][paroid] = childoid;
        prevent[PROG_NEXTSIB][paroid] = childoid;
    }
}

/*
 * NAME:        compile()
 * DESCRIPTION: the given object has just been compiled
 */
void compile(string path, string *inherited)
{
    int     oid, i, size;
    mixed  *progent;

    ASSERT_ACCESS(previous_object() == objectd);

    oid = objectd->join_oid(uid, status(path)[O_INDEX]);
    progent = progents[oid] = ({ path, ([ ]), ([ ]), nil });
    size = sizeof(inherited);
    for (i = 0; i < size; ++i) {
        link_child(oid, progent, inherited[i]);
    }

    if (prognames[path]) {
        prognames[path] += ({ oid });
    } else {
        prognames[path] = ({ oid });
    }
}

private void unlink_child(int childoid, mixed *childent, int paroid)
{
    int     prevoid;
    mixed  *parent;

    parent = objectd->find_program(paroid);
    DEBUG_ASSERT(parent != nil);
    prevoid = childent[PROG_PREVSIB][paroid];
    if (childoid == prevoid) {
        /* only link: remove chain */
        DEBUG_ASSERT(childoid == parent[PROG_FIRSTCHILD]);
        parent[PROG_FIRSTCHILD] = nil;
    } else {
        int     nextoid;
        mixed  *prevent, *nextent;

        /* unlink child */
        nextoid = childent[PROG_NEXTSIB][paroid];
        prevent = objectd->find_program(prevoid);
        DEBUG_ASSERT(prevent != nil);
        nextent = objectd->find_program(nextoid);
        DEBUG_ASSERT(nextent != nil);
        prevent[PROG_NEXTSIB][paroid] = nextoid;
        nextent[PROG_PREVSIB][paroid] = prevoid;

        if (parent[PROG_FIRSTCHILD] == childoid) {
            /* child was first in chain: appoint next child instead */
            parent[PROG_FIRSTCHILD] = nextoid;
        }
    }
}

/*
 * NAME:        remove_program()
 * DESCRIPTION: the last reference to the given program has been removed
 */
void remove_program(int index)
{
    int      oid, i, size, *paroids;
    string   oname;
    mixed   *progent;

    ASSERT_ACCESS(previous_object() == objectd);
    oid = objectd->join_oid(uid, index);
    progent = progents[oid];
    DEBUG_ASSERT(progent != nil);

    oname = progent[PROG_OBJNAME];
    DEBUG_ASSERT(prognames[oname] != nil);
    prognames[oname] -= ({ oid });
    if (sizeof(prognames[oname]) == 0) {
        prognames[oname] = nil;
    }

    paroids = map_indices(progent[PROG_PREVSIB]);
    size = sizeof(paroids);
    for (i = 0; i < size; ++i) {
        unlink_child(oid, progent, paroids[i]);
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
    DEBUG_ASSERT(oid >= 0);
    DEBUG_ASSERT(progents[oid] != nil);
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
    oid = objectd->join_oid(uid, nextindex--);
    envs[oid] = env;
    return oid;
}

/*
 * NAME:        find_data()
 * DESCRIPTION: find a managed LWO by number
 */
object find_data(int oid)
{
    object env;

    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(oid <= -2);
    env = envs[oid];
    return env ? env->_F_find(oid) : nil;
}

/*
 * NAME:        move_data()
 * DESCRIPTION: move a managed LWO to another environment
 */
void move_data(int oid, object env)
{
    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(oid <= -2 && envs[oid]);
    envs[oid] = env;
}

/*
 * NAME:        data_callout()
 * DESCRIPTION: schedule a call-out for a managed LWO
 */
int data_callout(int oid, string func, mixed delay, mixed *args)
{
    ASSERT_ACCESS(previous_object() == objectd);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(func);
    DEBUG_ASSERT(args);
    return call_out("call_data", delay, oid, func, args);
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
static void call_data(int oid, string func, mixed *args)
{
    object env;

    env = envs[oid];
    if (env) {
        object obj;

        obj = env->_F_find(oid);
        if (obj) {
            obj->_F_call_data(func, args);
        }
    }
}
