# include <kernel/kernel.h>
# include <system/assert.h>
# include <system/object.h>
# include <system/system.h>

private int     oid_;
private object  env_;  /* cached environment */

void init(int oid)
{
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(oid);
    oid_ = oid;
}

object find()
{
    object obj;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    if (!oid_) {
	return nil;
    }

    if (env_) {
        /* search cached environment */
        if (obj = env_->_F_find(oid_)) {
            return obj;
        }

        /* clear cached environment */
        env_ = nil;
    }

    if (obj = OBJECTD->find_dlwo(oid_)) {
        /* update cached environment */
        env_ = obj->_Q_env();
        return obj;
    }

    /* clear proxy */
    oid_ = 0;
    return nil;
}
