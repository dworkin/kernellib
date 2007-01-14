# include <status.h>
# include <type.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/path.h>
# include <system/object.h>
# include <system/system.h>
# include <system/tls.h>

private inherit path UTIL_PATH;

private int      oid_;
private object   env_;
private mapping  inv_;

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

        oid_ = path::number(oname);

	args = ::call_other(OBJECTD, "get_tlvar", SYSTEM_TLS_CREATE_ARGS);
	if (args) {
	    DEBUG_ASSERT(typeof(args) == T_ARRAY);
	    ::call_other(OBJECTD, "set_tlvar", SYSTEM_TLS_CREATE_ARGS, nil);
	    call_limited("create", args...);
	} else {
	    call_limited("create");
	}
    } else if (ptype == PT_DEFAULT) {
        oid_ = ::status(this_object())[O_INDEX];

	call_limited("create");
    }

    /* kernel creator function should not call create() */
    return TRUE;
}

nomask int _Q_oid()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    if (oid_ < -1
        && (!env_ || ::call_other(env_, "_F_find", oid_) != this_object()))
    {
        oid_ = -1;
        env_ = nil;
    }
    return oid_;
}

nomask void _F_move(object env)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    _Q_oid(); /* update environment */
    if (env_) {
        DEBUG_ASSERT(oid_ != -1);
        ::call_other(env_, "_F_leave", oid_);
    }

    env_ = env;
    if (env) {
        if (oid_ == -1) {
            oid_ = ::call_other(OBJECTD, "add_data", query_owner(), env);
        } else if (oid_ < -1) {
            ::call_other(OBJECTD, "move_data", oid_, env);
        }
        ::call_other(env, "_F_enter", oid_, this_object());
    } else if (oid_ < -1) {
        ::call_other(OBJECTD, "move_data", oid_, nil);
    }
}

nomask void _F_enter(int oid, object obj)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(obj);
    if (!inv_) {
        inv_ = ([ ]);
    }
    DEBUG_ASSERT(!inv_[oid]);
    inv_[oid] = obj;
}

nomask void _F_leave(int oid)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(inv_ && inv_[oid]);
    inv_[oid] = nil;
}

nomask object _F_find(int oid)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO
                  || previous_program() == OWNEROBJ);
    DEBUG_ASSERT(oid < -1);
    return (inv_) ? inv_[oid] : nil;
}

nomask object *_Q_inv()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    return inv_ ? map_values(inv_) : ({ });
}

nomask object _Q_env()
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    _Q_oid(); /* update environment */
    return env_;
}

static mixed call_other(mixed obj, string func, mixed args...)
{
    string prog;

    /*
     * resolve and validate object
     */
    ASSERT_ARG_1(obj);
    if (typeof(obj) == T_STRING) {
        int ptype, oid;

        obj = path::normalize(obj);
	ptype = path::type(obj);
        oid = path::number(obj);
        if (ptype == PT_LIGHTWEIGHT && oid < -1) {
            string master;
            
            /*
             * find managed light-weight object
             */
            master = path::master(obj);
            obj = ::call_other(OBJECTD, "find_data", oid);
            ASSERT_ARG_1(obj && path::master(::object_name(obj)) != master);
        } else {
            ASSERT_ARG_1(ptype == PT_DEFAULT || ptype == PT_CLONABLE && oid);
	    obj = ::find_object(obj);
            ASSERT_ARG_1(obj);
	}
    } else {
        ASSERT_ARG_1(typeof(obj) == T_OBJECT);
        if (path::type(::object_name(obj)) == PT_LIGHTWEIGHT) {
            ::call_other(obj, "_Q_oid"); /* update environment */
        }
    }

    /* 
     * function must be callable
     */
    ASSERT_ARG_2(func);
    prog = ::function_object(func, obj);
    ASSERT_ARG_2(prog && path::creator(prog) != "System");

    return ::call_other(obj, func, args...);
}

static object find_object(string oname)
{
    int     oid, ptype;
    object  obj;

    ASSERT_ARG(oname);
    oname = path::normalize(oname);
    ptype = path::type(oname);
    oid = path::number(oname);
    if (ptype == PT_LIGHTWEIGHT && oid < -1) {
        obj = ::call_other(OBJECTD, "find_data", oid);
        return (obj && path::master(::object_name(obj)) == path::master(oname))
            ? obj : nil;
    }

    /* cannot find inheritable objects or clonable master objects */
    return (ptype == PT_DEFAULT || ptype == PT_CLONABLE && oid)
	? ::find_object(oname) : nil;
}

