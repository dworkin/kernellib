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
 * NAME:	creator()
 * DESCRIPTION:	get creator of file
 */
private string creator(string path)
{
    DEBUG_ASSERT(path);
    return ::find_object(DRIVER)->creator(path);
}

/*
 * NAME:        normalize_path()
 * DESCRIPTION:	normalize a path relative to this object
 */
private string normalize_path(string path)
{
    string  name;
    object  driver;

    DEBUG_ASSERT(path);
    driver = ::find_object(DRIVER);
    name = object_name(this_object());
    return driver->normalize_path(path, name + "/..", driver->creator(name));
}

private void undefined_error(string name, mapping undefined)
{
    string  *programs, **functions;
    int      i, size;

    DEBUG_ASSERT(name);
    DEBUG_ASSERT(undefined);
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
 * NAME:        create()
 * DESCRIPTION: dummy initialization function
 */
static void create(mixed args...)
{ }

/*
 * NAME:        _F_system_create()
 * DESCRIPTION: system creator function
 */
nomask int _F_system_create(varargs int clone)
{
    string name;

    ASSERT_ACCESS(previous_program() == AUTO);
    name = object_name(this_object());
    if (clone) {
        object   objectd;
	mixed   *args;

        sscanf(name, "%*s#%d", oid_);
        objectd = ::find_object(OBJECTD);
	args = objectd->get_tlvar(SYSTEM_TLS_CREATE_ARGS);
	if (args) {
            /* pass arguments to create() */
	    DEBUG_ASSERT(typeof(args) == T_ARRAY);
	    objectd->set_tlvar(SYSTEM_TLS_CREATE_ARGS, nil);
	    call_limited("create", args...);
	} else {
            /* no arguments */
	    call_limited("create");
	}
    } else {
        mapping undefined;

        undefined = status(this_object())[O_UNDEFINED];
        if (undefined) {
            undefined_error(name, undefined);
        }

        /* do not call create() for clonable or light-weight master objects */
        if (!sscanf(name, "%*s" + CLONABLE_SUBDIR)
            && !sscanf(name, "%*s" + LIGHTWEIGHT_SUBDIR))
        {
            oid_ = ::status(this_object())[O_INDEX];
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
    if (oid_ < -1
        && (!environment_ || environment_->_F_find(oid_) != this_object()))
    {
        oid_ = -1;
        environment_ = nil;
    }
    return oid_;
}

/*
 * NAME:        _F_move()
 * DESCRIPTION: move this object to another environment
 */
nomask void _F_move(object dest)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    _Q_oid();  /* update environment */

    if (oid_ >= 0) {
        object this, obj;

        this = this_object();
        for (obj = dest; obj; obj = obj->_Q_env()) {
            if (obj == this) {
                error("Cannot move object into itself");
            }
        }
    }

    if (environment_ != nil) {
        DEBUG_ASSERT(oid_ != -1);
        environment_->_F_leave(oid_);
    }

    environment_ = dest;
    if (environment_ != nil) {
        if (oid_ == -1) {
            oid_ = ::find_object(OBJECTD)->add_data(query_owner(),
                                                    environment_);
        } else if (oid_ < -1) {
            ::find_object(OBJECTD)->move_data(oid_, environment_);
        }
        environment_->_F_enter(oid_, this_object());
    } else if (oid_ < -1) {
        ::find_object(OBJECTD)->move_data(oid_, nil);
    }
}

/*
 * NAME:        _F_enter()
 * DESCRIPTION: add an object to the inventory of this object
 */
nomask void _F_enter(int oid, object obj)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(obj);
    if (inventory_ == nil) {
        inventory_ = ([ ]);
    }
    DEBUG_ASSERT(!inventory_[oid]);
    inventory_[oid] = obj;
}

/*
 * NAME:        _F_leave()
 * DESCRIPTION: remove an object from the inventory of this object
 */
nomask void _F_leave(int oid)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(inventory_ && inventory_[oid]);
    inventory_[oid] = nil;
}

/*
 * NAME:        _F_find()
 * DESCRIPTION: find a distinct LWO by number in this object
 */
nomask object _F_find(int oid)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO
                  || previous_program() == OWNEROBJ);
    DEBUG_ASSERT(oid < -1);
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
    _Q_oid(); /* update environment */
    return environment_;
}

