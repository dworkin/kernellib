# include <system/assert.h>
# include <system/object.h>

private object objectd_;

static void create()
{
    objectd_ = find_object(OBJECTD);
}

static mapping get_program_dir(string path)
{
    ASSERT_ARG(path);
    return objectd_->get_program_dir(path);
}
