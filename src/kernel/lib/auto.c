# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/access.h>
# include <kernel/user.h>
# include <status.h>
# include <type.h>
# include <trace.h>
# include <kfun.h>

# define TLS()			::call_trace()[1][TRACE_FIRSTARG]
# define CHECKARG(arg, n, func)	if (!(arg)) badarg((n), (func))


/*
 * NAME:	badarg()
 * DESCRIPTION:	called when an argument check failed
 */
private void badarg(int n, string func)
{
    error("Bad argument " + n + " for function " + func);
}


private string creator, owner;	/* creator and owner of this object */

/*
 * NAME:	query_owner()
 * DESCRIPTION:	query the owner of an object
 */
nomask string query_owner()
{
    return owner;
}

void create() { }		/* default high-level create function */

# ifdef CREATOR
int CREATOR() { return FALSE; }	/* default System-level creator function */
# endif

/*
 * NAME:	_F_create()
 * DESCRIPTION:	kernel creator function
 */
nomask void _F_create()
{
    if (!creator) {
	string oname;
	object driver;
	int clone;

	rlimits (-1; -1) {
	    /*
	     * set creator and owner
	     */
	    oname = object_name(this_object());
	    driver = ::find_object(DRIVER);
	    creator = driver->creator(oname);
	    if (sscanf(oname, "%s#%d", oname, clone) != 0) {
		owner = TLSVAR(TLS(), TLS_ARGUMENT);
		if (clone >= 0) {
		    /*
		     * register object
		     */
		    driver->clone(this_object(), owner);
		}
	    } else {
		owner = creator;
	    }
	    if (!creator) {
		creator = "";
	    }
	}

# ifdef CREATOR
	/* call System-level creator function */
	if (CREATOR()) {
	    return;
	}
# endif
	create();
    }
}


/*
 * NAME:	find_object()
 * DESCRIPTION:	find an object
 */
static object find_object(string path)
{
    CHECKARG(path, 1, "find_object");
    if (!this_object()) {
	return nil;
    }

    path = ::find_object(DRIVER)->normalize_path(path, nil);
    if (sscanf(path, "%*s/lib/") != 0) {
	/*
	 * It is not possible to find a class object by name, or to call a
	 * function in it.
	 */
	return nil;
    }
    return ::find_object(path);
}

/*
 * NAME:	destruct_object()
 * DESCRIPTION:	destruct an object
 */
static int destruct_object(mixed obj)
{
    object driver;
    string oname, oowner;
    int clone, lib;

    /* check and translate argument */
    driver = ::find_object(DRIVER);
    if (typeof(obj) == T_STRING) {
	if (!this_object()) {
	    return FALSE;
	}
	obj = ::find_object(driver->normalize_path(obj, nil, creator));
	if (!obj) {
	    return FALSE;
	}
    } else {
	CHECKARG(typeof(obj) == T_OBJECT, 1, "destruct_object");
	if (!this_object()) {
	    return FALSE;
	}
    }

    /*
     * check privileges
     */
    oname = object_name(obj);
    clone = sscanf(oname, "%s#%d", oname, lib);
    if (clone && lib < 0) {
	error("Cannot destruct non-persistent object");
    }
    lib = sscanf(oname, "%*s/lib/");
    oowner = (lib || sscanf(oname, "%*s#") == 0) ?
	      driver->creator(oname) : obj->query_owner();
    if ((sscanf(oname, "/kernel/%*s") != 0 && !lib && !KERNEL()) ||
	(creator != "System" && owner != oowner)) {
	error("Cannot destruct object: not owner");
    }

    rlimits (-1; -1) {
	if (clone) {
	    /*
	     * non-clones are handled by driver->remove_program()
	     */
	    ::find_object(RSRCD)->rsrc_incr(oowner, "objects", -1);
	} else {
	    driver->destruct(object_name(obj), oowner);
	}
	::destruct_object(obj);
    }
    return TRUE;
}

/*
 * NAME:	_compile()
 * DESCRIPTION:	reversible low-level compile
 */
private atomic object _compile(object driver, string path, string uid,
			       string *source)
{
    int add;
    object obj;

    driver->compiling(path);
    add = !::find_object(path);
    obj = ::compile_object(path, source...);
    if (add) {
	::find_object(RSRCD)->rsrc_incr(uid, "objects", 1);
    }
    driver->compile(path, uid, source);
    return obj;
}

