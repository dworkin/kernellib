# include <config.h>

# define ASSERT_MESSAGE(expression, message) \
    if (!(expression)) error(__FILE__ + ", " + __LINE__ + ": " + message)

# define ASSERT(expression) \
    ASSERT_MESSAGE(expression, "Assertion failed: " + #expression)
# define ASSERT_ACCESS(expression) \
    ASSERT_MESSAGE(expression, "Access denied")
# define ASSERT_ARGUMENT(expression) \
    ASSERT_MESSAGE(expression, "Bad argument")
# define ASSERT_ARGUMENT_N(n, expression) \
    ASSERT_MESSAGE(expression, "Bad argument " + #n)

# define ASSERT_ARGUMENT_1(expression)  ASSERT_ARGUMENT_N(1, expression)
# define ASSERT_ARGUMENT_2(expression)  ASSERT_ARGUMENT_N(2, expression)
# define ASSERT_ARGUMENT_3(expression)  ASSERT_ARGUMENT_N(3, expression)

# ifdef DEBUG
#   define DEBUG_ASSERT(expression) \
      ASSERT_MESSAGE(expression, "Debug assertion failed: " + #expression)
# else
#   define DEBUG_ASSERT(expression)
# endif
