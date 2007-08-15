# include <status.h>
# include <type.h>
# include <kernel/kernel.h>
# include <system/object.h>

object   driver_;             /* driver object */
object   objectd_;            /* object manager */
int      uid_;                /* UID of owner */

mapping  pdb_entries_;        /* ([ int oid: mixed *entry ]) */
mapping  pdb_paths_;          /* ([ string path: ({ int oid, ... }) ]) */

int      next_index_;         /* index for the next middle-weight object */
mapping  persistent_oids_;    /* ([ int oid: object obj ]) */
mapping  middleweight_oids_;  /* ([ bucket_index: ([ oid: environment ]) ]) */

/*
 * NAME:        link_child()
 * DESCRIPTION: add entry to linked list of children
 */
private void link_child(int child_oid, mixed *child_entry, string parent_name)
{
    int      parent_uid, index, parent_oid;
    string   creator;
    mixed   *parent_entry;

    /* get program entry for parent */
    creator = driver_->creator(parent_name);
    parent_uid = objectd_->query_uid(creator);
    index = status(parent_name)[O_INDEX] + 1;
    parent_oid = OID_MASTER | (parent_uid << OID_OWNER_OFFSET)
        | (index << OID_INDEX_OFFSET);
    parent_entry = objectd_->query_entry(parent_oid);

    /* link child */
    if (!parent_entry[PDB_CHILDREN]) {
        /* first child: create chain */
        parent_entry[PDB_CHILDREN] = child_oid;
        child_entry[PDB_PREVIOUS][parent_oid] = child_oid;
        child_entry[PDB_NEXT][parent_oid] = child_oid;
    } else {
        int     first_oid, previous_oid;
        mixed  *first_entry, *previous_entry;
        
        /* find links */
        first_oid = parent_entry[PDB_CHILDREN];
        first_entry = objectd_->query_entry(first_oid);
        previous_oid = first_entry[PDB_PREVIOUS][parent_oid];
        previous_entry = objectd_->query_entry(previous_oid);

        /* add new child to chain */
        previous_entry[PDB_NEXT][parent_oid] = child_oid;
        child_entry[PDB_PREVIOUS][parent_oid] = previous_oid;
        first_entry[PDB_PREVIOUS][parent_oid] = child_oid;
        child_entry[PDB_NEXT][parent_oid] = first_oid;
    }
}

/*
 * NAME:        unlink_child()
 * DESCRIPTION: remove entry from linked list of children
 */
private void unlink_child(int child_oid, mixed *child_entry, int parent_oid)
{
    int     previous_oid;
    mixed  *parent_entry;

    parent_entry = objectd_->query_entry(parent_oid);
    previous_oid = child_entry[PDB_PREVIOUS][parent_oid];
    if (child_oid == previous_oid) {
        /* only link: remove chain */
        parent_entry[PDB_CHILDREN] = 0;
        child_entry[PDB_PREVIOUS][parent_oid] = nil;
        child_entry[PDB_NEXT][parent_oid] = nil;
    } else {
        int     next_oid;
        mixed  *previous_entry, *next_entry;

        /* find entries */
        next_oid = child_entry[PDB_NEXT][parent_oid];
        previous_entry = objectd_->query_entry(previous_oid);
        next_entry = objectd_->query_entry(next_oid);

        /* unlink child */
        previous_entry[PDB_NEXT][parent_oid] = next_oid;
        child_entry[PDB_PREVIOUS][parent_oid] = nil;
        next_entry[PDB_PREVIOUS][parent_oid] = previous_oid;
        child_entry[PDB_NEXT][parent_oid] = nil;

        if (parent_entry[PDB_CHILDREN] == child_oid) {
            /* child was first in chain: appoint next child instead */
            parent_entry[PDB_CHILDREN] = next_oid;
        }
    }
}

/*
 * NAME:        create()
 * DESCRIPTION: initialize owner node
 */
static void create(int clone)
{
    if (clone) {
        driver_ = find_object(DRIVER);
        objectd_ = find_object(OBJECTD);
        uid_ = objectd_->query_uid(query_owner());

        pdb_entries_ = ([ ]);
        pdb_paths_ = ([ ]);

        next_index_ = 1; /* use 1-based indices for middle-weight objects */
        persistent_oids_ = ([ ]);
        middleweight_oids_ = ([ ]);
    }
}

/*
 * NAME:        compile()
 * DESCRIPTION: the given object has just been compiled
 */
