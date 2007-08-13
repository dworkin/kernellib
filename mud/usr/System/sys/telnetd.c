# include <status.h>
# include <kernel/user.h>
# include <system/user.h>

object userd_;

/*
 * NAME:        create()
 * DESCRIPTION: initialize object
 */
static void create()
{
    userd_ = find_object(USERD);
}

/*
 * NAME:        select()
 * DESCRIPTION: return a user object based on the first line of input
 */
object select(string str)
{
    if (previous_object() == userd_) {
        return clone_object(SYSTEM_USER);
    }
}

/*
 * NAME:        query_timeout()
 * DESCRIPTION: return a timeout for the given connection, or -1 to close it
 *              immediately
 */
int query_timeout(object connection)
{
    if (previous_object() == userd_) {
        return DEFAULT_TIMEOUT;
    }
}

/*
 * NAME:        query_banner()
 * DESCRIPTION: return a login banner for the given connection, or nil for no
 *              banner
 */
string query_banner(object connection)
{
    if (previous_object() == userd_) {
        return "\nWelcome to Leprechaun.\n\n"
            + "Log in with one of:\n\n"
            + "  <name>\n"
            + "  <name> the [<gender>] [<race>] [<guild>]\n\n"
            + "Gender:  Female or male.\n"
            + "Race:    Dwarf, elf, goblin, human, or leprechaun.\n"
            + "Guild:   Bard, knight, monk, priest, ranger, thief, warrior, "
            + "or wizard.\n\n"
            + "Unspecified options will be randomly selected.\n\n"
            + "login: ";
    }
}
