# include <system/assert.h>

static int is_armor_type(string str)
{
    ASSERT_ARG(str);
    switch (str) {
    case "amulet":
    case "armor":
    case "belt":
    case "boot":
    case "cloak":
    case "glove":
    case "helmet":
    case "ring":
    case "shield":
    case "shirt":
        return TRUE;

    default:
        return FALSE;
    }
}