/*
 * NAME:	compile_object()
 * DESCRIPTION:	compile a master object
 */
static object compile_object(string path, string source...)
{
    string uid, err;
    object driver, obj;
    int lib, kernel;

    CHECKARG(path, 1, "compile_object");
    if (!this_object()) {
	error("Permission denied");
    }

    /*
     * check access
     */
    driver = ::find_object(DRIVER);
    path = driver->normalize_path(path, nil, creator);
    lib = sscanf(path, "%*s/lib/");
    kernel = sscanf(path, "/kernel/%*s");
    uid = driver->creator(path);
    if ((sizeof(source) != 0 && kernel) ||
	(creator != "System" &&
	 !::find_object(ACCESSD)->access(object_name(this_object()), path,
					 ((lib || !uid) &&
					  sizeof(source) == 0 && !kernel) ?
					  READ_ACCESS : WRITE_ACCESS))) {
	error("Access denied");
    }

    /*
     * do the compiling
     */
    rlimits (-1; -1) {
	err = catch(obj = _compile(driver, path, uid, source));
	if (err) {
	    driver->compile_failed(path, uid);
	    error(err);
	}
    }

    return (lib) ? nil : obj;
}

/*
 * NAME:	_clone()
 * DESCRIPTION:	reversible low-level clone
 */
private atomic object _clone(string path, string uid, object obj)
{
    if (path != RSRCOBJ) {
	::find_object(RSRCD)->rsrc_incr(uid, "objects", 1);
    }
    TLSVAR(TLS(), TLS_ARGUMENT) = uid;
    return ::clone_object(obj);
}

/*
 * NAME:	clone_object()
 * DESCRIPTION:	clone an object
 */
static object clone_object(string path, varargs string uid)
{
    string creator;
    object obj;

    CHECKARG(path, 1, "clone_object");
    if (uid) {
	CHECKARG(KERNEL() || SYSTEM(), 1, "clone_object");
    } else {
	uid = owner;
    }
    if (!this_object()) {
	error("Permission denied");
    }

    /*
     * check access
     */
    obj = ::find_object(DRIVER);
    creator = obj->creator(object_name(this_object()));
    path = obj->normalize_path(path, nil, creator);
    if ((sscanf(path, "/kernel/%*s") != 0 && !KERNEL()) ||
	(creator != "System" &&
	 !::find_object(ACCESSD)->access(object_name(this_object()), path,
					 READ_ACCESS))) {
	/*
	 * kernel objects can only be cloned by kernel objects, and cloning
	 * in general requires read access
	 */
	error("Access denied");
    }

    /*
     * check if object can be cloned
     */
    if (!owner || !(obj=::find_object(path)) || sscanf(path, "%*s#") != 0 ||
	sscanf(path, "%*s/lib/") != 0) {
	/*
	 * no owner for clone, master object not compiled, or not clonable
	 */
	error("Cannot clone " + path);
    }

    /*
     * do the cloning
     */
    return _clone(path, uid, obj);
}

/*
 * NAME:	_new()
 * DESCRIPTION:	reversible low-level new
 */
private atomic object _new(object obj)
{
    return ::new_object(obj);
}

/*
 * NAME:	new_object()
 * DESCRIPTION:	create a new non-persistent object
 */
static object new_object(mixed obj, varargs string uid)
{
    string creator, str;
    int create;

    if (!this_object()) {
	error("Permission denied");
    }
    switch (typeof(obj)) {
    case T_STRING:
	str = obj;
	obj = ::find_object(DRIVER);
	creator = obj->creator(object_name(this_object()));
	str = obj->normalize_path(str, nil, creator);
	obj = ::find_object(str);
	create = TRUE;
	break;

    case T_OBJECT:
	str = object_name(obj);
	if (sscanf(str, "%*s#-1") != 0) {
	    create = FALSE;
	    break;
	}
	/* fall through */
    default:
	badarg(1, "new_object");
    }
    if (uid) {
	CHECKARG(create && SYSTEM(), 1, "new_object");
    } else {
	uid = owner;
    }

