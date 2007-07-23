# include <game/action.h>
# include <game/command.h>
# include <game/selector.h>
# include <game/value.h>
# include <game/word.h>
# include <game/thing.h>
# include <system/assert.h>

inherit UTIL_VALUE;

# define CRYPT   "/usr/Game/room/crypt"
# define ELF     "/usr/Game/obj/elf"
# define SHIELD  "/usr/Game/data/shield"
# define SWORD   "/usr/Game/data/sword"
# define TEMPLE  "/usr/Game/room/temple"

object wordd_;
object commandd_;
object temple_;

static void test_command(string command)
{
    message(dump_value(commandd_->parse(command)));
}

static void create()
{
    object LIB_ROOM crypt;

    compile_object(INVENTORY_ACTION);
    compile_object(LOOK_ACTION);
    compile_object(LOOK_AT_ACTION);
    compile_object(GO_ACTION);
    compile_object(PICK_UP_ACTION);
    compile_object(PUT_DOWN_ACTION);
    compile_object(SAY_ACTION);
    compile_object(SAY_TO_ACTION);
    compile_object(GIVE_ACTION);

    compile_object(SELECTOR);
    compile_object(ORDINAL_SELECTOR);
    compile_object(COUNT_SELECTOR);
    compile_object(ALL_OF_SELECTOR);
    compile_object(ALL_SELECTOR);
    compile_object(LIST_SELECTOR);
    compile_object(EXCEPT_SELECTOR);

    wordd_ = compile_object(WORDD);
    commandd_ = compile_object(COMMANDD);

    temple_ = compile_object(TEMPLE);
    crypt = compile_object(CRYPT);
    compile_object(SWORD);
    compile_object(SHIELD);
    compile_object(ELF);

    move_object(new_object(SWORD), crypt);
    move_object(new_object(SHIELD), crypt);

    test_command("go west");

    test_command("look");
    test_command("look at rusty cage");

    test_command("say 'Hello, World!");
    test_command("say 'Hello, World!' to elf");
    test_command("say 'Hello, World!' to elves except orcs");
}

object LIB_CREATURE make_creature()
{
    object creature;

    creature = clone_object(ELF);
    
    move_object(creature, temple_);
    return creature;
}

string dump(mixed val)
{
    return dump_value(val);
}
