# include <type.h>
# include <kernel/kernel.h>
# include <kernel/tls.h>
# include <system/assert.h>
# include <system/path.h>
# include <system/object.h>
# include <system/system.h>

/*
 * To do: Consider a more space-efficient alternative than inheriting this API,
 * because it adds a variable to more or less every object in the mud.
 */
private inherit tls API_TLS;

private inherit path UTIL_PATH;

private int      oid_;
private object   proxy_;
private object   env_;
private mapping  inv_;

static void create(varargs mixed args...)
{ }

nomask int _F_system_create(varargs int clone)
{
    ASSERT_ACCESS(::previous_program() == AUTO);
    tls::create();
    if (clone) {
        string   oname;
        object   this;
	mixed   *args;

        this = ::this_object();
        oname = ::object_name(this);
        if (path::number(oname) == -1
            && sscanf(oname, "%*s" + DISTINCT_LWO_SUBDIR))
        {
            oid_ = ::call_other(OBJECTD, "new_dlwo", this);
            proxy_ = ::new_object(PROXY);
            ::call_other(proxy_, "init", oid_);
        }

	args = tls::get_tlvar(0);
	if (args) {
	    DEBUG_ASSERT(typeof(args) == T_ARRAY);
	    tls::set_tlvar(0, nil);
	    call_limited("create", args...);
	} else {
	    call_limited("create");
	}
	return TRUE;
    }
    return FALSE;
}

nomask int _Q_oid()
{
    ASSERT_ACCESS(::previous_program() == SYSTEM_AUTO);
    return oid_;
}

nomask object _Q_proxy()
{
    ASSERT_ACCESS(::previous_program() == SYSTEM_AUTO);
    return proxy_;
}

nomask void _F_move(object env)
{
    ASSERT_ACCESS(::previous_program() == SYSTEM_AUTO);
    if (env_) {
        ::call_other(env_, "_F_leave", oid_);
    }
    if (env_ = env) {
        ::call_other(env, "_F_enter", oid_, ::this_object());
    }
    if (proxy_) {
        ::call_other(OBJECTD, "move_dlwo", oid_, env ? env : ::this_object());
    }
}

