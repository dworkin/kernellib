# include <status.h>
# include <type.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/path.h>
# include <system/object.h>
# include <system/system.h>
# include <system/tls.h>

private inherit path UTIL_PATH;

private int      onumber_;  /* object number */
private object   env_;      /* environment */
private mapping  inv_;      /* ([ int onumber: object obj ]) */

/*
 * NAME:        create()
 * DESCRIPTION: dummy initialization function
 */
static void create(varargs mixed args...)
{ }

/*
 * NAME:        _F_system_create()
 * DESCRIPTION: system creator function
 */
nomask int _F_system_create(varargs int clone)
{
    int     ptype;
    string  oname;

    ASSERT_ACCESS(previous_program() == AUTO);
    oname = ::object_name(this_object());
    ptype = path::type(oname);
    if (clone) {
	mixed *args;

        onumber_ = path::number(oname);

	args = ::call_other(OBJECTD, "get_tlvar", SYSTEM_TLS_CREATE_ARGS);
	if (args) {
            /* pass arguments to create() */
	    DEBUG_ASSERT(typeof(args) == T_ARRAY);
	    ::call_other(OBJECTD, "set_tlvar", SYSTEM_TLS_CREATE_ARGS, nil);
	    call_limited("create", args...);
	} else {
            /* no arguments */
	    call_limited("create");
	}
    } else if (ptype == PT_DEFAULT) {
        onumber_ = ::status(this_object())[O_INDEX];

	call_limited("create"); 
    }

    /* kernel creator function should not call create() */
    return TRUE;
}

/*
 * NAME:        _Q_number()
 * DESCRIPTION: return the object number for this object
 */
nomask int _Q_number()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    if (onumber_ <= -2
        && (!env_ || ::call_other(env_, "_F_find", onumber_) != this_object()))
    {
        onumber_ = -1;
        env_ = nil;
    }
    return onumber_;
}

/*
 * NAME:        _F_move()
 * DESCRIPTION: move this object to another environment
 */
nomask void _F_move(object env)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    _Q_number();  /* update environment */
    if (env_) {
        DEBUG_ASSERT(onumber_ != -1);
        ::call_other(env_, "_F_leave", onumber_);
    }

    env_ = env;
    if (env) {
        if (onumber_ == -1) {
            onumber_ = ::call_other(OBJECTD, "add_data", query_owner(), env);
        } else if (onumber_ <= -2) {
            ::call_other(OBJECTD, "move_data", onumber_, env);
        }
        ::call_other(env, "_F_enter", onumber_, this_object());
    } else if (onumber_ <= -2) {
        ::call_other(OBJECTD, "move_data", onumber_, nil);
    }
}

/*
 * NAME:        _F_enter()
 * DESCRIPTION: add an object to the inventory of this object
 */
nomask void _F_enter(int onumber, object obj)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    DEBUG_ASSERT(obj);
    if (!inv_) {
        inv_ = ([ ]);
    }
    DEBUG_ASSERT(!inv_[onumber]);
    inv_[onumber] = obj;
}

/*
 * NAME:        _F_leave()
 * DESCRIPTION: remove an object from the inventory of this object
 */
nomask void _F_leave(int onumber)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    DEBUG_ASSERT(inv_ && inv_[onumber]);
    inv_[onumber] = nil;
}

/*
 * NAME:        _F_find()
 * DESCRIPTION: find a distinct LWO by number in this object
 */
nomask object _F_find(int onumber)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO
                  || previous_program() == OWNEROBJ);
    DEBUG_ASSERT(onumber <= -2);
    return (inv_) ? inv_[onumber] : nil;
}

/*
 * NAME:        _Q_inv()
 * DESCRIPTION: return the inventory of this object
 */
nomask object *_Q_inv()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    return inv_ ? map_values(inv_) : ({ });
}

/*
 * NAME:        _Q_env()
 * DESCRIPTION: return the environment of this object
 */
nomask object _Q_env()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    _Q_number();  /* update environment */
    return env_;
}

/*
 * NAME:        call_other()
 * DESCRIPTION: call a named function in an object
 */
