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
# define TLS_LIMIT	0	/* resource limits */
# define TLS_ARGUMENT	1	/* argument, result or error */
# define TLS_SOURCE	2	/* source files */
# define TLS_INHERIT	3	/* inherited objects */
# define TLS_USER	4	/* current user */
# define TLS_RESOURCE	5	/* resources incremented */
# define TLS_PUT_ATOMIC	6	/* send message through atomic barrier */
# define TLS_GET_ATOMIC	7	/* retrieve message through atomic barrier */
