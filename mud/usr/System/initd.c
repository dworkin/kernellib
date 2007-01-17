# include <config.h>
# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/tls.h>
# include <kernel/user.h>
# include <system/object.h>
# include <system/path.h>
# include <system/system.h>
# include <system/user.h>

private inherit rsrc  API_RSRC;
private inherit tls   API_TLS;

/*
 * NAME:        load()
 * DESCRIPTION: find or compile an object
 */
private object load(string path)
{
    object obj;

    obj = find_object(path);
    if (!obj && !status(path)) {
        obj = compile_object(path);
    }
    if (obj) {
	call_other(obj, "???");
    }
    return obj;
}

/*
 * NAME:        create()
 * DESCRIPTION: initialize system
 */
static void create()
{
    int      i, size;
    string  *owners, path;
    object   objectd, telnetd;

    rsrc::create();
    tls::create();

    DRIVER->message("Initializing system.\n");

    /* reserve a TLS slot for create() arguments */
    tls::set_tls_size(1);

    /* install object manager */
    objectd = load(OBJECTD);
    load(OWNEROBJ);
    DRIVER->set_object_manager(objectd);

    /* install telnet manager */
    telnetd = load(SYSTEM_TELNETD);
    load(SYSTEM_USER);
    load(SYSTEM_WIZTOOL);
    USERD->set_telnet_manager(0, telnetd);

    /* initialize user code */
    owners = rsrc::query_owners() - ({ nil, "System" });
    size = sizeof(owners);
    for (i = 0; i < size; ++i) {
        path = USR_DIR + "/" + owners[i] + "/initd";
        if (DRIVER->file_size(path + ".c")) {
	    /*
	     * the increased TLS size does not affect the current execution
	     * round, so initialize user code in call-outs
	     */
	    call_out("init", 0, path);
        }
    }
}

/*
 * NAME:        init
 * DESCRIPTION: create user initd after call-out
 */
static void init(string path)
{
    DRIVER->message("Initializing " + path + ".\n");
    load(path);
}