static mixed call_other(mixed obj, string func, mixed args...)
{
    string prog;

    /* resolve and validate object */
    ASSERT_ARG_1(obj);
    if (typeof(obj) == T_STRING) {
        int ptype, onumber;

        obj = path::normalize(obj);
	ptype = path::type(obj);
        onumber = path::number(obj);
        if (ptype == PT_LIGHTWEIGHT && onumber <= -2) {
            string master;
            
            /* find distinct LWO */
            master = path::master(obj);
            obj = ::call_other(OBJECTD, "find_data", onumber);
            ASSERT_ARG_1(obj && path::master(::object_name(obj)) == master);
        } else {
            ASSERT_ARG_1(ptype == PT_DEFAULT
                         || ptype == PT_CLONABLE && onumber);
	    obj = ::find_object(obj);
            ASSERT_ARG_1(obj);
	}
    } else {
        ASSERT_ARG_1(typeof(obj) == T_OBJECT);
        if (path::type(::object_name(obj)) == PT_LIGHTWEIGHT) {
            ::call_other(obj, "_Q_number");  /* update environment */
        }
    }

    /* function must be callable */
    ASSERT_ARG_2(func);
    prog = ::function_object(func, obj);
    ASSERT_ARG_2(prog && (path::creator(prog) != "System"
                          || func == "create"));

    return ::call_other(obj, func, args...);
}

/*
 * NAME:        object_number()
 * DESCRIPTION: return the object number of an object
 */
int object_number(object obj)
{
    ASSERT_ARG(obj);
    return obj <- SYSTEM_AUTO ? ::call_other(obj, "_Q_number") : 0;
}

/*
 * NAME:        find_object()
 * DESCRIPTION: find an object by name or number
 */
static object find_object(mixed oname)
{
    int     onumber, ptype;
    object  obj;

    /* find object by number */
    if (typeof(oname) == T_INT) {
        if (oname <= -2) {  /* distinct LWO */
            return ::call_other(OBJECTD, "find_data", oname);
        } else if (oname >= 1) {  /* persistent object */
            return ::call_other(OBJECTD, "find_obj", oname);
        } else {
            return nil;
        }
    }

    ASSERT_ARG(typeof(oname) == T_STRING);
    oname = path::normalize(oname);
    ptype = path::type(oname);
    onumber = path::number(oname);

    /*
     * Find a distinct LWO by name: First attempt to find it by number, then
     * verify that the master name matches.
     */
    if (ptype == PT_LIGHTWEIGHT && onumber <= -2) {
        obj = ::call_other(OBJECTD, "find_data", onumber);
        return (obj && path::master(::object_name(obj)) == path::master(oname))
            ? obj : nil;
    }

    /* cannot find inheritable objects or clonable master objects */
    return (ptype == PT_DEFAULT || ptype == PT_CLONABLE && onumber)
	? ::find_object(oname) : nil;
}

/*
 * NAME:        message()
 * DESCRIPTION: write a message to the console
 */
static void message(string message)
{
    ASSERT_ARG(message);
    ::call_other(DRIVER, "message",
		 previous_program() + ": " + message + "\n");
}

/*
 * NAME:        clone_object()
 * DESCRIPTION: create a new persistent clone
 */
static atomic object clone_object(string master, varargs mixed args...)
{
    ASSERT_ARG_1(master);

    /* pass arguments to create() via TLS */
    if (sizeof(args)) {
	::call_other(OBJECTD, "set_tlvar", SYSTEM_TLS_CREATE_ARGS, args);
    }

    return ::clone_object(master);
}

/*
 * NAME:        new_object()
 * DESCRIPTION: create or copy a light-weight object
 */
static atomic object new_object(mixed master, varargs mixed args...)
{
    if (typeof(master) == T_STRING) {  /* create new LWO */
        /* pass arguments to create() via TLS */
	if (sizeof(args)) {
            ::call_other(OBJECTD, "set_tlvar", SYSTEM_TLS_CREATE_ARGS,
                         args);
	}
    } else {  /* copy existant LWO */
        ASSERT_ARG_1(typeof(master) == T_OBJECT
                     && path::number(::object_name(master)) == -1);

        /* cannot pass arguments when copying LWO */
        ASSERT_MESSAGE(!sizeof(args), "Cannot pass arguments");
    }
    return ::new_object(master);
}

/*
 * NAME:        object_name()
 * DESCRIPTION: return the object name of an object
 */
static string object_name(object obj)
{
    string oname;

    ASSERT_ARG(obj);
    oname = ::object_name(obj);
    if (path::number(oname) == -1) {
        return path::master(oname) + "#" + ::call_other(obj, "_Q_number");
    }
    return oname;
}

