#include <system/assert.h>

inherit "/usr/System/open/lib/auto";

static void create() {
    string  oname;
    object  sword, temple;

    compile_object("~/data/ent/sword");
    temple = compile_object("~/room/temple");

    sword = new_object("~/data/ent/sword");
    sword = new_object("~/data/ent/sword");
    sword = new_object("~/data/ent/sword");
    ASSERT(find_object(sword));
    oname = object_name(sword);
    ASSERT(find_object(oname));
    message("~Game/initd: created <" + oname + ">\n");

    move_object(sword, temple);
    ASSERT(environment(sword) == temple);
    ASSERT(sizeof(inventory(temple)) == 1);
    message("~Game/initd: moved <" + oname + ">\n");

    move_object(sword, nil);
    ASSERT(environment(sword) == nil);
    ASSERT(!sizeof(inventory(temple)));
    message("~Game/initd: moved <" + oname + "> again\n");

    destruct_object(sword);
    ASSERT(!sizeof(inventory(temple)));
    ASSERT(!find_object(sword));
    message("~Game/initd: destructed <" + oname + ">\n");
}
