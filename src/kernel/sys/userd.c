# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/user.h>
# include <status.h>


object *users;				/* user mappings */
mapping names;				/* name : connection object */
object *connections;			/* saved connections */
mapping telnet, binary, datagram;	/* port managers */
mapping editors;			/* editor : TRUE */

/*
 * NAME:	create()
 * DESCRIPTION:	initialize object
 */
static void create()
{
    /* load essential objects */
    compile_object(TELNET_CONN);
    compile_object(BINARY_CONN);
    compile_object(DATAGRAM_CONN);
    compile_object(DEFAULT_USER);

    /* initialize user arrays */
    users = ({ });
    names = ([ ]);
    telnet = ([ ]);
    binary = ([ ]);
    datagram = ([ ]);
    editors = ([ ]);
}

/*
 * NAME:	telnet_connection()
 * DESCRIPTION:	return a new telnet connection object
 */
object telnet_connection(mapping tls, int port)
{
    if (previous_program() == DRIVER) {
	object conn;

	conn = clone_object(TELNET_CONN);
	conn->set_port(port);
	return conn;
    }
}

/*
 * NAME:	binary_connection()
 * DESCRIPTION:	return a new binary connection object
 */
object binary_connection(mapping tls, int port)
{
    if (previous_program() == DRIVER) {
	object conn;

	conn = clone_object(BINARY_CONN);
	conn->set_port(port);
	return conn;
    }
}

/*
 * NAME:	datagram_connection()
 * DESCRIPTION:	return a new datagram connection object
 */
object datagram_connection(mapping tls, int port)
{
    if (previous_program() == DRIVER) {
	object conn;

	conn = clone_object(DATAGRAM_CONN);
	conn->set_port(port);
	return conn;
    }
}

/*
 * NAME:	set_telnet_manager()
 * DESCRIPTION:	set the telnet manager object, which determines what the
 *		user object is, based on the first line of input
 */
void set_telnet_manager(int port, object manager)
{
    if (SYSTEM() && !telnet[port]) {
	telnet[port] = manager;
    }
}

/*
 * NAME:	set_binary_manager()
 * DESCRIPTION:	set the binary manager object, which determines what the
 *		user object is, based on the first line of input
 */
void set_binary_manager(int port, object manager)
{
    if (SYSTEM() && !binary[port]) {
	binary[port] = manager;
    }
}

/*
 * NAME:	set_datagram_manager()
 * DESCRIPTION:	set the datagram manager object, which determines what the
 *		user object is, based on the first datagram
 */
void set_datagram_manager(int port, object manager)
{
    if (SYSTEM() && !datagram[port]) {
	datagram[port] = manager;
    }
}


/*
 * NAME:	telnet_user()
 * DESCRIPTION:	select user object for telnet connection, based on line of
 *		input
 */
object telnet_user(int port, string str)
{
    if (previous_program() == LIB_CONN) {
	object user;

	user = names[str];
	if (!user) {
	    user = telnet[port];
	    if (user && (str != "admin" || port != 0)) {
		user = (object LIB_USER) user->select(str);
	    } else {
		user = clone_object(DEFAULT_USER);
	    }
	}
	return user;
    }
}

/*
 * NAME:	binary_user()
 * DESCRIPTION:	select user object for binary connection, based on line of
 *		input
 */
object binary_user(int port, string str)
{
    if (previous_program() == LIB_CONN) {
	object user;

	user = names[str];
	if (!user) {
	    user = binary[port];
	    if (user) {
		user = (object LIB_USER) user->select(str);
	    } else {
		user = clone_object(DEFAULT_USER);
	    }
	}
	return user;
    }
}

/*
 * NAME:	datagram_user()
 * DESCRIPTION:	select user object for datagram connection, based on the first
 *		datagram
 */
object datagram_user(int port, string str)
{
    if (previous_program() == LIB_CONN) {
	object user;

	user = names[str];
	if (!user) {
	    user = datagram[port];
	    if (user) {
		user = (object LIB_USER) user->select(str);
	    } else {
		user = clone_object(DEFAULT_USER);
	    }
	}
	return user;
    }
}

/*
 * NAME:	query_telnet_timeout()
 * DESCRIPTION:	return the current telnet connection timeout
 */
