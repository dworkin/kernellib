# include <status.h>
# include <type.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/system.h>
# include <system/tls.h>

private int      oid_;          /* object number */
private object   environment_;  /* environment */
private mapping  inventory_;    /* ([ int oid: object obj ]) */

/*
 * NAME:        create()
 * DESCRIPTION: user initialization function
 */
static void create(mixed arguments...) { }

/*
 * NAME:        promote()
 * DESCRIPTION: called on promotion to middle-weight object
 */
static void promote() { }

/*
 * NAME:        message()
 * DESCRIPTION: write a message to the console
 */
static void message(string message)
{
    ASSERT_ARGUMENT(message);
    ::find_object(DRIVER)->message(previous_program() + ": " + message + "\n");
}

/*
 * NAME:        creator()
 * DESCRIPTION: get creator of file
 */
private string creator(string path)
{
    return ::find_object(DRIVER)->creator(path);
}

/*
 * NAME:        normalize_path()
 * DESCRIPTION: normalize a path relative to this object
 */
private string normalize_path(string path)
{
    string  name;
    object  driver;

    driver = ::find_object(DRIVER);
    name = object_name(this_object());
    return driver->normalize_path(path, name + "/..", driver->creator(name));
}

/*
 * NAME:        undefined_error()
 * DESCRIPTION: log and raise an undefined function error
 */
private void undefined_error(string name, mapping undefined)
{
    string  *programs, **functions;
    int      i, size;

    programs = map_indices(undefined);
    functions = map_values(undefined);
    size = sizeof(programs);
    for (i = 0; i < size; ++i) {
        string message;
        
        message = "undefined function";
        if (sizeof(functions[i]) == 1) {
            message += " " + functions[i][0];
        } else {
            message += "s " + implode(functions[i], ", ");
        }
        if (programs[i] != name) {
            message += " (declared in " + programs[i] + ")";
        }
        ::find_object(DRIVER)->message(name + ": " + message + "\n");
    }
    error("Non-inheritable object cannot have undefined functions");
}

/*
 * NAME:        _object_number()
 * DESCRIPTION: return the object number of a persistent object, or 0 if the
 *              object is non-persistent
 */
private int _object_number(object obj)
{
    string  path, owner;
    int     category, uid, index, oid;

    path = object_name(obj);
    if (sscanf(path, "%*s#%d", index)) {
        if (index == -1) {
            /* non-persistent object */
            return 0;
        }

        category = OID_CLONE;
        owner = obj->query_owner();
        ++index; /* use 1-based object indices */
    } else {
        category = OID_MASTER;
        owner = creator(path);
        index = ::status(obj)[O_INDEX] + 1; /* use 1-based object indices */
    }
    uid = ::find_object(OBJECTD)->query_uid(owner);
    return category | (uid << OID_OWNER_OFFSET) | (index << OID_INDEX_OFFSET);
}

/*
 * NAME:        normalize_mwo()
 * DESCRIPTION: normalize a middle-weight object, silently demoting it to a
 *              light-weight object if it is a duplicate
 */
private void normalize_mwo()
{
    if ((oid_ & OID_CATEGORY_MASK) == OID_MIDDLEWEIGHT
        && (!environment_ || environment_->_F_find(oid_) != this_object()))
    {
        /* duplicated middle-weight object: demote to light-weight */
        oid_ = 0;
        environment_ = nil;
    }
}

/*
 * NAME:        _move_object()
 * DESCRIPTION: move object to destination
 */
private void _move_object(object destination)
{
    int promoted;

    if (environment_) {
        environment_->_F_leave(oid_);
    }
    environment_ = destination;
    promoted = FALSE;
    if ((oid_ & OID_CATEGORY_MASK) == OID_MIDDLEWEIGHT) {
        if (environment_) {
            /* move middle-weight object */
            ::find_object(OBJECTD)->move(oid_, environment_);
        } else {
            /* demote middle-weight object */
            ::find_object(OBJECTD)->demote(oid_);
        }
    } else if (!oid_ && environment_) {
        /* promote to middle-weight object */
        promoted = TRUE;
        oid_ = ::find_object(OBJECTD)->promote(query_owner(), environment_);
    }
    if (environment_) {
        environment_->_F_enter(oid_, this_object());
    }

    if (promoted) {
        promote();
    }
}

