# include <game/thing.h>

inherit LIB_THING;

object user_;

void set_user(object user)
{
    user_ = user;
}

void observe(string mess)
{
    if (user_) {
        user_->observe(mess);
    }
}
