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
