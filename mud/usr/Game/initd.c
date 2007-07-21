# include <game/action.h>
# include <game/command.h>
# include <game/selector.h>
# include <game/value.h>
# include <game/word.h>
# include <system/assert.h>

inherit UTIL_VALUE;

object wordd_;
object commandd_;

static void test_command(string command)
{
    message(dump_value(commandd_->parse(command)));
}

static void create()
{
    compile_object(LOOK_ACTION);
    compile_object(LOOK_AT_ACTION);
    compile_object(GO_ACTION);
    compile_object(PICK_UP_ACTION);
    compile_object(SAY_ACTION);
    compile_object(SAY_TO_ACTION);
    compile_object(GIVE_ACTION);

    compile_object(SELECTOR);
    compile_object(ORDINAL_SELECTOR);
    compile_object(COUNT_SELECTOR);
    compile_object(ALL_OF_SELECTOR);
    compile_object(ALL_SELECTOR);
    compile_object(EXCEPT_SELECTOR);

    wordd_ = compile_object(WORDD);
    commandd_ = compile_object(COMMANDD);

    test_command("go west");

    test_command("look");
    test_command("look at rusty cage");

    test_command("say 'Hello, World!");
    test_command("say 'Hello, World!' to elf");
    test_command("say 'Hello, World!' to elves except orcs");
}