    /*
     * create the object
     */
    if (create) {
	/*
	 * check access
	 */
	if (sscanf(str, "/kernel/%*s") != 0 ||
	    (creator != "System" &&
	     !::find_object(ACCESSD)->access(object_name(this_object()), str,
					     READ_ACCESS))) {
	    error("Access denied");
	}

	/*
	 * check if object can be created
	 */
	if (!obj || sscanf(str, "%*s/lib/") != 0) {
	    /*
	     * master object not compiled, or not suitable
	     */
	    error("Cannot create new instance of " + str);
	}

	TLSVAR(TLS(), TLS_ARGUMENT) = uid;
    }
    return _new(obj);
}

/*
 * NAME:	process_trace()
 * DESCRIPTION:	filter out function call arguments from a call trace
 */
private mixed *process_trace(mixed *trace, string creator, object driver)
{
    if (sizeof(trace) > TRACE_FIRSTARG &&
	creator != driver->creator(trace[TRACE_PROGNAME])) {
	/* remove arguments */
	return trace[.. TRACE_FIRSTARG - 1];
    }
    return trace;
}

/*
 * NAME:	call_trace()
 * DESCRIPTION:	function call trace
 */
static mixed *call_trace(varargs mixed index)
{
    object driver;
    string creator;
    mixed *trace;

    driver = ::find_object(DRIVER);
    creator = driver->creator(object_name(this_object()));
    if (index == nil) {
	trace = ::call_trace();
	if (previous_program() != RSRCOBJ) {
	    trace[1][TRACE_FIRSTARG] = nil;
	}
	if (creator != "System") {
	    int i;

	    for (i = sizeof(trace) - 1; --i >= 0; ) {
		trace[i] = process_trace(trace[i], creator, driver);
	    }
	}
    } else {
	trace = ::call_trace()[index];
	if (index == 1 && previous_program() != RSRCOBJ) {
	    trace[TRACE_FIRSTARG] = nil;
	}
	if (creator != "System") {
	    trace = process_trace(trace, creator, driver);
	}
    }

    return trace;
}

/*
 * NAME:	process_callouts()
 * DESCRIPTION:	process callouts in a status(obj) return value
 */
private mixed **process_callouts(object obj, mixed **callouts)
{
    int i;
    string oname;
    mixed *co;

    if (callouts && (i=sizeof(callouts)) != 0) {
	oname = object_name(obj);
	if (sscanf(oname, "/kernel/%*s") != 0) {
	    /* can't see callouts in kernel objects */
	    return ({ });
	} else if (obj != this_object() && creator != "System" &&
		   (!owner || owner != obj->query_owner())) {
	    /* remove arguments from callouts */
	    do {
		--i;
		co = callouts[i];
		callouts[i] = ({ co[CO_HANDLE], co[CO_FIRSTXARG],
				 co[CO_DELAY] });
	    } while (i != 0);
	} else {
	    do {
		--i;
		co = callouts[i];
		callouts[i] = ({ co[CO_HANDLE], co[CO_FIRSTXARG],
				 co[CO_DELAY] }) +
			      co[CO_FIRSTXARG + 2];
	    } while (i != 0);
	}
    }

    return callouts;
}

/*
 * NAME:	status()
 * DESCRIPTION:	get information about an object
 */
static mixed status(varargs mixed obj, mixed index)
{
    mixed status;
    object driver;

    if (!this_object()) {
	return nil;
    }

    switch (typeof(obj)) {
    case T_NIL:
	CHECKARG(index == nil, 1, "status");
	status = ::status();
	if (status[ST_STACKDEPTH] >= 0) {
	    status[ST_STACKDEPTH]++;
	}
	break;

    case T_INT:
	if (obj == -1) {
	    return (index == nil) ? ::status(1) : ::status(1)[index];
	} else {
	    CHECKARG(index == nil, 1, "status");
	    status = ::status()[obj];
	    if (obj == ST_STACKDEPTH && status >= 0) {
		status++;
	    }
	}
	break;

    case T_STRING:
	/* get corresponding object */
	driver = ::find_object(DRIVER);
	obj = ::find_object(driver->normalize_path(obj, nil));
	if (!obj) {
	    return nil;
	}
	/* fall through */
    case T_OBJECT:
	switch (typeof(index)) {
	case T_NIL:
	    status = ::status(obj);
	    status[O_CALLOUTS] = process_callouts(obj, status[O_CALLOUTS]);
	    break;

	case T_INT:
	    status = ::status(obj)[index];
	    if (index == O_CALLOUTS) {
		status = process_callouts(obj, status);
	    }
	    break;

	default:
	    badarg(2, "status");
	}
	break;
    }

    return status;
}

