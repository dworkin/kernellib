# include <game/armor.h>
# include <system/assert.h>

static int is_armor_type(string str)
{
    ASSERT_ARG(str);
    switch (str) {
    case BODY_ARMOR_PIECE:
    case BOOT_PIECE:
    case CLOAK_PIECE:
    case GLOVE_PIECE:
    case HELMET_PIECE:
    case SHIELD_PIECE:
    case SHIRT_PIECE:
        
    case AMULET_PIECE:
    case BELT_PIECE:
    case RING_PIECE:
        return TRUE;

    default:
        return FALSE;
    }
}
