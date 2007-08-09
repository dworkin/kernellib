# include <type.h>
# include <kernel/kernel.h>
# include <kernel/user.h>
# include <system/object.h>

inherit tool  LIB_WIZTOOL;
inherit obj   API_OBJECT;

object user; /* associated user object */

/*
 * NAME:	normalize_path()
 * DESCRIPTION:	normalize a path relative to the current directory and owner
 */
private string normalize_path(string path)
{
    return find_object(DRIVER)->normalize_path(path, query_directory(),
                                               query_owner());
}

/*
 * NAME:        format_oids()
 * DESCRIPTION: return a comma-separated list of object numbers
 */
private string format_oids(int *oids)
{
    string  str;
    int     i, size;

    str = (string) oids[0];
    size = sizeof(oids);
    for (i = 1; i < size; ++i) {
        str += ", " + oids[i];
    }
    return str;
}

/*
 * NAME:	create()
 * DESCRIPTION:	initialize object
 */
static void create(int clone)
{
    if (clone) {
	tool::create(200);
        obj::create();
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
    case "pls":

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

/*
 * NAME:	cmd_find()
 * DESCRIPTION:	find an object by name or $ident
 */
static void cmd_find(object user, string cmd, string str)
{
    int    i;
    mixed  obj;

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
        str = normalize_path(str);
        obj = find_object(str);
        if (!obj) {
            message("No such object.\n");
            return;
        }
    }
    store(obj);
}

/*
 * NAME:	cmd_pls()
 * DESCRIPTION:	list programs
 */
static void cmd_pls(object user, string cmd, string str)
{
    mapping    dir;
    string    *names, message;
    int      **oids, i, size;

    if (!str) {
        str = ".";
    }
    str = normalize_path(str);
    dir = get_program_dir(str);
    names = map_indices(dir);
    oids = map_values(dir);

    size = sizeof(names);
    if (!size) {
        message("No programs.\n");
        return;
    }

    message = "";
    for (i = 0; i < size; ++i) {
        string line;

        if (oids[i][0] == -2) {
            names[i] += "/";
            oids[i] = oids[i][1 ..];
        } else {
            names[i] += " ";
        }
        
        line = names[i];
        if (strlen(line) < 18) {
            line = "                  "[.. 18 - strlen(line) - 1] + line;
        }
        if (sizeof(oids[i])) {
            line += "  " + format_oids(oids[i]);
        }
        message += line + "\n";
    }
    message(message);
}
