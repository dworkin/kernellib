# include <game/action.h>
# include <game/command.h>
# include <game/value.h>
# include <game/word.h>
# include <system/assert.h>

inherit UTIL_VALUE;

object wordd_;
object commandd_;

static void test_command(string command)
{
    message(format_value(commandd_->parse(command)));
}

static void create()
{
    compile_object(GO_DIRECTION_ACTION);
    compile_object(LOOK_ACTION);
    compile_object(LOOK_AT_SELECTION_ACTION);
    compile_object(SAY_QUOTE_ACTION);
    compile_object(SAY_QUOTE_TO_SELECTION_ACTION);

    wordd_ = compile_object(WORDD);
    commandd_ = compile_object(COMMANDD);

    test_command("go west");

    test_command("look");
    test_command("look at rusty, rusty cage cage");

    test_command("say 'Hello, World!");
    test_command("say 'Hello, World!' to elf");
}