nomask void _F_enter(int oid, object obj)
{
    ASSERT_ACCESS(::previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(obj);
    if (!inv_) inv_ = ([ ]);
    DEBUG_ASSERT(!inv_[oid]);
    inv_[oid] = obj;
}

nomask void _F_leave(int oid)
{
    ASSERT_ACCESS(::previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    DEBUG_ASSERT(inv_ && inv_[oid]);
    inv_[oid] = nil;
}

nomask object _F_find(int oid)
{
    ASSERT_ACCESS(::previous_program() == PROXY
                  || ::previous_program() == OBJNODE);
    DEBUG_ASSERT(oid);
    return inv_ ? inv_[oid] : nil;
}

nomask object *_Q_inv()
{
    int      i, size;
    object  *inv;

    ASSERT_ACCESS(::previous_program() == SYSTEM_AUTO);
    if (!inv_) return ({ });
    inv = map_values(inv_);
    size = sizeof(inv);
    for (i = 0; i < size; ++i) {
        if (path::number(::object_name(inv[i])) == -1) {
            inv[i] = ::call_other(inv[i], "_Q_proxy");
            DEBUG_ASSERT(inv[i]);
        }
    }
    return inv_ ? map_values(inv_) : ({ });
}

nomask object _Q_env()
{
    ASSERT_ACCESS(::previous_program() == PROXY
                  || ::previous_program() == SYSTEM_AUTO);
    return env_;
}

static mixed call_other(mixed obj, string func, mixed args...)
{
    ASSERT_ARG_1(obj);
    ASSERT_ARG_2(func);
    if (typeof(obj) == T_OBJECT && ::object_name(obj) == PROXY + "#-1") {
        obj = ::call_other(obj, "find");
        ASSERT_ARG_1(obj);
    } else if (typeof(obj) == T_STRING) {
        int oid;

        obj = path::normalize(obj);
        oid = path::number(obj);
        if (oid < -1) {
            /* find distinct LWO */
            obj = ::call_other(OBJECTD, "find_dlwo", oid);
            ASSERT_ARG_1(obj);
        }
    }

    /* function must be callable */
    ASSERT_ARG_2(::function_object(func, obj));
    return ::call_other(obj, func, args...);
}

static int destruct_object(mixed obj)
{
    if (typeof(obj) == T_OBJECT && ::object_name(obj) == PROXY + "#-1") {
        int oid;

        obj = ::call_other(obj, "find");
        ASSERT_ARG(obj);
        oid = ::call_other(obj, "_Q_oid");
        DEBUG_ASSERT(oid <= -2);
        ::call_other(obj, "_F_move", nil);
        ::call_other(OBJECTD, "destruct_dlwo", oid);
        return TRUE;
    }
    if (typeof(obj) == T_STRING) {
        int     oid;
        string  oname;

        oname = path::normalize(obj);
        oid = path::number(oname);
        if (oid <= -2) {
            obj = ::call_other(OBJECTD, "find_dlwo", oid);
            if (!obj) return FALSE;
            ::call_other(obj, "_F_move", nil);
            ::call_other(OBJECTD, "destruct_dlwo", oid);
            return TRUE;
        }
    }
    return ::destruct_object(obj);
}

static object find_object(mixed oname)
{
    if (typeof(oname) == T_OBJECT && ::object_name(oname) == PROXY + "#-1") {
        /* validate proxy */
        return ::call_other(oname, "find") ? oname : nil;
    }

    if (typeof(oname) == T_STRING) {
        int oid;

        oname = path::normalize(oname);
        oid = path::number(oname);
        if (oid < -1) {
            object obj;

            /* find distinct LWO, returning by proxy */
            obj = ::call_other(OBJECTD, "find_dlwo", oid);
            return obj ? ::call_other(obj, "_Q_proxy") : nil;
        }
    }

    return ::find_object(oname);
}

static string function_object(string func, object obj)
{
    ASSERT_ARG_1(func);
    ASSERT_ARG_2(obj);
    if (::object_name(obj) == PROXY + "#-1") {
        obj = ::call_other(obj, "find");
        ASSERT_ARG_2(obj);
    }
    return ::function_object(func, obj);
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
    if (sizeof(args)) {
	tls::set_tlvar(0, args);
    }
    return ::clone_object(master);
}

static atomic object new_object(mixed obj, varargs mixed args...)
{
    if (typeof(obj) == T_STRING) {
	if (sizeof(args)) {
	    tls::set_tlvar(0, args);
	}
        obj = ::new_object(obj);
        if (sscanf(::object_name(obj), "%*s" + DISTINCT_LWO_SUBDIR)) {
            return ::call_other(obj, "_Q_proxy");
        }
        return obj;
    }
    return ::new_object(obj);
}

static string object_name(object obj)
{
    ASSERT_ARG(obj);
    if (::object_name(obj) == PROXY + "#-1") {
        int oid;

        obj = ::call_other(obj, "find");
        ASSERT_ARG(obj);
        oid = ::call_other(obj, "_Q_oid");
        DEBUG_ASSERT(oid <= -2);
        return path::master(::object_name(obj)) + "#" + oid;
    }
    return ::object_name(obj);
}

static object previous_object(varargs int n)
{
    object obj;

    if (obj = ::previous_object(n)) {
        string oname;

        oname = ::object_name(obj);
        if (path::number(oname) == -1
            && sscanf(oname, "%*s" + DISTINCT_LWO_SUBDIR))
        {
            obj = ::call_other(obj, "_Q_proxy");
        }
    }
    return obj;
}

static mixed *status(varargs mixed obj)
{
    if (typeof(obj) == T_OBJECT && ::object_name(obj) == PROXY + "#-1") {
        obj = ::call_other(obj, "find");
        if (!obj) return nil;
    } else if (typeof(obj) == T_STRING) {
        int oid;

        obj = path::normalize(obj);
        oid = path::number(obj);
        if (oid < -1) {
            /* find distinct LWO */
            obj = ::call_other(OBJECTD, "find_dlwo", oid);
            if (!obj) return nil;
        }
    }
    return ::status(obj);
}

static object this_object()
{
    return proxy_ ? proxy_ : ::this_object();
}

static void move_object(object obj, object env)
{
    ASSERT_ARG_1(obj);
    ASSERT_ARG_2(!env || ::object_name(env) != PROXY + "#-1");
    if (::object_name(obj) == PROXY + "#-1") {
        obj = ::call_other(obj, "find");
        ASSERT_ARG_1(obj);
    }
    ::call_other(obj, "_F_move", env);
}

static object environment(object obj)
{
    ASSERT_ARG(obj);
    if (::object_name(obj) == PROXY + "#-1") {
        obj = ::call_other(obj, "find");
        ASSERT_ARG(obj);
    }
    return ::call_other(obj, "_Q_env");
}

static object *inventory(object obj)
{
    ASSERT_ARG(obj);
    if (::object_name(obj) == PROXY + "#-1") {
        obj = ::call_other(obj, "find");
        ASSERT_ARG(obj);
    }
    return ::call_other(obj, "_Q_inv");
}
