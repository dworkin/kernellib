# include <game/gender.h>
# include <system/assert.h>

static int is_gender(string str)
{
    ASSERT_ARG(str);
    switch (str) {
    case FEMALE_GENDER:
    case MALE_GENDER:
    case NEUTER_GENDER:
        return TRUE;

    default:
        return FALSE;
    }
}
