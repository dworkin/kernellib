# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/path.h>

static string creator(string path)
{
    DEBUG_ASSERT(path);
    return DRIVER->creator(path);
}

static string normalize(string path, varargs string dir, string uid)
{
    object driver;

    DEBUG_ASSERT(path);
    driver = find_object(DRIVER);
    if (!dir) {
        string oname;

        oname = ::object_name(::this_object());
        dir = oname + "/..";
        uid = driver->creator(oname);
    }
    return driver->normalize_path(path, dir, uid);
}

static string master(string path)
{
    DEBUG_ASSERT(path);
    sscanf(path, "%s#", path);
    return path;
}

static int number(string path)
{
    int number;

    DEBUG_ASSERT(path);
    sscanf(path, "%*s#%d", number);
    return number;
}

static int type(string path)
{
    DEBUG_ASSERT(path);
    if (sscanf(path, "%*s" + INHERITABLE_SUBDIR)) {
	return PT_INHERITABLE;
    } else if (sscanf(path, "%*s" + CLONABLE_SUBDIR)) {
	/* clonable path cannot have lightweight subdirectory */
	return sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR) ? PT_DEFAULT
	    : PT_CLONABLE;
    } else if (sscanf(path, "%*s" + LIGHTWEIGHT_SUBDIR)) {
	return PT_LIGHTWEIGHT;
    } else {
	return PT_DEFAULT;
    }
}