void compile(mixed obj, string *source, string *inherited)
{
    if (previous_object() == objectd_) {
        string   path;
        int      index, oid, i, size;
        mixed   *entry;
        
        path = (typeof(obj) == T_STRING) ? obj : object_name(obj);
        if (path != DRIVER && path != AUTO && !sizeof(inherited)) {
            inherited = ({ AUTO });
        }

        index = status(path)[O_INDEX] + 1;
        oid = OID_MASTER | (uid_ << OID_OWNER_OFFSET)
            | (index << OID_INDEX_OFFSET);
        entry = pdb_entries_[oid];
        if (entry) {
            int *parent_oids;
            
            parent_oids = map_indices(entry[PDB_PREVIOUS]);
            size = sizeof(parent_oids);
            for (i = 0; i < size; ++i) {
                unlink_child(oid, entry, parent_oids[i]);
            }
        } else {
            entry = pdb_entries_[oid] = ({ path, ([ ]), ([ ]), 0 });
        }
        size = sizeof(inherited);
        for (i = 0; i < size; ++i) {
            link_child(oid, entry, inherited[i]);
        }
        
        if (pdb_paths_[path]) {
            pdb_paths_[path] += ({ oid });
        } else {
            pdb_paths_[path] = ({ oid });
        }
    }
}

/*
 * NAME:        clone()
 * DESCRIPTION: the given object has just been cloned
 */
void clone(object obj)
{
    if (previous_object() == objectd_) {
        int index, oid;

        sscanf(object_name(obj), "%*s#%d", index);
        ++index; /* use 1-based object indices */
        oid = OID_CLONE | (uid_ << OID_OWNER_OFFSET)
            | (index << OID_INDEX_OFFSET);
        persistent_oids_[oid] = obj;
    }
}

/*
 * NAME:        destruct()
 * DESCRIPTION: the given object is about to be destructed
 */
void destruct(object obj)
{
    if (previous_object() == objectd_) {
        int category, index, oid;

        if (sscanf(object_name(obj), "%*s#%d", index)) {
            category = OID_CLONE;
        } else {
            category = OID_MASTER;
            index = status(obj)[O_INDEX] + 1;
        }
        oid = category | (uid_ << OID_OWNER_OFFSET)
            | (index << OID_INDEX_OFFSET);
        persistent_oids_[oid] = nil;
    }
}

/*
 * NAME:        find()
 * DESCRIPTION: find a persistent or middle-weight object by number
 */
object find(int oid)
{
    if (previous_object() == objectd_) {
        if ((oid & OID_CATEGORY_MASK) == OID_MIDDLEWEIGHT) {
            int      index;
            mapping  bucket;
            object   environment;

            index = (oid & OID_INDEX_MASK) >> OID_INDEX_OFFSET;
            bucket = middleweight_oids_[(index - 1) / MWO_BUCKET_SIZE];
            if (!bucket) {
                return nil;
            }
            environment = bucket[oid];
            return environment ? environment->_F_find(oid) : nil;
        } else {
            return persistent_oids_[oid];
        }
    }
}

/*
 * NAME:        remove_program()
 * DESCRIPTION: the last reference to the given program has been removed
 */
void remove_program(string path, int index)
{
    if (previous_object() == objectd_) {
        int     oid, i, size, *parent_oids;
        mixed  *entry;

        ++index; /* use 1-based indices, not 0-based as supplied by DGD */
        oid = OID_MASTER | (uid_ << OID_OWNER_OFFSET)
            | (index << OID_INDEX_OFFSET);
        entry = pdb_entries_[oid];

        pdb_paths_[path] -= ({ oid });
        if (!sizeof(pdb_paths_[path])) {
            pdb_paths_[path] = nil;
        }

        parent_oids = map_indices(entry[PDB_PREVIOUS]);
        size = sizeof(parent_oids);
        for (i = 0; i < size; ++i) {
            unlink_child(oid, entry, parent_oids[i]);
        }
        pdb_entries_[oid] = nil;
    }
}

/*
 * NAME:        query_entry()
 * DESCRIPTION: return the program entry with the specified object number
 */
mixed *query_entry(int oid)
{
    if (previous_object() == objectd_) {
        mixed *entry;

        entry = pdb_entries_[oid];
        if (!entry) {
            error("No such program");
        }
        return entry;
    }
}