/*
 * NAME:        _F_system_create()
 * DESCRIPTION: system initialization function
 */
nomask int _F_system_create(varargs int clone)
{
    if (previous_program() == AUTO) {
        /* non-inheritable objects cannot have undefined functions */
        if (!clone) {
            mapping undefined;

            undefined = status(this_object())[O_UNDEFINED];
            if (undefined) {
                undefined_error(object_name(this_object()), undefined);
            }
        }

        /* initialize object number */
        oid_ = _object_number(this_object());

        if (clone) {
            mixed *arguments;

            arguments
                = ::find_object(OBJECTD)->remove_tlvar(TLS_CREATE_ARGUMENTS);
            if (arguments) {
                /* pass arguments to create() */
                call_limited("create", arguments...);
            } else {
                /* no arguments */
                call_limited("create");
            }
        } else {
            string path;

            /*
             * do not call create() for clonable or light-weight master objects
             */
            path = object_name(this_object());
            if (!sscanf(path, "%*s" + CLONABLE_SUBDIR)
                && !sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR))
            {
                call_limited("create"); 
            }
        }

        /* kernel initialization function should not call create() */
        return TRUE;
    }
}

/*
 * NAME:        _F_system_destruct()
 * DESCRIPTION: prepare object for destruction
 */
nomask void _F_system_destruct()
{
    if (previous_program() == OBJECTD && inventory_) {
        object  *inventory;
        int      i, size;

        inventory = map_values(inventory_);
        size = sizeof(inventory);
        for (i = 0; i < size; ++i) {
            if (sscanf(object_name(inventory[i]), "%*s#-1")) {
                /* demote middle-weight object */
                inventory[i]->_F_demote();
            }
        }
    }
}

/*
 * NAME:        _Q_oid()
 * DESCRIPTION: return object number
 */
nomask int _Q_oid()
{
    if (previous_program() == SYSTEM_AUTO) {
        normalize_mwo();
        return oid_;
    }
}

/*
 * NAME:        _F_demote()
 * DESCRIPTION: demote middle-weight object on destruction of environment
 */
nomask void _F_demote()
{
    if (previous_program() == SYSTEM_AUTO) {
        _move_object(nil);
    }
}

/*
 * NAME:        _F_enter()
 * DESCRIPTION: add object to inventory
 */
nomask void _F_enter(int oid, object obj)
{
    if (previous_program() == SYSTEM_AUTO) {
        if (!inventory_) {
            inventory_ = ([ ]);
        }
        inventory_[oid] = obj;
    }
}

/*
 * NAME:        _F_leave()
 * DESCRIPTION: remove object from inventory
 */
nomask void _F_leave(int oid)
{
    if (previous_program() == SYSTEM_AUTO) {
        inventory_[oid] = nil;
    }
}

/*
 * NAME:        _F_find()
 * DESCRIPTION: find an object by number in this environment
 */
nomask object _F_find(int oid)
{
    if (previous_program() == SYSTEM_AUTO
        || previous_program() == OWNER_NODE)
    {
        return inventory_ ? inventory_[oid] : nil;
    }
}

/*
 * NAME:        _Q_inventory()
 * DESCRIPTION: return the inventory of this object
 */
nomask object *_Q_inventory()
{
    if (previous_program() == SYSTEM_AUTO) {
        return inventory_ ? map_values(inventory_) - ({ nil }) : ({ });
    }
}

/*
 * NAME:        _Q_environment()
 * DESCRIPTION: return the environment of this object
 */
nomask object _Q_environment()
{
    if (previous_program() == SYSTEM_AUTO) {
        normalize_mwo();
        return environment_;
    }
}

/*
 * NAME:        object_number()
 * DESCRIPTION: return the object number of an object
 */
