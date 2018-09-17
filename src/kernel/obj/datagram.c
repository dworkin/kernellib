# include <kernel/kernel.h>
# include <kernel/user.h>

inherit LIB_CONN;	/* basic connection object */

# define TLS(tls, n)	tls[-1 - n]

object driver;		/* driver object */

/*
 * NAME:	create()
 * DESCRIPTION:	initialize
 */
static void create()
{
    ::create("datagram");
    driver = find_object(DRIVER);
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
 * NAME:	receive_datagram()
 * DESCRIPTION:	forward a message to listeners
 */
static void receive_datagram(string str)
{
    if (this_object()) {
	::receive_message(([ ]), str);
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
    }
}

/*
 * NAME:	message()
 * DESCRIPTION:	send a message to the other side
 */
int message(string str)
{
    return datagram(str);
}
