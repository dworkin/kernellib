# include <config.h>

# define DRIVER		"/kernel/sys/driver"
# define AUTO		"/kernel/lib/auto"

/*
 * privilege levels
 */
# define KERNEL()	sscanf(previous_program(), "/kernel/%*s")
# define SYSTEM()	sscanf(previous_program(), "/usr/System/%*s")

/*
 * kernel TLS
 */
# define TLSVAR(tls, n)	tls[-1 - n]

# define TLS_LIMIT	0	/* resource limits */
# define TLS_ARGUMENT	1	/* argument or result */
# define TLS_ERROR	2	/* error */
# define TLS_SOURCE	3	/* source files */
# define TLS_INHERIT	4	/* inherited objects */
# define TLS_USER	5	/* current user */
# define TLS_RESOURCE	6	/* resources incremented */
# define TLS_PUT_ATOMIC	7	/* send message through atomic barrier */
# define TLS_GET_ATOMIC	8	/* retrieve message through atomic barrier */
