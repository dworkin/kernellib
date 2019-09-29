# include <kernel/kernel.h>
# include <kernel/user.h>

inherit LIB_CONN;	/* basic connection object */


object driver;		/* driver object */
string buffer;		/* buffered input */
int length;		/* length of message to receive */
int raw;		/* pending raw input? */

/*
 * NAME:	create()
 * DESCRIPTION:	initialize
 */
static void create()
{
    ::create("binary");
    driver = find_object(DRIVER);
    buffer = "";
}

/*
 * NAME:	connect()
 * DESCRIPTION:	initiate an outbound connection
 */
void connect(object user, string address, int port)
{
    if (previous_program() == LIB_USER) {
	::connect(address, port);
	set_user(user, nil);
    }
}

/*
 * NAME:	open()
 * DESCRIPTION:	open the connection
 */
static void open()
{
    ::open(([ ]));
}

/*
 * NAME:	unconnected()
 * DESCRIPTION:	an outbound connection could not be established
 */
static void unconnected(int refused)
{
    ::unconnected(([ ]), refused);
}

/*
 * NAME:	close()
 * DESCRIPTION:	close the connection
 */
static void close(int dest)
{
    ::close(([ ]), dest);
}

/*
 * NAME:	timeout()
 * DESCRIPTION:	connection timed out
 */
static void timeout()
{
    ::timeout(([ ]));
}

/*
 * NAME:	set_message_length()
 * DESCRIPTION:	set the size of the receive buffer
 */
void set_message_length(int len)
{
    if (previous_program() == LIB_USER) {
	length = len;
    }
}

/*
 * NAME:	add_to_buffer()
 * DESCRIPTION:	do this where an error is allowed to happen
 */
private void add_to_buffer(mapping tls, string str)
{
    catch {
	buffer += str;
    } : error("Binary connection buffer overflow");
}

/*
 * NAME:	receive_message()
 * DESCRIPTION:	forward a message to listeners
 */
static void receive_message(string str)
{
    int mode, len;
    string head, pre;
    mapping tls;

    add_to_buffer(tls = ([ ]), str);

    while (this_object() &&
	   (mode=query_mode()) != MODE_BLOCK && mode != MODE_DISCONNECT) {
	if (mode != MODE_RAW) {
	    if (sscanf(buffer, "%s\r\n%s", str, buffer) != 0 ||
		sscanf(buffer, "%s\n%s", str, buffer) != 0) {
		while (sscanf(str, "%s\b%s", head, str) != 0) {
		    while (sscanf(head, "%s\x7f%s", pre, head) != 0) {
			len = strlen(pre);
			if (len != 0) {
			    head = pre[0 .. len - 2] + head;
			}
		    }
		    len = strlen(head);
		    if (len != 0) {
			str = head[0 .. len - 2] + str;
		    }
		}
		while (sscanf(str, "%s\x7f%s", head, str) != 0) {
		    len = strlen(head);
		    if (len != 0) {
			str = head[0 .. len - 2] + str;
		    }
		}

		::receive_message(tls, str);
	    } else {
		break;
	    }
	} else {
	    if (strlen(buffer) != 0) {
		if (length > 0) {
		    if (length < strlen(buffer)) {
			str = buffer[.. length - 1];
			buffer = buffer[length ..];
			length = 0;
		    } else {
			length -= strlen(buffer);
			str = buffer;
			buffer = "";
		    }
		} else {
		    str = buffer;
		    buffer = "";
		}
		::receive_message(tls, str);
	    }
	    break;
	}
    }
}

/*
 * NAME:	set_mode()
 * DESCRIPTION:	set the connection mode
 */
void set_mode(int mode)
{
    if (previous_program() == LIB_CONN || SYSTEM()) {
	::set_mode(mode);
	if (!raw && mode == MODE_RAW && strlen(buffer) != 0) {
	    call_out("raw_message", 0);
	    raw = TRUE;
	}
    }
}

/*
 * NAME:	raw_message()
 * DESCRIPTION:	process the whole input buffer after switching to raw mode
 */
static void raw_message()
{
    string str;
    mapping tls;

    raw = FALSE;
    if (query_mode() == MODE_RAW && strlen(buffer) != 0) {
	if (length > 0) {
	    if (length < strlen(buffer)) {
		str = buffer[.. length - 1];
		buffer = buffer[length ..];
		length = 0;
	    } else {
		length -= strlen(buffer);
		str = buffer;
		buffer = "";
	    }
	} else {
	    str = buffer;
	    buffer = "";
	}
	tls = ([ ]);
	TLSVAR(tls, TLS_USER) = this_object();
	::receive_message(tls, str);
    }
}

/*
 * NAME:	message()
 * DESCRIPTION:	send a message to the other side
 */
int message(string str)
{
    if (query_mode() < MODE_RAW) {
	str = implode(explode("\n" + str + "\n", "\n"), "\r\n");
    }
    return ::message(str);
}

/*
 * NAME:	message_done()
 * DESCRIPTION:	called when output is completed
 */
static void message_done()
{
    ::message_done(([ ]));
}

/*
 * NAME:	datagram_attach()
 * DESCRIPTION:	attach a datagram channel to this connection
 */
static void datagram_attach()
{
    ::datagram_attach(([ ]));
}

/*
 * NAME:	receive_datagram()
 * DESCRIPTION:	receive a datagram
 */
static void receive_datagram(string str)
{
    ::receive_datagram(([ ]), str);
}
