#include <kernel/kernel.h>
#include <system/assert.h>
#include <system/object.h>
#include <system/system.h>

private int     onumber_;
private object  env_; /* cached environment */

void init(int onumber) {
    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    DEBUG_ASSERT(onumber);
    onumber_ = onumber;
}

object find() {
    object obj;

    ASSERT_ACCESS(previous_program() == SYSTEM_AUTO);
    if (!onumber_) return nil;

    if (env_) {
        /* search cached environment */
        if (obj = env_->_F_find(onumber_)) {
            return obj;
        }

        /* clear cached environment */
        env_ = nil;
    }

    if (obj = OBJECTD->find_ent(onumber_)) {
        /* update cached environment */
        env_ = obj->_Q_env();
        return obj;
    }

    /* clear proxy */
    onumber_ = 0;
    return nil;
}
