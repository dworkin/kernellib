# include <system/assert.h>

private void log(string message)
{
    message(object_name(this_object()) + ": " + message + "\n");
}

static void create()
{
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
    log("created <" + oname + ">");

    move_object(sword, temple);
    ASSERT(environment(sword) == temple);
    ASSERT(sizeof(inventory(temple)) == 1);
    log("moved <" + oname + ">");

    move_object(sword, nil);
    ASSERT(environment(sword) == nil);
    ASSERT(!sizeof(inventory(temple)));
    log("moved <" + oname + "> again");

    destruct_object(sword);
    ASSERT(!sizeof(inventory(temple)));
    ASSERT(!find_object(sword));
    log("destructed <" + oname + ">");
}
