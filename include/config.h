# define USR_DIR		"/usr"	/* default user directory */
# define INHERITABLE_SUBDIR	"/lib/"
# define CLONABLE_SUBDIR	"/obj/"
# define LIGHTWEIGHT_SUBDIR	"/data/"

/* 
 * Support both the old pre 1.4 network package, and 1.4's network extensions
 */
# if definded( __NETWORK_PACKAGE__ ) || defined( __NETWORK_EXTENSIONS__ )
#  define SYS_NETWORKING	/* Network package is enabled */
# endif

# ifdef SYS_NETWORKING
#  define TELNET_PORT	6047	/* default telnet port */
#  define BINARY_PORT	6048	/* default binary port */
# endif

# undef SYS_PERSISTENT			/* off by default */
