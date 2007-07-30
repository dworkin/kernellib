# include <system/assert.h>
# include <system/object.h>

private object objectd;

static void create()
{
    objectd = find_object(OBJECTD);
}

static mapping get_program_dir(string path)
{
    ASSERT_ARG(path);
    DEBUG_ASSERT(objectd);
    return objectd->get_program_dir(path);
}
