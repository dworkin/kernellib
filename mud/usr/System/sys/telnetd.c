# include <status.h>
# include <kernel/user.h>
# include <system/assert.h>
# include <system/user.h>

object userd;

static void create()
{
    userd = find_object(USERD);
}

object select(string str)
{
    ASSERT_ACCESS(previous_object() == userd);
    return clone_object(SYSTEM_USER);
}

int query_timeout(object connection)
{
    ASSERT_ACCESS(previous_object() == userd);
    return DEFAULT_TIMEOUT;
}

string query_banner(object connection)
{
    ASSERT_ACCESS(previous_object() == userd);
    return "\nWelcome to Leprechaun.\n\nlogin: ";
}