int query_telnet_timeout(int port, object obj)
{
    if (previous_program() == LIB_CONN) {
	object manager;

	manager = telnet[port];
	return (manager) ? manager->query_timeout(obj) : DEFAULT_TIMEOUT;
    }
}

/*
 * NAME:	query_binary_timeout()
 * DESCRIPTION:	return the current binary connection timeout
 */
int query_binary_timeout(int port, object obj)
{
    if (previous_program() == LIB_CONN) {
	object manager;

	manager = binary[port];
	return (manager) ? manager->query_timeout(obj) : DEFAULT_TIMEOUT;
    }
}

/*
 * NAME:	query_datagram_timeout()
 * DESCRIPTION:	return the current datagram connection timeout
 */
int query_datagram_timeout(int port, object obj)
{
    if (previous_program() == LIB_CONN) {
	object manager;

	manager = datagram[port];
	return (manager) ? manager->query_timeout(obj) : DEFAULT_TIMEOUT;
    }
}

/*
 * NAME:	query_telnet_banner()
 * DESCRIPTION:	return the current telnet login banner
 */
string query_telnet_banner(int port, object obj)
{
    if (previous_program() == LIB_CONN) {
	object manager;

	manager = telnet[port];
	return (manager) ?
		manager->query_banner(obj) :
		"\n" + status(ST_VERSION) + " (telnet)\n\nlogin: ";
    }
}

/*
 * NAME:	query_binary_banner()
 * DESCRIPTION:	return the current binary login banner
 */
string query_binary_banner(int port, object obj)
{
    if (previous_program() == LIB_CONN) {
	object manager;

	manager = binary[port];
	return (manager) ?
		manager->query_banner(obj) :
		(port >= 0) ?
		 "\r\n" + status(ST_VERSION) + " (binary)\r\n\r\nlogin: " :
		 nil;
    }
}


/*
 * NAME:	login()
 * DESCRIPTION:	login user
 */
void login(object user, string name)
{
    if (previous_program() == LIB_USER) {
	users = (users - ({ nil })) | ({ user });
	names[name] = user;
    }
}

/*
 * NAME:	logout()
 * DESCRIPTION:	log user out
 */
void logout(object user, string name)
{
    if (previous_program() == LIB_USER) {
	users -= ({ user });
	names[name] = nil;
    }
}


/*
 * NAME:	query_users()
 * DESCRIPTION:	return the current telnet and binary users
 */
object *query_users()
{
    if (previous_program() == AUTO) {
	object *usr;
	int i, changed;

	usr = users - ({ nil });
	changed = FALSE;
	for (i = sizeof(usr); --i >= 0; ) {
	    if (!usr[i]->query_conn()) {
		usr[i] = nil;
		changed = TRUE;
	    }
	}
	return (changed) ? usr - ({ nil }) : usr;
    }
}

/*
 * NAME:	query_connections()
 * DESCRIPTION:	return the current connections
 */
object *query_connections()
{
    if (previous_program() == API_USER) {
	return users();
    }
}

/*
 * NAME:	find_user()
 * DESCRIPTION:	find the user associated with a certain name
 */
object find_user(string name)
{
    if (previous_program() == API_USER) {
	return names[name];
    }
}

/*
 * NAME:	add_editor()
 * DESCRIPTION:	register an editor object
 */
void add_editor(object obj)
{
    if (previous_program() == AUTO) {
	editors[obj] = TRUE;
    }
}

/*
 * NAME:	remove_editor()
 * DESCRIPTION:	unregister an editor object
 */
void remove_editor(object obj)
{
    if (previous_program() == AUTO) {
	editors[obj] = nil;
    }
}

/*
 * NAME:	query_editors()
 * DESCRIPTION:	return the current editor objects
 */
object *query_editors()
{
    if (SYSTEM()) {
	return map_indices(editors);
    }
}


/*
 * NAME:	prepare_reboot()
 * DESCRIPTION:	prepare for a reboot
 */
void prepare_reboot()
{
    if (previous_program() == DRIVER) {
	editors = ([ ]);
	catch {
	    connections = users();
	}
    }
}

/*
 * NAME:	reboot()
 * DESCRIPTION:	handle a reboot
 */
void reboot()
{
    if (previous_program() == DRIVER) {
	int i;

	if (connections) {
	    for (i = sizeof(connections); --i >= 0; ) {
		connections[i]->reboot();
	    }
	    connections = nil;
	}

	users = ({ });
	names = ([ ]);
    }
}