/*
 * NAME:	this_user()
 * DESCRIPTION:	return the user object and not a connection object
 */
static object this_user()
{
    object user;

    user = ::this_user();
    if (!user) {
	user = TLSVAR(TLS(), TLS_USER);
    }
    if (user) {
	user = user->query_user();
    }
    return user;
}

/*
 * NAME:	users()
 * DESCRIPTION:	return an array with the current user objects
 */
static object *users()
{
    if (!this_object()) {
	return nil;
    } else if (object_name(this_object()) == USERD) {
	/* connection objects */
	return ::users();
    } else {
	return ::find_object(USERD)->query_users();
    }
}

/*
 * NAME:	connect()
 * DESCRIPTION:	establish an outbound connection
 */
static void connect(string address, int port)
{
    if (previous_program() != BINARY_CONN) {
	error("Permission denied");
    }
    ::connect(address, port);
}

/*
 * NAME:	connect_datagram()
 * DESCRIPTION:	establish an outbound datagram connection
 */
static void connect_datagram(int dgram, string address, int port)
{
    if (previous_program() != DATAGRAM_CONN) {
	error("Permission denied");
    }
# ifdef KF_CONNECT_DATAGRAM
    ::connect_datagram(dgram, address, port);
# else
    ::call_out("unconnected", 0, 0);
# endif
}

/*
 * NAME:	swapout()
 * DESCRIPTION:	swap out all objects
 */
static void swapout()
{
    if (creator != "System") {
	error("Permission denied");
    }
    ::swapout();
}

/*
 * NAME:	dump_state()
 * DESCRIPTION:	create snapshot
 */
static void dump_state(varargs int incr)
{
    if (creator != "System" || !this_object()) {
	error("Permission denied");
    }
    rlimits (-1; -1) {
	::find_object(DRIVER)->prepare_reboot();
	::dump_state(incr);
    }
}

/*
 * NAME:	shutdown()
 * DESCRIPTION:	shutdown the system
 */
static void shutdown(varargs int hotboot)
{
    if (creator != "System" || !this_object()) {
	error("Permission denied");
    }
    rlimits (-1; -1) {
	::shutdown(hotboot);
	::find_object(DRIVER)->message("System halted.\n");
    }
}

/*
 * NAME:	call_touch()
 * DESCRIPTION:	arrange to be warned when a function is called in an object
 */
static int call_touch(object obj)
{
    if (creator != "System") {
	error("Permission denied");
    }
    return ::call_touch(obj);
}


/*
 * NAME:	_F_call_limited()
 * DESCRIPTION:	call a function with limited stack depth and ticks
 */
private mixed _F_call_limited(mixed arg1, string oowner, mixed *args)
{
    object rsrcd;
    int stack, ticks;
    string func;
    mixed tls, *limits, result;

    rsrcd = ::find_object(RSRCD);
    func = arg1;
    stack = ::status()[ST_STACKDEPTH];
    ticks = ::status()[ST_TICKS];
    rlimits (-1; -1) {
	tls = TLS();
	if (tls == arg1) {
	    tls = arg1 = ([ ]);
	}
	limits = TLSVAR(tls, TLS_LIMIT) =
		 rsrcd->call_limits(TLSVAR(tls, TLS_LIMIT), oowner, stack,
				    ticks);
    }

    rlimits (limits[LIM_MAXSTACK]; limits[LIM_MAXTICKS]) {
	result = call_other(this_object(), func, args...);

	ticks = ::status()[ST_TICKS];
	rlimits (-1; -1) {
	    rsrcd->update_ticks(limits, ticks);
	    TLSVAR(tls, TLS_LIMIT) = limits[LIM_NEXT];

	    return result;
	}
    }
}

/*
 * NAME:	call_limited()
 * DESCRIPTION:	call a function with the current object owner's resource limits
 */
static mixed call_limited(string func, mixed args...)
{
    CHECKARG(func, 1, "call_limited");
    if (!this_object()) {
	return nil;
    }
    CHECKARG(function_object(func, this_object()) != AUTO || func == "create",
	     1, "call_limited");

    return _F_call_limited(func, owner, args);
}

/*
 * NAME:	call_out()
 * DESCRIPTION:	start a callout
 */