static void message(string message)
{
    ASSERT_ARG(message);
    ::call_other(DRIVER, "message",
		 previous_program() + ": " + message + "\n");
}

static atomic object clone_object(string master, varargs mixed args...)
{
    ASSERT_ARG_1(master);

    /*
     * pass create() arguments using TLS
     */
    if (sizeof(args)) {
	::call_other(OBJECTD, "set_tlvar", SYSTEM_TLS_CREATE_ARGS, args);
    }

    return ::clone_object(master);
}

static atomic object new_object(mixed master, varargs mixed args...)
{
    if (typeof(master) == T_STRING) {
        /*
         * pass create() arguments using TLS
         */
	if (sizeof(args)) {
            if (sizeof(args)) {
                ::call_other(OBJECTD, "set_tlvar", SYSTEM_TLS_CREATE_ARGS,
                             args);
            }
	}
    } else {
        ASSERT_ARG_1(typeof(master) == T_OBJECT
                     && path::number(::object_name(master)) == -1);

        /*
         * cannot pass arguments when copying
         */
        ASSERT_MESSAGE(!sizeof(args), "Cannot pass arguments");
    }
    return ::new_object(master);
}

static string object_name(object obj)
{
    string oname;

    ASSERT_ARG(obj);
    oname = ::object_name(obj);
    if (path::number(oname) == -1) {
        return path::master(oname) + "#" + ::call_other(obj, "_Q_oid");
    }
    return oname;
}

/*
 * TODO: find inventory light-weight object by name
 * TODO: call-out information for inventory light-weight object
 */
static mixed *status(varargs mixed obj)
{
    int     oid;
    mixed  *status;

    if (typeof(obj) == T_STRING) {
        int ptype;

        obj = path::normalize(obj);
        ptype = path::type(obj);
        oid = path::number(obj);
        if (ptype == PT_LIGHTWEIGHT && oid < -1) {
            string master;

            master = path::master(obj);
            obj = ::call_other(OBJECTD, "find_data", oid);
            if (!obj || path::master(::object_name(obj)) != master) {
                return nil;
            }
        }
    } else if (typeof(obj) == T_OBJECT) {
        oid = ::call_other(obj, "_Q_oid");
    }
    status = ::status(obj);
    if (oid < -1) {
        DEBUG_ASSERT(status);
        status[O_CALLOUTS] = ::call_other(OBJECTD, "query_data_callouts",
                                          query_owner(), oid);
    }
    return status;
}

static void move_object(object obj, object env)
{
    ASSERT_ARG_1(obj);
    ASSERT_ARG_2(!env || path::number(::object_name(env)) != -1);
    ::call_other(obj, "_F_move", env);
}

static object environment(object obj)
{
    ASSERT_ARG(obj);
    return ::call_other(obj, "_Q_env");
}

static object *inventory(object obj)
{
    ASSERT_ARG(obj);
    return ::call_other(obj, "_Q_inv");
}

static atomic object compile_object(string path, varargs string source)
{
    object obj;

    ASSERT_ARG_1(path);
    path = path::normalize(path);
    obj = ::compile_object(path, source);
    if (obj && status(obj)[O_UNDEFINED]) {
	error("Non-inheritable object cannot be abstract");
    }

    /* hide clonable and light-weight master objects */
    return (obj && path::type(path) == PT_DEFAULT) ? obj : nil;
}

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

static int call_out(string func, mixed delay, mixed args...)
{
    string prog;

    ASSERT_ARG_1(func);
    prog = ::function_object(func, this_object());
    ASSERT_ARG_1(prog && path::creator(prog) != "System");
    ASSERT_ARG_2(typeof(delay) == T_INT || typeof(delay) == T_FLOAT);
    DEBUG_ASSERT(oid_ == _Q_oid());
    if (oid_ < -1) {
        return ::call_other(OBJECTD, "data_callout", oid_, func, delay, args);
    }
    return ::call_out(func, delay, args...);
}

static mixed remove_call_out(int handle)
{
    DEBUG_ASSERT(oid_ == _Q_oid());
    if (oid_ < -1) {
        return ::call_other(OBJECTD, "remove_data_callout", oid_, handle);
    }
    return ::remove_call_out(handle);
}

nomask void _F_call_data(string func, mixed *args)
{
    string prog;

    ASSERT_ACCESS(previous_program() == OWNEROBJ);
    prog = ::function_object(func, this_object());
    if (prog && path::creator(prog) != "System") {
        ::call_other(this_object(), func, args...);
    }
}