static int object_number(object obj)
{
    ASSERT_ARGUMENT(obj);
    return obj <- SYSTEM_AUTO ? obj->_Q_oid() : _object_number(obj);
}

/*
 * NAME:        find_object()
 * DESCRIPTION: find an object by name or number
 */
static object find_object(mixed name)
{
    object obj;

    switch (typeof(name)) {
    case T_INT:
        /* find object by number */
        obj = ::find_object(OBJECTD)->find(name);
        break;

    case T_STRING:
        /* find object by name */
        obj = ::find_object(name);
        break;

    default:
        ASSERT_ARGUMENT(FALSE);
    }
    if (!obj) {
        return nil;
    }

    /* cannot find clonable or light-weight master objects */
    name = object_name(obj);
    return sscanf(name, "%*s#") || !sscanf(name, "%*s" + CLONABLE_SUBDIR)
        && !sscanf(name, "%*s" + LIGHTWEIGHT_SUBDIR) ? obj : nil;
}

/*
 * NAME:        clone_object()
 * DESCRIPTION: create a new persistent clone
 */
static atomic object clone_object(string master, mixed arguments...)
{
    ASSERT_ARGUMENT_1(master);

    /* forward arguments to create() via thread-local storage */
    if (sizeof(arguments)) {
        ::find_object(OBJECTD)->set_tlvar(TLS_CREATE_ARGUMENTS, arguments);
    }

    return ::clone_object(master);
}

/*
 * NAME:        new_object()
 * DESCRIPTION: create or copy a light-weight object
 */
static atomic object new_object(mixed master, mixed arguments...)
{
    if (typeof(master) == T_STRING) {
        /*
         * create a new light-weight object, forwarding arguments to create()
         * via thread-local storage
         */
        if (sizeof(arguments)) {
            ::find_object(OBJECTD)->set_tlvar(TLS_CREATE_ARGUMENTS, arguments);
        }
    } else {
        /* copy an existant light-weight object */
        ASSERT_ARGUMENT_1(typeof(master) == T_OBJECT
                          && sscanf(object_name(master), "%*s#-1") == 1);

        /* cannot pass arguments when copying LWO */
        ASSERT_MESSAGE(!sizeof(arguments), "Cannot pass arguments");
    }
    return ::new_object(master);
}

/*
 * NAME:        status()
 * DESCRIPTION: return information about an object or the system
 */
static mixed *status(varargs mixed obj)
{
    mixed *status;

    switch (typeof(obj)) {
    case T_NIL:
        return ::status();

    case T_STRING:
        return ::status(obj);

    case T_OBJECT:
        status = ::status(obj);
        if (obj <- SYSTEM_AUTO && sscanf(object_name(obj), "%*s#-1")) {
            int oid;
            
            oid = obj->_Q_oid();
            if ((oid & OID_CATEGORY_MASK) == OID_MIDDLEWEIGHT) {
                obj = ::find_object(OBJECTD);
                status[O_CALLOUTS] = obj->query_mwo_callouts(query_owner(),
                                                             oid);
            }
        }
        return status;

    default:
        ASSERT_ARGUMENT(FALSE);
    }
}

/*
 * NAME:        move_object()
 * DESCRIPTION: move this object to another environment
 */
static atomic void move_object(object destination)
{
    object this;

    ASSERT_ARGUMENT(!destination
                    || !sscanf(object_name(destination), "%*s#-1"));
    this = this_object();
    if (!this || destination && (!destination->allow_move(this)
                                 || !destination || !this))
    {
        error("Cannot move object to destination");
    }

    normalize_mwo();
    if (oid_ && (oid_ & OID_CATEGORY_MASK) != OID_MIDDLEWEIGHT)
    {
        object obj;

        for (obj = destination; obj; obj = obj->_Q_environment()) {
            if (obj == this) {
                error("Cannot move object into itself");
            }
        }
    }
    
    _move_object(destination);
}

/*
 * NAME:        environment()
 * DESCRIPTION: return the environment of an object
 */
static object environment(object obj)
{
    ASSERT_ARGUMENT(obj);
    return obj <- SYSTEM_AUTO ? obj->_Q_environment() : nil;
}

