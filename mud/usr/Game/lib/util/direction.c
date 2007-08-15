# include <game/direction.h>
# include <system/assert.h>

static int is_horizontal_direction(string str)
{
    ASSERT_ARGUMENT(str);
    switch (str) {
    case EAST_DIRECTION:
    case NORTH_DIRECTION:
    case NORTHEAST_DIRECTION:
    case NORTHWEST_DIRECTION:
    case SOUTH_DIRECTION:
    case SOUTHEAST_DIRECTION:
    case SOUTHWEST_DIRECTION:
    case WEST_DIRECTION:
        return TRUE;

    default:
        return FALSE;
    }
}

static int is_vertical_direction(string str)
{
    ASSERT_ARGUMENT(str);
    return str == DOWN_DIRECTION || str == UP_DIRECTION;
}

static int is_direction(string str)
{
    ASSERT_ARGUMENT(str);
    return is_horizontal_direction(str) || is_vertical_direction(str);
}

static string reverse_direction(string str)
{
    ASSERT_ARGUMENT(str && is_direction(str));
    switch (str) {
    case EAST_DIRECTION:       return WEST_DIRECTION;
    case NORTH_DIRECTION:      return SOUTH_DIRECTION;
    case NORTHEAST_DIRECTION:  return SOUTHWEST_DIRECTION;
    case NORTHWEST_DIRECTION:  return SOUTHEAST_DIRECTION;
    case SOUTH_DIRECTION:      return NORTH_DIRECTION;
    case SOUTHEAST_DIRECTION:  return NORTHWEST_DIRECTION;
    case SOUTHWEST_DIRECTION:  return NORTHEAST_DIRECTION;
    case WEST_DIRECTION:       return EAST_DIRECTION;

    case DOWN_DIRECTION:       return UP_DIRECTION;
    case UP_DIRECTION:         return DOWN_DIRECTION;
    }
}
