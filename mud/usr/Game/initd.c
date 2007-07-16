# include <system/assert.h>
# include <game/action.h>
# include <game/command.h>

object commandd_;

static void test_command(string command)
{
    ASSERT(commandd_->parse(command));
}

static void create()
{
    compile_object(GO_DIRECTION_ACTION);
    compile_object(LOOK_ACTION);
    compile_object(LOOK_AT_THING_ACTION);

    commandd_ = compile_object(COMMANDD);

    test_command("go west");

    test_command("look");
    test_command("look at rusty cage");
}