/*
 * NAME:        status()
 * DESCRIPTION: return information about the system or an object
 */
static mixed *status(varargs mixed obj)
{
    int     onumber;
    mixed  *status;

    if (typeof(obj) == T_STRING) {
        int ptype;

        obj = path::normalize(obj);
        ptype = path::type(obj);
        onumber = path::number(obj);
        if (ptype == PT_LIGHTWEIGHT && onumber <= -2) {
            string master;

            master = path::master(obj);
            obj = ::call_other(OBJECTD, "find_data", onumber);
            if (!obj || path::master(::object_name(obj)) != master) {
                return nil;
            }
        }
    } else if (typeof(obj) == T_OBJECT) {
        onumber = ::call_other(obj, "_Q_number");
    }
    status = ::status(obj);
    if (onumber <= -2) {
        DEBUG_ASSERT(status);
        status[O_CALLOUTS] = ::call_other(OBJECTD, "query_data_callouts",
                                          query_owner(), onumber);
    }
    return status;
}

/*
 * NAME:        move_object()
 * DESCRIPTION: move an object to another environment
 */
static void move_object(object obj, object env)
{
    ASSERT_ARG_1(obj);
    ASSERT_ARG_2(!env || path::number(::object_name(env)) != -1);
    ::call_other(obj, "_F_move", env);
}

/*
 * NAME:        environment()
 * DESCRIPTION: return the environment of an object
 */
static object environment(object obj)
{
    ASSERT_ARG(obj);
    return ::call_other(obj, "_Q_env");
}

/*
 * NAME:        inventory()
 * DESCRIPTION: return the inventory of an object
 */
static object *inventory(object obj)
{
    ASSERT_ARG(obj);
    return ::call_other(obj, "_Q_inv");
}

/*
 * NAME:        compile_object()
 * DESCRIPTION: compile an object
 */
static atomic object compile_object(string path, varargs string source)
{
    object obj;

    ASSERT_ARG_1(path);
    path = path::normalize(path);
    obj = ::compile_object(path, source);
    if (obj && status(obj)[O_UNDEFINED]) {
	error("Non-inheritable object cannot have undefined functions");
    }

    /* hide clonable and light-weight master objects */
    return (obj && path::type(path) == PT_DEFAULT) ? obj : nil;
}

/*
 * NAME:        get_dir()
 * DESCRIPTION: list the contents of a directory
 */
static mixed **get_dir(string path)
{
    int      ptype;
    mixed  **list;

    ASSERT_ARG(path);
    path = path::normalize(path);
    list = ::get_dir(path);

    /* hide clonable and light-weight master objects */
    ptype = path::type(path);
    if (ptype == PT_CLONABLE || ptype == PT_LIGHTWEIGHT) {
        int i, size;

        for (i = 0; i < size; ++i) {
            if (list[3][i]) {
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
    path = path::normalize(path);
    info = ::file_info(path);
    if (typeof(info[2]) == T_OBJECT) {
        int ptype;

        /* hide clonable and light-weight master objects */
        ptype = path::type(::object_name(info[2]));
        if (ptype == PT_CLONABLE || ptype == PT_LIGHTWEIGHT) {
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
    ASSERT_ARG_1(prog && path::creator(prog) != "System");
    ASSERT_ARG_2(typeof(delay) == T_INT || typeof(delay) == T_FLOAT);
    DEBUG_ASSERT(onumber_ == _Q_number());
    if (onumber_ <= -2) {
        return ::call_other(OBJECTD, "data_callout", onumber_, func, delay,
                            args);
    }
    return ::call_out(func, delay, args...);
}

/*
 * NAME:        remove_call_out()
 * DESCRIPTION: remove a scheduled function call
 */
static mixed remove_call_out(int handle)
{
    DEBUG_ASSERT(onumber_ == _Q_number());
    if (onumber_ <= -2) {
        return ::call_other(OBJECTD, "remove_data_callout", onumber_, handle);
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
    DEBUG_ASSERT(onumber_ <= -2);
    prog = ::function_object(func, this_object());

    /*
     * Make sure that it is still safe to call the function. This object may
     * have been recompiled since the call was scheduled.
     */
    if (prog && (path::creator(prog) != "System" || func == "create")) {
        ::call_other(this_object(), func, args...);
    }
}
