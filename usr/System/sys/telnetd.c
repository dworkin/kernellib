# include <status.h>
# include <kernel/user.h>
# include <System.h>

private object user;

static void
create()
{
    user = find_object(SYS_USER);
    USERD->set_telnet_manager(this_object());
}

object
select(string str)
{
    if (previous_program() == USERD) {
        return clone_object(SYS_USER);
    }
}

string
query_banner(object user)
{
    if (previous_program() == USERD) {
        return "\nDemo DGD " + status()[ST_VERSION] + " (telnet)\n\nlogin: ";
    }
}

int
query_timeout(object user)
{
    if (previous_program() == USERD) {
        return 60;
    }
}