static int call_out(string func, mixed delay, mixed args...)
{
    int type;
    string oname;

    CHECKARG(func, 1, "call_out");
    type = typeof(delay);
    CHECKARG(type == T_INT || type == T_FLOAT, 2, "call_out");
    if (!this_object()) {
	return 0;
    }
    CHECKARG(function_object(func, this_object()) != AUTO || func == "create",
	     1, "call_out");
    oname = object_name(this_object());
    if (sscanf(oname, "%*s#-1") != 0) {
	error("Callout in non-persistent object");
    }

    /*
     * add callout
     */
    if (sscanf(oname, "/kernel/%*s") != 0) {
	/* direct callouts for kernel objects */
	return ::call_out(func, delay, args...);
    }
    return ::call_out("_F_callout", delay, func, owner, args);
}

/*
 * NAME:	_F_callout()
 * DESCRIPTION:	callout gate
 */
nomask void _F_callout(string func, string oowner, mixed *args)
{
    if (!previous_program()) {
	_F_call_limited(func, oowner, args);
    }
}

/*
 * NAME:	call_out_other()
 * DESCRIPTION:	start a callout in another object
 */
static int call_out_other(object obj, string func, mixed delay, mixed args...)
{
    int type;
    string oname;
    mixed *limits;

    CHECKARG(obj, 1, "call_out_other");
    CHECKARG(func, 2, "call_out_other");
    type = typeof(delay);
    CHECKARG(type == T_INT || type == T_FLOAT, 3, "call_out_other");
    if (!this_object()) {
	return 0;
    }
    oname = function_object(func, obj);
    CHECKARG(oname && (sscanf(oname, "/kernel/%*s") == 0 || func == "create"),
	     2, "call_out_other");
    oname = object_name(obj);
    if (sscanf(oname, "%*s#-1") != 0) {
	error("Callout in non-persistent object");
    }

    /*
     * add callout
     */
    limits = TLSVAR(TLS(), TLS_LIMIT);
    return obj->_F_callout_other(delay, func,
				 (limits) ? limits[LIM_OWNER] : "System", args);
}

/*
 * NAME:	_F_callout_other()
 * DESCRIPTION:	callout_other gate
 */
nomask int _F_callout_other(mixed delay, string func, string oowner,
			    mixed *args)
{
    if (previous_program() == AUTO) {
	return ::call_out("_F_callout", delay, func, oowner, args);
    }
}


/*
 * NAME:	read_file()
 * DESCRIPTION:	read a string from a file
 */
static string read_file(string path, varargs int offset, int size)
{
    CHECKARG(path, 1, "read_file");
    if (!this_object()) {
	error("Permission denied");
    }

    path = ::find_object(DRIVER)->normalize_path(path, nil, creator);
    if (creator != "System" &&
	!::find_object(ACCESSD)->access(object_name(this_object()), path,
					READ_ACCESS)) {
	error("Access denied");
    }

    return ::read_file(path, offset, size);
}

/*
 * NAME:	write_file()
 * DESCRIPTION:	write a string to a file
 */
static int write_file(string path, string str, varargs int offset)
{
    string fcreator;
    object driver, rsrcd;
    int size, result, *rsrc;

    CHECKARG(path, 1, "write_file");
    CHECKARG(str, 2, "write_file");
    if (!this_object()) {
	error("Permission denied");
    }

    driver = ::find_object(DRIVER);
    path = driver->normalize_path(path, nil, creator);
    if (sscanf(path, "/kernel/%*s") != 0 ||
	sscanf(path, "/include/kernel/%*s") != 0 ||
	(creator != "System" &&
	 !::find_object(ACCESSD)->access(object_name(this_object()), path,
					 WRITE_ACCESS))) {
	error("Access denied");
    }

    fcreator = driver->creator(path);
    rsrcd = ::find_object(RSRCD);
    rsrc = rsrcd->rsrc_get(fcreator, "fileblocks");
    if (creator != "System" && rsrc[RSRC_USAGE] >= rsrc[RSRC_MAX] &&
	rsrc[RSRC_MAX] >= 0) {
	error("File quota exceeded");
    }

    size = driver->file_size(path);
    catch {
	rlimits (-1; -1) {
	    result = ::write_file(path, str, offset);
	    if (result != 0 && (size=driver->file_size(path) - size) != 0) {
		rsrcd->rsrc_incr(fcreator, "fileblocks", size);
	    }
	}
    } : error(TLSVAR(TLS(), TLS_ERROR));

    return result;
}

