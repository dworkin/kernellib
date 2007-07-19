# include <type.h>
# include <kernel/kernel.h>
# include <kernel/user.h>

inherit LIB_WIZTOOL;

private object user;		/* associated user object */

/*
 * NAME:	create()
 * DESCRIPTION:	initialize object
 */
static void create(int clone)
{
    if (clone) {
	::create(200);
	user = this_user();
    }
}

/*
 * NAME:	message()
 * DESCRIPTION:	pass on a message to the user
 */
static void message(string str)
{
    user->message(str);
}

/*
 * NAME:	input()
 * DESCRIPTION:	deal with input from user
 */
void input(string str)
{
    if (previous_object() == user) {
	call_limited("process", str);
    }
}

/*
 * NAME:	process()
 * DESCRIPTION:	process user input
 */
static void process(string str)
{
    string arg;

    if (query_editor(this_object())) {
	if (strlen(str) != 0 && str[0] == '!') {
	    str = str[1 ..];
	} else {
	    str = editor(str);
	    if (str) {
		message(str);
	    }
	    return;
	}
    }

    if (str == "") {
	return;
    }

    sscanf(str, "%s %s", str, arg);
    if (arg == "") {
	arg = nil;
    }

    switch (str) {
    case "code":
    case "history":
    case "clear":
    case "compile":
    case "clone":
    case "destruct":
    case "find":

    case "cd":
    case "pwd":
    case "ls":
    case "cp":
    case "mv":
    case "rm":
    case "mkdir":
    case "rmdir":
    case "ed":

    case "access":
    case "grant":
    case "ungrant":
    case "quota":
    case "rsrc":

    case "people":
    case "status":
    case "swapout":
    case "statedump":
    case "shutdown":
    case "reboot":
	call_other(this_object(), "cmd_" + str, user, str, arg);
	break;

    default:
	message("No command: " + str + "\n");
	break;
    }
}

static void cmd_find(object user, string cmd, string str)
{
    int i;
    mixed obj;

    i = -1;
    if (!str || (sscanf(str, "$%d%s", i, str) != 0 && (i < 0 || str != "")))
    {
        message("Usage: " + cmd + " [<obj> | $<ident>]\n");
        return;
    }

    if (i >= 0) {
        str = catch(obj = fetch(i));
        if (str) {
            message("Error: " + str + "\n");
            return;
        }
        if (typeof(obj) != T_OBJECT) {
            message("Not an object.\n");
            return;
        }
    } else if (sscanf(str, "$%s", str) != 0) {
        obj = ident(str);
        if (!obj) {
            message("Unknown $ident.\n");
            return;
        }
    } else {
        str = DRIVER->normalize_path(str, query_directory(), query_owner());
        obj = find_object(str);
        if (!obj) {
            message("No such object.\n");
            return;
        }
    }
    store(obj);
}
