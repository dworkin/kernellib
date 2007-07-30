# include <game/gender.h>
# include <system/assert.h>

private inherit UTIL_GENDER;

static string format_human_int(int i)
{
    string str;
    int minus, j;

    str = (string) i;
    minus = sscanf(str, "-%s", str);
    for (j = strlen(str) - 3; j >= 1; j -= 3) {
        str = str[.. j - 1] + "," + str[j ..];
    }
    return minus ? "-" + str : str;
}

static mixed parse_human_int(string str)
{
    int minus, len, i;

    minus = sscanf(str, "-%s", str);
    len = strlen(str);
    if (!len || str[0] == ' ') {
        return nil;
    }
    if (sscanf(str, "%*s,")) {
        if (str[0] == ',' || str[len - 1] == ',' || sscanf(str, "%*s,,")) {
            return nil;
        }
        str = implode(explode(str, ","), "");
    }
    if (sscanf(str, "%d%*c", i) != 1) {
        return nil;
    }
    return minus ? -i : i;
}

static string format_count(int count)
{
    ASSERT_ARG(count >= 1);
    switch (count) {
    case  1: return "one";
    case  2: return "two";
    case  3: return "three";
    case  4: return "four";
    case  5: return "five";
    case  6: return "six";
    case  7: return "seven";
    case  8: return "eight";
    case  9: return "nine";
    case 10: return "ten";
    case 11: return "eleven";
    case 12: return "twelve";
    }
    return format_human_int(count);
}

static mixed parse_count(string str) {
    mixed count;

    switch (str) {
    case "one":     return  1;
    case "two":     return  2;
    case "three":   return  3;
    case "four":    return  4;
    case "five":    return  5;
    case "six":     return  6;
    case "seven":   return  7;
    case "eight":   return  8;
    case "nine":    return  9;
    case "ten":     return 10;
    case "eleven":  return 11;
    case "twelve":  return 12;
    }
    count = parse_human_int(str);
    return count != nil && count >= 1 ? count : nil;
}

static string ordinal_suffix(int i)
{
    ASSERT_ARG(i >= 1);
    switch (i % 100) {
    case 11 .. 19:
        return "th";

    default:
        switch (i % 10) {
        case 1:   return "st";
        case 2:   return "nd";
        case 3:   return "rd";
        default:  return "th";
        }
    }
}

static string format_ordinal(int ordinal)
{
    ASSERT_ARG(ordinal >= 1);
    switch (ordinal) {
    case  1: return "first";
    case  2: return "second";
    case  3: return "third";
    case  4: return "fourth";
    case  5: return "fifth";
    case  6: return "sixth";
    case  7: return "seventh";
    case  8: return "eighth";
    case  9: return "ninth";
    case 10: return "tenth";
    case 11: return "eleventh";
    case 12: return "twelveth";
    }

    return format_human_int(ordinal) + ordinal_suffix(ordinal);
}

static mixed parse_ordinal(string str)
{
    int    len;
    mixed  ordinal;

    switch (str) {
    case "first":     return  1;
    case "second":    return  2;
    case "third":     return  3;
    case "fourth":    return  4;
    case "fifth":     return  5;
    case "sixth":     return  6;
    case "seventh":   return  7;
    case "eighth":    return  8;
    case "ninth":     return  9;
    case "tenth":     return 10;
    case "eleventh":  return 11;
    case "twelveth":  return 12;
    }

    len = strlen(str);
    if (len < 3) {
        return nil;
    }
    ordinal = parse_human_int(str[.. len - 3]);
    if (ordinal == nil || ordinal < 1) {
        return nil;
    }
    return str[len - 2 ..] == ordinal_suffix(ordinal) ? ordinal : nil;
}

static string indefinite_article(string str)
{
    if (!strlen(str)) {
        return "a";
    }

    switch (str[0]) {
    case 'a': case 'e': case 'i': case 'o': case 'u': case 'y':
        return "an";

    default:
        return "a";
    }
}

static string list_strings(string *arr)
{
    int size;

    size = sizeof(arr);
    switch (size) {
    case 0:
        return "";

    case 1:
        return arr[0];

    case 2:
        return arr[0] + " and " + arr[1];

    default:
        return implode(arr[.. size - 2], ", ") + ", and " + arr[size - 1];
    }
}

static string nominative_pronoun(string gender)
{
    ASSERT_ARG(gender && is_gender(gender));
    switch (gender) {
    case FEMALE_GENDER:  return "she";
    case MALE_GENDER:    return "he";
    case NEUTER_GENDER:  return "it";
    }
}

static string accusative_pronoun(string gender)
{
    ASSERT_ARG(gender && is_gender(gender));
    switch (gender) {
    case FEMALE_GENDER:  return "her";
    case MALE_GENDER:    return "him";
    case NEUTER_GENDER:  return "it";
    }
}

static string reflective_pronoun(string gender)
{
    ASSERT_ARG(gender && is_gender(gender));
    switch (gender) {
    case FEMALE_GENDER:  return "herself";
    case MALE_GENDER:    return "himself";
    case NEUTER_GENDER:  return "itself";
    }
}

static string possessive_pronoun(string gender)
{
    ASSERT_ARG(gender && is_gender(gender));
    switch (gender) {
    case FEMALE_GENDER:  return "her";
    case MALE_GENDER:    return "his";
    case NEUTER_GENDER:  return "its";
    }
}