/*
 * NAME:	remove_file()
 * DESCRIPTION:	remove a file
 */
static int remove_file(string path)
{
    object driver;
    int size, result;

    CHECKARG(path, 1, "remove_file");
    if (!this_object()) {
	error("Permission denied");
    }

    driver = ::find_object(DRIVER);
    path = driver->normalize_path(path, nil, creator);
    if (sscanf(path, "/kernel/%*s") != 0 ||
	sscanf(path, "/include/kernel/%*s") != 0 ||
	(creator != "System" &&
	 !::find_object(ACCESSD)->access(object_name(this_object()), path,
					 WRITE_ACCESS))) {
	error("Access denied");
    }

    size = driver->file_size(path);
    catch {
	rlimits (-1; -1) {
	    result = ::remove_file(path);
	    if (result != 0 && size != 0) {
		::find_object(RSRCD)->rsrc_incr(driver->creator(path),
						"fileblocks", -size);
	    }
	}
    } : error(TLSVAR(TLS(), TLS_ERROR));
    return result;
}

/*
 * NAME:	rename_file()
 * DESCRIPTION:	rename a file
 */
static int rename_file(string from, string to)
{
    string oname, fcreator, tcreator;
    object driver, accessd, rsrcd;
    int size, result, *rsrc;

    CHECKARG(from, 1, "rename_file");
    CHECKARG(to, 2, "rename_file");
    if (!this_object()) {
	error("Permission denied");
    }

    oname = object_name(this_object());
    driver = ::find_object(DRIVER);
    from = driver->normalize_path(from, oname + "/..", creator);
    to = driver->normalize_path(to, oname + "/..", creator);
    accessd = ::find_object(ACCESSD);
    if (sscanf(from + "/", "/kernel/%*s") != 0 ||
	sscanf(to, "/kernel/%*s") != 0 ||
	sscanf(from + "/", "/include/kernel/%*s") != 0 || from == "/include" ||
	sscanf(to, "/include/kernel/%*s") != 0 ||
	(creator != "System" &&
	 (!accessd->access(oname, from, WRITE_ACCESS) ||
	  !accessd->access(oname, to, WRITE_ACCESS)))) {
	error("Access denied");
    }

    fcreator = driver->creator(from);
    tcreator = driver->creator(to);
    size = driver->file_size(from, TRUE);
    rsrcd = ::find_object(RSRCD);
    rsrc = rsrcd->rsrc_get(tcreator, "fileblocks");
    if (size != 0 && fcreator != tcreator && creator != "System" &&
	rsrc[RSRC_USAGE] >= rsrc[RSRC_MAX] && rsrc[RSRC_MAX] >= 0) {
	error("File quota exceeded");
    }

    catch {
	rlimits (-1; -1) {
	    result = ::rename_file(from, to);
	    if (result != 0 && fcreator != tcreator) {
		rsrcd->rsrc_incr(tcreator, "fileblocks", size);
		rsrcd->rsrc_incr(fcreator, "fileblocks", -size);
	    }
	}
    } : error(TLSVAR(TLS(), TLS_ERROR));
    return result;
}

/*
 * NAME:	get_dir()
 * DESCRIPTION:	get a directory listing
 */
static mixed **get_dir(string path)
{
    string *names, dir;
    mixed **list, *olist;
    int i, sz;

    CHECKARG(path, 1, "get_dir");
    if (!this_object()) {
	error("Permission denied");
    }

    path = ::find_object(DRIVER)->normalize_path(path, nil, creator);
    if (creator != "System" &&
	!::find_object(ACCESSD)->access(object_name(this_object()), path,
					READ_ACCESS)) {
	error("Access denied");
    }

    list = ::get_dir(path);
    names = explode(path, "/");
    dir = implode(names[.. sizeof(names) - 2], "/");
    names = list[0];
    olist = allocate(sz = sizeof(names));
    if (sscanf(path, "%*s/lib/") != 0) {
	/* class objects */
	for (i = sz; --i >= 0; ) {
	    path = dir + "/" + names[i];
	    if ((sz=strlen(path)) >= 2 && path[sz - 2 ..] == ".c" &&
		::find_object(path[.. sz - 3])) {
		olist[i] = TRUE;
	    }
	}
    } else {
	/* ordinary objects */
	for (i = sz; --i >= 0; ) {
	    object obj;

	    path = dir + "/" + names[i];
	    if ((sz=strlen(path)) >= 2 && path[sz - 2 ..] == ".c" &&
		(obj=::find_object(path[.. sz - 3]))) {
		olist[i] = obj;
	    }
	}
    }
    return list + ({ olist });
}

