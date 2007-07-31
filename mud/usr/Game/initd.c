# include <limits.h>
# include <game/action.h>
# include <game/command.h>
# include <game/selector.h>
# include <game/word.h>
# include <game/thing.h>
# include <system/assert.h>

# define BAG     "/usr/Game/obj/bag"
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
    compile_object(PUT_IN_ACTION);
    compile_object(TAKE_OUT_ACTION);
    compile_object(WEAR_ACTION);

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
    compile_object(LOOK_TO_COMMAND);
    compile_object(PICK_UP_COMMAND);
    compile_object(PUT_DOWN_COMMAND);
    compile_object(PUT_IN_COMMAND);
    compile_object(SAY_COMMAND);
    compile_object(SAY_TO_COMMAND);
    compile_object(TAKE_OUT_COMMAND);
    compile_object(WEAR_COMMAND);

    wordd_ = compile_object(WORDD);
    commandd_ = compile_object(COMMANDD);

    temple_ = compile_object(TEMPLE);
    crypt = compile_object(CRYPT);
    compile_object(ELF);
    compile_object(ORC);
    compile_object(CORPSE);
    compile_object(BAG);
    compile_object(SWORD);
    compile_object(SHIELD);
    compile_object(COIN);

    move_object(clone_object(BAG), temple_);
    move_object(clone_object(ORC), crypt);
    move_object(new_object(SWORD), crypt);
    move_object(new_object(SHIELD), crypt);
    move_object(new_object(COIN, "silver", 1 + random(13)), crypt);
}

object LIB_CREATURE make_creature()
{
    object creature;

    creature = clone_object(ELF);
    
    move_object(creature, temple_);
    return creature;
}
