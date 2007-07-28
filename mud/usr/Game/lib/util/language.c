# include <system/assert.h>

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