/*
 * NAME:	file_info()
 * DESCRIPTION:	get info for a single file
 */
static mixed *file_info(string path)
{
    object obj;
    mixed *info;
    int i, sz;

    CHECKARG(path, 1, "file_info");
    if (!this_object()) {
	error("Permission denied");
    }

    obj = ::find_object(DRIVER);
    path = obj->normalize_path(path, nil, creator);
    if (creator != "System" &&
	!::find_object(ACCESSD)->access(object_name(this_object()), path,
					READ_ACCESS)) {
	error("Access denied");
    }

    info = ::get_dir(obj->escape_path(path));
    if (sizeof(info[0]) == 0) {
	return nil;	/* file does not exist */
    }
    info = ({ info[1][i], info[2][i], nil });
    if ((sz=strlen(path)) >= 2 && path[sz - 2 ..] == ".c" &&
	(obj=::find_object(path[.. sz - 3]))) {
	info[2] = (sscanf(path, "%*s/lib/") != 0) ? TRUE : obj;
    }
    return info;
}

/*
 * NAME:	make_dir()
 * DESCRIPTION:	create a directory
 */
static int make_dir(string path)
{
    string fcreator;
    object driver, rsrcd;
    int result, *rsrc;

    CHECKARG(path, 1, "make_dir");
    if (!this_object()) {
	error("Permission denied");
    }

    driver = ::find_object(DRIVER);
    path = driver->normalize_path(path, nil, creator);
    if (sscanf(path, "/kernel/%*s") != 0 ||
	sscanf(path, "/include/kernel/%*s") != 0 ||
	(creator != "System" &&
	 !::find_object(ACCESSD)->access(object_name(this_object()), path,
					 WRITE_ACCESS))) {
	error("Access denied");
    }

    fcreator = driver->creator(path + "/");
    rsrcd = ::find_object(RSRCD);
    rsrc = rsrcd->rsrc_get(fcreator, "fileblocks");
    if (creator != "System" && rsrc[RSRC_USAGE] >= rsrc[RSRC_MAX] &&
	rsrc[RSRC_MAX] >= 0) {
	error("File quota exceeded");
    }

    catch {
	rlimits (-1; -1) {
	    result = ::make_dir(path);
	    if (result != 0) {
		rsrcd->rsrc_incr(fcreator, "fileblocks", 1);
	    }
	}
    } : error(TLSVAR(TLS(), TLS_ERROR));
    return result;
}

/*
 * NAME:	remove_dir()
 * DESCRIPTION:	remove a directory
 */
static int remove_dir(string path)
{
    object driver;
    int result;

    CHECKARG(path, 1, "remove_dir");
    if (!this_object()) {
	error("Permission denied");
    }

    driver = ::find_object(DRIVER);
    path = driver->normalize_path(path, nil, creator);
    if (sscanf(path, "/kernel/%*s") != 0 ||
	sscanf(path, "/include/kernel/%*s") != 0 ||
	(creator != "System" &&
	 !::find_object(ACCESSD)->access(object_name(this_object()), path,
					 WRITE_ACCESS))) {
	error("Access denied");
    }

    catch {
	rlimits (-1; -1) {
	    result = ::remove_dir(path);
	    if (result != 0) {
		::find_object(RSRCD)->rsrc_incr(driver->creator(path + "/"),
						"fileblocks", -1);
	    }
	}
    } : error(TLSVAR(TLS(), TLS_ERROR));
    return result;
}

/*
 * NAME:	restore_object()
 * DESCRIPTION:	restore the state of an object
 */
static int restore_object(string path)
{
    CHECKARG(path, 1, "restore_object");
    if (!this_object()) {
	error("Permission denied");
    }

    path = ::find_object(DRIVER)->normalize_path(path, nil, creator);
    if (creator != "System" &&
	!::find_object(ACCESSD)->access(object_name(this_object()), path,
					READ_ACCESS)) {
	error("Access denied");
    }

    return ::restore_object(path);
}

/*
 * NAME:	save_object()
 * DESCRIPTION:	save the state of an object
 */
