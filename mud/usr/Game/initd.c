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
# define ORC     "/usr/Game/obj/orc"
# define SHIELD  "/usr/Game/data/shield"
# define SWORD   "/usr/Game/data/sword"
# define TEMPLE  "/usr/Game/room/temple"

object wordd_;
object commandd_;
object temple_;

static void create()
{
    object LIB_ROOM crypt;

    compile_object(GIVE_ACTION);
    compile_object(PICK_UP_ACTION);
    compile_object(PUT_DOWN_ACTION);

    compile_object(SIMPLE_SELECTOR);
    compile_object(ORDINAL_SELECTOR);
    compile_object(COUNT_SELECTOR);
    compile_object(ALL_OF_SELECTOR);
    compile_object(ALL_SELECTOR);
    compile_object(LIST_SELECTOR);
    compile_object(EXCEPT_SELECTOR);

    compile_object(GIVE_COMMAND);
    compile_object(GO_COMMAND);
    compile_object(INVENTORY_COMMAND);
    compile_object(LOOK_COMMAND);
    compile_object(LOOK_AT_COMMAND);
    compile_object(PICK_UP_COMMAND);
    compile_object(PUT_DOWN_COMMAND);
    compile_object(SAY_COMMAND);
    compile_object(SAY_TO_COMMAND);

    wordd_ = compile_object(WORDD);
    commandd_ = compile_object(COMMANDD);

    temple_ = compile_object(TEMPLE);
    crypt = compile_object(CRYPT);
    compile_object(SWORD);
    compile_object(SHIELD);
    compile_object(ELF);
    compile_object(ORC);

    move_object(clone_object(ORC), crypt);
    move_object(new_object(SWORD), crypt);
    move_object(new_object(SHIELD), crypt);
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
