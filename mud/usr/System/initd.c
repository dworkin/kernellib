# include <config.h>
# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/tls.h>
# include <system/object.h>
# include <system/system.h>

private inherit api_rsrc  API_RSRC;
private inherit api_tls   API_TLS;

/*
 * NAME:        load
 * DESCRIPTION: find or compile an object
 */
private object load(string path)
{
    object obj;

    obj = find_object(path);
    if (!obj) {
        obj = compile_object(path);
    }
    if (obj) {
	call_other(obj, "???");
    }
    return obj;
}

/*
 * NAME:        create
 * DESCRIPTION: initialize system initd
 */
static void create()
{
    int      i, size;
    string  *owners, path;

    api_rsrc::create();
    api_tls::create();

    DRIVER->message("Initializing system.\n");

    /* reserve TLS space for create() arguments */
    api_tls::set_tls_size(1);

    /* install object manager */
    DRIVER->set_object_manager(load(OBJECTD));

    load(ENTOBJ);
    load(PROXY);

    /* initialize user code */
    owners = api_rsrc::query_owners() - ({ nil, "System" });
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
