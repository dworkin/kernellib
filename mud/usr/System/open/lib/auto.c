# include <status.h>
# include <type.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/system.h>

private int      oid_;          /* object number */
private object   environment_;  /* environment */
private mapping  inventory_;    /* ([ int oid: object obj ]) */

/*
 * NAME:	creator()
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
 * DESCRIPTION: return the object number of a persistent object
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
    } else {
        category = OID_MASTER;
        owner = creator(path);
        index = ::status(obj)[O_INDEX] + 1; /* use 1-based object indices */
    }
    uid = ::find_object(OBJECTD)->query_uid(owner);
    return category | (uid << OID_OWNER_OFFSET) | (index << OID_INDEX_OFFSET);
}

/*
 * NAME:        normalize_data()
 * DESCRIPTION: normalize a managed light-weight object
 */
private void normalize_data()
{
    if (oid_ < 0
        && (!environment_ || environment_->_F_find(oid_) != this_object()))
    {
        /* demote to unmanaged light-weight object */
        oid_ = 0;
        environment_ = nil;
    }
}

/*
 * NAME:        create()
 * DESCRIPTION: dummy initialization function
 */
static void create(mixed arguments...)
{ }

/*
 * NAME:        _F_system_create()
 * DESCRIPTION: system creator function
 */
nomask int _F_system_create(varargs int clone)
{
    ASSERT_ACCESS(previous_program() == AUTO);

    /* forbid non-inheritable objects with undefined functions */
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

	arguments = ::find_object(OBJECTD)->fetch_create_arguments();
	if (arguments) {
            /* pass arguments to create() */
	    call_limited("create", arguments...);
	} else {
            /* no arguments */
	    call_limited("create");
	}
    } else {
        string path;

        /* do not call create() for clonable or light-weight master objects */
        path = object_name(this_object());
        if (!sscanf(path, "%*s" + CLONABLE_SUBDIR)
            && !sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR))
        {
            call_limited("create"); 
        }
    }

    /* kernel creator function should not call create() */
    return TRUE;
}

/*
 * NAME:        _Q_oid()
 * DESCRIPTION: return the object number for this object
 */
nomask int _Q_oid()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    normalize_data();
    return oid_;
}

/*
 * NAME:        _F_move()
 * DESCRIPTION: move this object to another environment
 */
nomask void _F_move(object destination)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    normalize_data();

    if (oid_ > 0) {
        object this, obj;

        this = this_object();
        for (obj = destination; obj; obj = obj->_Q_environment()) {
            if (obj == this) {
                error("Cannot move object into itself");
            }
        }
    }

    if (environment_) {
        environment_->_F_leave(oid_);
    }
    environment_ = destination;
    if (oid_ < 0) {
        /* move managed light-weight object */
        ::find_object(OBJECTD)->move_data(oid_, environment_);
    } else if (!oid_ && environment_) {
        /* promote unmanaged light-weight object to managed */
        oid_ = ::find_object(OBJECTD)->add_data(query_owner(), environment_);
    }
    if (environment_) {
        environment_->_F_enter(oid_, this_object());
    }
}

/*
 * NAME:        _F_enter()
 * DESCRIPTION: add an object to the inventory of this object
 */
nomask void _F_enter(int oid, object obj)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    if (!inventory_) {
        inventory_ = ([ ]);
    }
    inventory_[oid] = obj;
}

/*
 * NAME:        _F_leave()
 * DESCRIPTION: remove an object from the inventory of this object
 */
nomask void _F_leave(int oid)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    inventory_[oid] = nil;
}

/*
 * NAME:        _F_find()
 * DESCRIPTION: find an object by number in this environment
 */
nomask object _F_find(int oid)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO
                  || previous_program() == OWNER_NODE);
    return inventory_ ? inventory_[oid] : nil;
}

/*
 * NAME:        _Q_inventory()
 * DESCRIPTION: return the inventory of this object
 */
nomask object *_Q_inventory()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    return inventory_ ? map_values(inventory_) : ({ });
}

/*
 * NAME:        _Q_environment()
 * DESCRIPTION: return the environment of this object
 */
nomask object _Q_environment()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    normalize_data();
    return environment_;
}

/*
 * NAME:        object_number()
 * DESCRIPTION: return the object number of an object
 */
static int object_number(object obj)
{
    ASSERT_ARG(obj);
    return obj <- SYSTEM_AUTO ? obj->_Q_oid() : _object_number(obj);
}

/*
 * NAME:        find_object()
 * DESCRIPTION: find an object by name or number
 */
