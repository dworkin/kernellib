# include <system/assert.h>

static void create()
{
    string  oname;
    object  sword, temple;

    compile_object("~/data/ent/sword");
    temple = compile_object("~/room/temple");

    sword = new_object("~/data/ent/sword", 10);
    sword = new_object("~/data/ent/sword", 11);
    sword = new_object("~/data/ent/sword", 12);
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
