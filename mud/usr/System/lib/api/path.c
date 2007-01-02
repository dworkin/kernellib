#include <kernel/kernel.h>

static string creator(string path) {
    return DRIVER->creator(path);
}

static string normalize(string path, varargs string dir, string uid) {
    object driver;

    driver = find_object(DRIVER);
    if (!dir) {
        string oname;

        oname = ::object_name(::this_object());
        dir = oname + "/..";
        uid = driver->creator(oname);
    }
    return driver->normalize_path(path, dir, uid);
}

static string master(string path) {
    sscanf(path, "%s#", path);
    return path;
}

static int number(string path) {
    int number;

    sscanf(path, "%*s#%d", number);
    return number;
}
