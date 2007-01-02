#include <kernel/kernel.h>
#include <kernel/rsrc.h>
#include <system/object.h>
#include <system/system.h>

private inherit api_rsrc API_RSRC;

private object load(string path) {
    object obj;

    obj = find_object(path);
    if (!obj) {
        obj = compile_object(path);
    }
    call_other(obj, "???");
    return obj;
}

static void create() {
    int i, size;
    string *owners, path;

    api_rsrc::create();

    /* install object manager */
    DRIVER->set_object_manager(load(OBJECTD));

    load(ENTOBJ);
    load(PROXY);

    /* initialize user code */
    owners = api_rsrc::query_owners() - ({ nil, "System" });
    size = sizeof(owners);
    for (i = 0; i < size; ++i) {
        path = "/usr/" + owners[i] + "/initd";
        if (DRIVER->file_size(path + ".c")) {
            catch {
                call_limited("init", path);
            }
        }
    }
}

static void init(string path) {
    load(path);
}
