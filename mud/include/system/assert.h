# include <config.h>

# define ASSERT_MESSAGE(expr, mess)  if (!(expr)) error(mess)

# define ASSERT(expr)  ASSERT_MESSAGE(expr, "Assertion failed: " + #expr)
# define ASSERT_ACCESS(expr)    ASSERT_MESSAGE(expr, "Access denied")
# define ASSERT_ARG(expr)       ASSERT_MESSAGE(expr, "Bad argument")
# define ASSERT_ARG_N(n, expr)  ASSERT_MESSAGE(expr, "Bad argument " + #n)
# define ASSERT_ARG_1(expr)     ASSERT_ARG_N(1, expr)
# define ASSERT_ARG_2(expr)     ASSERT_ARG_N(2, expr)
# define ASSERT_ARG_3(expr)     ASSERT_ARG_N(3, expr)

# ifdef DEBUG
#   define DEBUG_ASSERT(expr)  ASSERT(expr)
# else
#   define DEBUG_ASSERT(expr)
# endif
