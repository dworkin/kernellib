# include <game/action.h>
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

void add_action(object LIB_ACTION action)
{
    ASSERT_ARG(action);
    call_out("act", 0, action);
}

static void act(object LIB_ACTION action)
{
    string error;

    error = catch(action->perform(this_object()));
    if (error) {
        event("error", error);
    }
}