/*
 * NAME:        object_number()
 * DESCRIPTION: return the object number of an object
 */
int object_number(object obj)
{
    ASSERT_ARG(obj && obj <- SYSTEM_AUTO);
    return obj->_Q_oid();
}

/*
 * NAME:        find_object()
 * DESCRIPTION: find an object by name or number
 */
static object find_object(mixed oname)
{
    int     ptype, onumber;
    string  master;
    object  obj;

    switch (typeof(oname)) {
    case T_INT:
        /* find object by number */
        if (oname < 0) {
            /* distinct LWO */
            return ::find_object(OBJECTD)->find_data(oname);
        } else {
            /* persistent object */
            obj = ::find_object(OBJECTD)->find_obj(oname);
        }
        break;

    case T_STRING:
        /* find object by name */
        obj = ::find_object(oname);
        break;

    default:
        ASSERT_ARG(FALSE);
    }
    if (!obj) {
        return nil;
    }

    /* cannot find clonable or light-weight master objects */
    oname = object_name(obj);
    return sscanf(oname, "%*s#") || !sscanf(oname, "%*s" + CLONABLE_SUBDIR)
        && !sscanf(oname, "%*s" + LIGHTWEIGHT_SUBDIR) ? obj : nil;
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
static atomic object clone_object(string master, mixed args...)
{
    ASSERT_ARG_1(master);

    /* pass arguments to create() via TLS */
    if (sizeof(args)) {
	::find_object(OBJECTD)->set_tlvar(SYSTEM_TLS_CREATE_ARGS, args);
    }

    return ::clone_object(master);
}

/*
 * NAME:        new_object()
 * DESCRIPTION: create or copy a light-weight object
 */
static atomic object new_object(mixed master, mixed args...)
{
    if (typeof(master) == T_STRING) {  /* create new LWO */
        /* pass arguments to create() via TLS */
	if (sizeof(args)) {
            ::find_object(OBJECTD)->set_tlvar(SYSTEM_TLS_CREATE_ARGS, args);
	}
    } else {  /* copy existant LWO */
        ASSERT_ARG_1(typeof(master) == T_OBJECT
                     && sscanf(object_name(master), "%*s#-1") == 1);

        /* cannot pass arguments when copying LWO */
        ASSERT_MESSAGE(sizeof(args) == 0, "Cannot pass arguments");
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
            if (oid < -1) {
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
static int move_object(object obj, object dest)
{
    object env;

    ASSERT_ARG_1(obj);
    ASSERT_ARG_2(!dest || !sscanf(object_name(dest), "%*s#-1"));
    obj->_F_move(dest);
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
static int call_out(string func, mixed delay, mixed args...)
{
    string prog;

    ASSERT_ARG_1(func);
    prog = ::function_object(func, this_object());
    ASSERT_ARG_1(prog && creator(prog) != "System");
    ASSERT_ARG_2(typeof(delay) == T_INT || typeof(delay) == T_FLOAT);
    _Q_oid(); /* update environment */
    if (oid_ < -1) {
        return ::find_object(OBJECTD)->data_callout(oid_, func, delay, args);
    }
    return ::call_out(func, delay, args...);
}

/*
 * NAME:        remove_call_out()
 * DESCRIPTION: remove a scheduled function call
 */
static mixed remove_call_out(int handle)
{
    _Q_oid(); /* update environment */
    if (oid_ < -1) {
        return ::find_object(OBJECTD)->remove_data_callout(oid_, handle);
    }
    return ::remove_call_out(handle);
}

/*
 * NAME:        _F_call_data()
 * DESCRIPTION: dispatch a scheduled function call
 */
nomask void _F_call_data(string func, mixed *args)
{
    string prog;

    ASSERT_ACCESS(previous_program() == OWNEROBJ);
    DEBUG_ASSERT(oid_ < -1);
    prog = ::function_object(func, this_object());

    /*
     * Make sure that it is still safe to call the function. This object may
     * have been recompiled after the call was scheduled.
     */
    if (prog && (creator(prog) != "System" || func == "create")) {
        call_other(this_object(), func, args...);
    }
}