static void save_object(string path)
{
    string oname, fcreator;
    object driver, rsrcd;
    int size, *rsrc;

    CHECKARG(path, 1, "save_object");
    if (!this_object()) {
	error("Permission denied");
    }

    oname = object_name(this_object());
    driver = ::find_object(DRIVER);
    path = driver->normalize_path(path, oname + "/..", creator);
    if ((sscanf(path, "/kernel/%*s") != 0 &&
	 sscanf(oname, "/kernel/%*s") == 0) ||
	sscanf(path, "/include/kernel/%*s") != 0 ||
	(creator != "System" &&
	 !::find_object(ACCESSD)->access(oname, path, WRITE_ACCESS))) {
	error("Access denied");
    }

    fcreator = driver->creator(path);
    rsrcd = ::find_object(RSRCD);
    rsrc = rsrcd->rsrc_get(fcreator, "fileblocks");
    if (creator != "System" && rsrc[RSRC_USAGE] >= rsrc[RSRC_MAX] &&
	rsrc[RSRC_MAX] >= 0) {
	error("File quota exceeded");
    }

    size = driver->file_size(path);
    catch {
	rlimits (-1; -1) {
	    ::save_object(path);
	    if ((size=driver->file_size(path) - size) != 0) {
		rsrcd->rsrc_incr(fcreator, "fileblocks", size);
	    }
	}
    } : error(TLSVAR(TLS(), TLS_ERROR));
}

/*
 * NAME:	editor()
 * DESCRIPTION:	pass a command to the editor
 */
static string editor(varargs string cmd)
{
    object rsrcd, driver;
    string result;
    mixed *info;

    if (creator != "System" || !this_object() ||
	sscanf(object_name(this_object()), "%*s#-1") != 0) {
	error("Permission denied");
    }

    catch {
	rlimits (-1; -1) {
	    rsrcd = ::find_object(RSRCD);
	    if (!query_editor(this_object())) {
		::find_object(USERD)->add_editor(this_object());
	    }
	    driver = ::find_object(DRIVER);

	    TLSVAR(TLS(), TLS_ARGUMENT) = nil;
	    result = (cmd) ? ::editor(cmd) : ::editor();
	    info = TLSVAR(TLS(), TLS_ARGUMENT);

	    if (!query_editor(this_object())) {
		::find_object(USERD)->remove_editor(this_object());
	    }
	    if (info) {
		rsrcd->rsrc_incr(driver->creator(info[0]), "fileblocks",
				 driver->file_size(info[0]) - info[1]);
	    }
	}
    } : error(TLSVAR(TLS(), TLS_ERROR));
    return result;
}


/*
 * NAME:	tls_set()
 * DESCRIPTION:	set TLS value
 */
static void tls_set(mixed index, mixed value)
{
    if (typeof(index) == T_INT && index < 0) {
	badarg(1, "tls_set");
    }
    TLS()[index] = value;
}

/*
 * NAME:	tls_get()
 * DESCRIPTION:	get TLS value
 */
static mixed tls_get(mixed index)
{
    if (typeof(index) == T_INT && index < 0) {
	badarg(1, "tls_get");
    }
    return TLS()[index];
}

/*
 * NAME:	send_atomic_message()
 * DESCRIPTION:	add a message to be passed on through an atomic error
 */
static void send_atomic_message(string str)
{
    mapping tls;
    string *mesg, *messages;

    if (!str || sscanf(str, "*%s\0") != 0) {
	badarg(1, "send_atomic_message");
    }

    mesg = ({ "." + str });
    tls = TLS();
    messages = TLSVAR(tls, TLS_PUT_ATOMIC);
    if (messages) {
	messages += mesg;
    } else {
	messages = mesg;
    }
    TLSVAR(tls, TLS_PUT_ATOMIC) = messages;
}

/*
 * NAME:	retrieve_atomic_messages()
 * DESCRIPTION:	retrieve messages after an atomic error
 */
static string *retrieve_atomic_messages()
{
    mapping tls;
    string *messages;

    tls = TLS();
    messages = TLSVAR(tls, TLS_GET_ATOMIC);
    TLSVAR(tls, TLS_GET_ATOMIC) = nil;
    return (messages) ? messages : ({ });
}

/*
 * NAME:	error()
 * DESCRIPTION:	throw an error
 */
static void error(string str)
{
    if (!str || sscanf(str, "%*s\0") != 0) {
	badarg(1, "error");
    }
    ::error(str);
}
