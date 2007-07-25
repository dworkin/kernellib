# include <game/command.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_THING;

static void create()
{
    ::create();
    add_event("observe");
    add_event("error");
}

int allow_subscribe(object obj, string name)
{
    return TRUE;
}

void observe(string mess)
{
    event("observe", mess);
}

void add_command(object LIB_COMMAND command)
{
    ASSERT_ARG(command);
    call_out("act", 0, command);
}

static void act(object LIB_COMMAND command)
{
    string error;

    error = catch(command->perform(this_object()));
    if (error) {
        event("error", error);
    }
}
