#include <system/assert.h>
#include <system/object.h>

int     *free_, free_size_, total_size_;
object  *ents_;

static void create(int clone) {
    if (clone) {
        free_ = allocate_int(ENTOBJ_SIZE);
        ents_ = allocate(ENTOBJ_SIZE);
    }
}

int new(object ent) {
    int minor;

    ASSERT_ACCESS(previous_program() == OBJECTD);
    DEBUG_ASSERT(ent);
    if (free_size_) {
        minor = free_[--free_size_];
    } else if (total_size_ < ENTOBJ_SIZE) {
        minor = total_size_++;
    } else {
        return -1;
    }
    ents_[minor] = ent;
    return minor;
}

int destruct(int minor) {
    ASSERT_ACCESS(previous_program() == OBJECTD);
    if (minor >= 0 && minor < ENTOBJ_SIZE && ents_[minor]) {
        ents_[minor] = nil;
        DEBUG_ASSERT(free_size_ < ENTOBJ_SIZE);
        free_[free_size_++] = minor;
        return TRUE;
    }
    return FALSE;
}

object find(int minor) {
    ASSERT_ACCESS(previous_program() == OBJECTD);
    return minor >= 0 && minor < ENTOBJ_SIZE ? ents_[minor] : nil;
}
