# include <kernel/user.h>
# include <system/assert.h>
# include <system/user.h>

object select(string str)
{
    ASSERT_ACCESS(previous_program() == USERD);
    return clone_object(SYSTEM_USER);
}

int query_timeout(object connection)
{
    ASSERT_ACCESS(previous_program() == USERD);
    return DEFAULT_TIMEOUT;
}

string query_banner(object connection)
{
    ASSERT_ACCESS(previous_program() == USERD);
    return nil;
}