/*
 * NAME:        get_program_dir()
 * DESCRIPTION: return all programs within a parent directory
 */
mapping get_program_dir(string path)
{
    if (previous_object() == objectd_) {
        int       i, size;
        string   *paths;
        mapping   names, directories, programs;
        
        names = pdb_paths_[path + "/" .. path + "0"] - ({ path + "0" }); 
        paths = map_indices(names);
        directories = ([ ]);
        programs = ([ ]);
        size = sizeof(paths);
        for (i = 0; i < size; ++i) {
            string base;
            
            base = paths[i][strlen(path) + 1 ..];
            if (sscanf(base, "%s/", base) == 1) {
                directories[base] = TRUE;
            } else {
                programs[base] = names[paths[i]];
            }
        }
        
        paths = map_indices(directories);
        size = sizeof(paths);
        for (i = 0; i < size; ++i) {
            programs[paths[i]] = programs[paths[i]]
                ? ({ -2 }) + programs[paths[i]] : ({ -2 });
        }
        return programs;
    }
}

/*
 * NAME:        add_mwo()
 * DESCRIPTION: add a middle-weight object
 */
int add_mwo(object environment)
{
    if (previous_object() == objectd_) {
        int      oid, index;
        mapping  bucket;

        index = next_index_++;
        oid = OID_MIDDLEWEIGHT | (uid_ << OID_OWNER_OFFSET)
            | (index << OID_INDEX_OFFSET);
        bucket = middleweight_oids_[(index - 1) / MWO_BUCKET_SIZE];
        if (!bucket) {
            bucket = middleweight_oids_[(index - 1) / MWO_BUCKET_SIZE]
                = ([ ]);
        }
        bucket[oid] = environment;
        return oid;
    }
}

/*
 * NAME:        move_mwo()
 * DESCRIPTION: move a middle-weight object to another environment
 */
void move_mwo(int oid, object environment)
{
    if (previous_object() == objectd_) {
        int      index;
        mapping  bucket;
        
        index = (oid & OID_INDEX_MASK) >> OID_INDEX_OFFSET;
        bucket = middleweight_oids_[(index - 1) / MWO_BUCKET_SIZE];
        bucket[oid] = environment;
        if (!environment && !map_sizeof(bucket)) {
            middleweight_oids_[(index - 1) / MWO_BUCKET_SIZE] = nil;
        }
    }
}

/*
 * NAME:        add_mwo_callout()
 * DESCRIPTION: add a callout for a middle-weight object
 */
int add_mwo_callout(int oid, string function, mixed delay, mixed *arguments)
{
    if (previous_object() == objectd_) {
        return call_out("mwo_callout", delay, oid, function, arguments);
    }
}

/*
 * NAME:        remove_mwo_callout()
 * DESCRIPTION: remove a callout for a middle-weight object
 */
mixed remove_mwo_callout(int oid, int handle)
{
    if (previous_object() == objectd_) {
        int      i;
        mixed  **callouts;

        callouts = status(this_object())[O_CALLOUTS];
        for (i = sizeof(callouts) - 1; i >= 0; --i) {
            if (callouts[i][CO_HANDLE] == handle) {
                /*
                 * Found the callout. Remove it only if it belongs to the
                 * middle-weight object.
                 */
                return (callouts[i][CO_FIRSTXARG] == oid)
                    ? remove_call_out(handle) : -1;
            }
        }
        return -1;
    }
}

/*
 * NAME:        query_mwo_callouts()
 * DESCRIPTION: return the callouts for a middle-weight object
 */
mixed *query_mwo_callouts(string owner, int oid)
{
    if (previous_object() == objectd_) {
        int      i, j, size, owned;
        mixed  **callouts;

        /* filter callouts by object number */
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
}

/*
 * NAME:        mwo_callout()
 * DESCRIPTION: dispatch a callout to a middle-weight object
 */
static void mwo_callout(int oid, string function, mixed *arguments)
{
    int      index;
    mapping  bucket;
    object   environment, obj;

    index = (oid & OID_INDEX_MASK) >> OID_INDEX_OFFSET;
    bucket = middleweight_oids_[(index - 1) / MWO_BUCKET_SIZE];
    if (!bucket) {
        return;
    }
    environment = bucket[oid];
    if (!environment) {
        return;
    }
    obj = environment->_F_find(oid);
    if (obj) {
        obj->_F_mwo_callout(function, arguments);
    }
}
