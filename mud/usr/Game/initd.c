# include <system/assert.h>

static void create()
{
    string  oname;
    object  temple, elf, sword;

    temple = compile_object("~/room/temple");
    compile_object("~/obj/elf");
    compile_object("~/data/id/sword");

    elf = clone_object("~/obj/elf", 10);
    sword = new_object("~/data/id/sword", 10);
    ASSERT(sword->query_class() == 10);
    sword = new_object("~/data/id/sword", 11);
    sword = new_object("~/data/id/sword", 12);
    ASSERT(find_object(sword));
    oname = object_name(sword);
    ASSERT(find_object(oname));
    message("created <" + oname + ">");

    move_object(sword, temple);
    ASSERT(environment(sword) == temple);
    ASSERT(sizeof(inventory(temple)) == 1);
    message("moved <" + oname + ">");

    move_object(sword, nil);
    ASSERT(environment(sword) == nil);
    ASSERT(!sizeof(inventory(temple)));
    message("moved <" + oname + "> again");

    destruct_object(sword);
    ASSERT(!sizeof(inventory(temple)));
    ASSERT(!find_object(sword));
    message("destructed <" + oname + ">");
}
