# include <system/assert.h>

private int object_number(object obj)
{
    int onumber;

    DEBUG_ASSERT(obj);
    sscanf(object_name(obj), "%*s#%d", onumber);
    return onumber;
}

static void create()
{
    object temple, elf, sword;

    temple = compile_object("~/room/temple");
    ASSERT(temple);
    elf = compile_object("~/obj/elf");
    ASSERT(!elf); /* hidden master object */
    sword = compile_object("~/data/sword");
    ASSERT(!sword); /* hidden master object */

    elf = clone_object("~/obj/elf", 9);
    ASSERT(elf);
    ASSERT(elf->query_level() == 9);

    sword = new_object("~/data/sword", 10);
    ASSERT(sword);
    ASSERT(sword->query_class() == 10);
    ASSERT(object_number(sword) == -1);
    ASSERT(!find_object(object_name(sword)));

    move_object(sword, elf);
    ASSERT(environment(sword) == elf);
    ASSERT(sizeof(inventory(elf)) == 1);
    ASSERT(object_number(sword) < -1);
    ASSERT(find_object(object_name(sword)) == sword);
    ASSERT(find_object(object_number(sword)) == sword);

    move_object(sword, temple);
    ASSERT(environment(sword) == temple);
    ASSERT(sizeof(inventory(temple)) == 1);
    ASSERT(!sizeof(inventory(elf)));

    call_out("copy", 0, sword);
    sword->start();
}

static void copy(object sword)
{
    ASSERT(object_number(sword) == -1);
}
