# include <kernel/kernel.h>
# include <System.h>

private void
load(string filename)
{
    if (!find_object(filename)) {
        compile_object(filename);
    }
}

static void
create()
{
    load(SYS_LIB_WIZTOOL);
    load(SYS_WIZTOOL);
    load(SYS_LIB_USER);
    load(SYS_USER);
    load(SYS_TELNETD);
}

void
prepare_reboot()
{
    if (previous_program() == DRIVER) {
        /* ... */
    }
}

void
reboot()
{
    if (previous_program() == DRIVER) {
        /* ... */
    }
}