static object find_object(mixed name)
{
    object  obj;

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
        ASSERT_ARG(FALSE);
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
 * NAME:        message()
 * DESCRIPTION: write a message to the console
 */
static void message(string message)
{
    ASSERT_ARG(message);
    ::find_object(DRIVER)->message(previous_program() + ": " + message + "\n");
}

/*
 * NAME:        clone_object()
 * DESCRIPTION: create a new persistent clone
 */
static atomic object clone_object(string master, mixed arguments...)
{
    ASSERT_ARG_1(master);

    /* pass arguments to create() via TLS */
    if (sizeof(arguments)) {
	::find_object(OBJECTD)->store_create_arguments(arguments);
    }

    return ::clone_object(master);
}

/*
 * NAME:        new_object()
 * DESCRIPTION: create or copy a light-weight object
 */
static atomic object new_object(mixed master, mixed arguments...)
{
    if (typeof(master) == T_STRING) {  /* create new LWO */
        /* pass arguments to create() via TLS */
	if (sizeof(arguments)) {
            ::find_object(OBJECTD)->store_create_arguments(arguments);
	}
    } else {  /* copy existant LWO */
        ASSERT_ARG_1(typeof(master) == T_OBJECT
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
            if (oid < 0) {
                obj = ::find_object(OBJECTD);
                status[O_CALLOUTS] = obj->query_data_callouts(query_owner(),
                                                              oid);
            }
        }
        return status;

    default:
        ASSERT_ARG(FALSE);
    }
}

/*
 * NAME:        move_object()
 * DESCRIPTION: move an object to another environment
 */
static int move_object(object obj, object destination)
{
    ASSERT_ARG_1(obj);
    ASSERT_ARG_2(!destination || !sscanf(object_name(destination), "%*s#-1"));
    if (destination && !destination->allow_enter(obj) || !obj
        || !obj->allow_move(destination) || !obj)
    {
        return FALSE;
    }
    obj->_F_move(destination);
    return TRUE;
}

/*
 * NAME:        environment()
 * DESCRIPTION: return the environment of an object
 */
static object environment(object obj)
{
    ASSERT_ARG(obj && obj <- SYSTEM_AUTO);
    return obj->_Q_environment();
}

/*
 * NAME:        inventory()
 * DESCRIPTION: return the inventory of an object
 */
static object *inventory(object obj)
{
    ASSERT_ARG(obj && obj <- SYSTEM_AUTO);
    return obj->_Q_inventory();
}

/*
 * NAME:        compile_object()
 * DESCRIPTION: compile an object
 */
static atomic object compile_object(string path, string source...)
{
    object   obj;
    mapping  undefined;

    ASSERT_ARG_1(path);
    obj = ::compile_object(path, source...);
    if (!obj) {
        return nil;
    }

    path = object_name(obj);
    undefined = status(obj)[O_UNDEFINED];
    if (undefined) {
        undefined_error(path, undefined);
    }

    /* hide clonable and light-weight master objects */
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

    ASSERT_ARG(path);
    path = normalize_path(path);
    list = ::get_dir(path);

    /* hide clonable and light-weight master objects */
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

    ASSERT_ARG(path);
    info = ::file_info(path);
    if (typeof(info[2]) == T_OBJECT) {
        /* hide clonable and light-weight master objects */
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
 * DESCRIPTION: schedule a function call
 */
static int call_out(string function, mixed delay, mixed arguments...)
{
    string program;

    ASSERT_ARG_1(function);
    program = ::function_object(function, this_object());
    ASSERT_ARG_1(program
                 && (creator(program) != "System" || function == "create"));
    ASSERT_ARG_2(typeof(delay) == T_INT || typeof(delay) == T_FLOAT);
    normalize_data();
    if (oid_ < 0) {
        return ::find_object(OBJECTD)->data_callout(oid_, function, delay,
                                                    arguments);
    } else {
        return ::call_out(function, delay, arguments...);
    }
}

/*
 * NAME:        remove_call_out()
 * DESCRIPTION: remove a scheduled function call
 */
static mixed remove_call_out(int handle)
{
    normalize_data();
    if (oid_ < 0) {
        return ::find_object(OBJECTD)->remove_data_callout(oid_, handle);
    } else {
        return ::remove_call_out(handle);
    }
}

/*
 * NAME:        _F_call_data()
 * DESCRIPTION: dispatch a scheduled function call
 */
nomask void _F_call_data(string function, mixed *arguments)
{
    object  this;
    string  program;

    ASSERT_ACCESS(previous_program() == OWNER_NODE);
    this = this_object();
    program = ::function_object(function, this);

    /*
     * Make sure that it is still safe to call the function. This object may
     * have been recompiled after the call was scheduled.
     */
    if (program && (creator(program) != "System" || function == "create")) {
        call_other(this, function, arguments...);
    }
}
