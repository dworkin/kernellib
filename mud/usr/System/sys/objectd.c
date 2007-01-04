# include <status.h>
# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/path.h>
# include <system/system.h>

private inherit api_path  API_PATH;

int      onumber_;
mapping  ents_;

static void create()
{
    onumber_ = -2;
    ents_ = ([ ]);
}

string path_special(string compiled)
{
    ASSERT_ACCESS(previous_program() == DRIVER);
    return "/include/system/auto.h";
}

int forbid_inherit(string from, string path, int priv)
{
    ASSERT_ACCESS(previous_program() == DRIVER);

    /*
     * user objects cannot inherit system objects, except for objects in
     * ~System/open
     */
    return api_path::creator(from) != "System"
        && api_path::creator(path) == "System"
        && !sscanf(path, "/usr/System/open/%*s");
}

int new_ent(object ent)
{
    int onumber;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(ent);
    onumber = onumber_--;
    ents_[onumber] = ent;
    return onumber;
}

void destruct_ent(int onumber)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    ASSERT(ents_[onumber]);
    ents_[onumber] = nil;
}

object find_ent(int onumber)
{
    object obj;

    ASSERT_ACCESS(previous_program() == PROXY
                  || previous_program() == SYSTEM_AUTO);
    obj = ents_[onumber];
    if (obj && api_path::number(object_name(obj)) != -1) {
        obj = obj->_F_find(onumber);
    }
    return obj;
}

void move_ent(int onumber, object obj)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber && ents_[onumber]);
    DEBUG_ASSERT(obj);
    ents_[onumber] = obj;
}