/*
 * NAME:        inventory()
 * DESCRIPTION: return the inventory of an object
 */
static object *inventory(object obj)
{
    ASSERT_ARGUMENT(obj);
    return obj <- SYSTEM_AUTO ? obj->_Q_inventory() : ({ });
}

/*
 * NAME:        compile_object()
 * DESCRIPTION: compile an object
 */
static atomic object compile_object(string path, string source...)
{
    object   obj;
    mapping  undefined;

    ASSERT_ARGUMENT_1(path);
    obj = ::compile_object(path, source...);
    if (!obj) {
        /* the kernel does not return inheritable objects */
        return nil;
    }

    /* non-inheritable objects cannot have undefined functions */
    path = object_name(obj);
    undefined = status(obj)[O_UNDEFINED];
    if (undefined) {
        undefined_error(path, undefined);
    }

    /* do not return clonable or light-weight master objects */
    return sscanf(path, "%*s" + CLONABLE_SUBDIR)
        || sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR) ? nil : obj;
}

/*
 * NAME:        get_dir()
 * DESCRIPTION: list the contents of a directory
 */
static mixed **get_dir(string path)
{
    mixed **list;

    ASSERT_ARGUMENT(path);
    path = normalize_path(path);
    list = ::get_dir(path);

    /* do not return clonable or light-weight master objects */
    if (sscanf(path, "%*s" + CLONABLE_SUBDIR)
        || sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR))
    {
        int i, size;

        size = sizeof(list[3]);
        for (i = 0; i < size; ++i) {
            if (typeof(list[3][i]) == T_OBJECT) {
                list[3][i] = TRUE;
            }
        }
    }
    return list;
}

/*
 * NAME:        file_info()
 * DESCRIPTION: return information for a file or directory
 */
static mixed *file_info(string path)
{
    mixed *info;

    ASSERT_ARGUMENT(path);
    info = ::file_info(path);
    if (typeof(info[2]) == T_OBJECT) {
        /* do not return clonable or light-weight master objects */
        path = object_name(info[2]);
        if (sscanf(path, "%*s" + CLONABLE_SUBDIR)
            || sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR))
        {
            info[2] = TRUE;
        }
    }
    return info;
}

/*
 * NAME:        call_out()
 * DESCRIPTION: add a delayed function call
 */
static int call_out(string function, mixed delay, mixed arguments...)
{
    string program;

    ASSERT_ARGUMENT_1(function);
    program = ::function_object(function, this_object());
    ASSERT_ARGUMENT_1(program && (creator(program) != "System"
                                  || function == "create"));
    ASSERT_ARGUMENT_2(typeof(delay) == T_INT || typeof(delay) == T_FLOAT);
    normalize_mwo();
    if ((oid_ & OID_CATEGORY_MASK) == OID_MIDDLEWEIGHT) {
        return ::find_object(OBJECTD)->add_mwo_callout(oid_, function, delay,
                                                       arguments);
    } else {
        return ::call_out(function, delay, arguments...);
    }
}

/*
 * NAME:        remove_call_out()
 * DESCRIPTION: remove a delayed function call
 */
static mixed remove_call_out(int handle)
{
    normalize_mwo();
    if ((oid_ & OID_CATEGORY_MASK) == OID_MIDDLEWEIGHT) {
        return ::find_object(OBJECTD)->remove_mwo_callout(oid_, handle);
    } else {
        return ::remove_call_out(handle);
    }
}

/*
 * NAME:        _F_mwo_callout()
 * DESCRIPTION: dispatch a callout in a middle-weight object
 */
nomask void _F_mwo_callout(string function, mixed *arguments)
{
    if (previous_program() == OWNER_NODE) {
        object  this;
        string  program;

        this = this_object();
        program = ::function_object(function, this);

        /*
         * Ensure that it is still safe to call the function. This object may
         * have been recompiled after the delayed call was added.
         */
        if (program && (creator(program) != "System" || function == "create"))
        {
            call_other(this, function, arguments...);
        }
    }
}
