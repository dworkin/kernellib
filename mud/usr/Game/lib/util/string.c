static string normalize_whitespace(string str)
{
    int len;

    if (sscanf(str, "%*s\n")) {
        str = implode(explode(str, "\n"), " ");
    }
    if (sscanf(str, "%*s\r")) {
        str = implode(explode(str, "\r"), " ");
    }
    if (sscanf(str, "%*s\t")) {
        str = implode(explode(str, "\t"), " ");
    }

    len = strlen(str);
    if (len && (str[0] == ' ' || sscanf(str, "%*s  ") || str[len - 1] == ' '))
    {
        str = implode(explode(str, " ") - ({ "" }), " ");
    }
    return str;
}

static int has_substring(string str, string sub)
{
    if (sscanf(sub, "%*s%%")) {
        sub = implode(explode("%" + sub + "%", "%"), "%%");
    }
    return sscanf(str, "%*s" + sub);
}

static string replace_string(string str, string from, string to)
{
    if (has_substring(str, from)) {
        str = implode(explode(from + str + from, from), to);
    }
    return str;
}

static int has_suffix(string str, string suffix)
{
    int len, suffix_len;

    len = strlen(str);
    suffix_len = strlen(suffix);
    return len >= suffix_len && str[len - suffix_len ..] == suffix;
}

static string replace_suffix(string str, string from, string to)
{
    if (has_suffix(str, from)) {
        return str[.. strlen(str) - strlen(from) - 1] + to;
    } else {
        return str;
    }
}

static string remove_suffix(string str, string suffix)
{
    if (has_suffix(str, suffix)) {
        return str[.. strlen(str) - strlen(suffix) - 1];
    } else {
        return str;
    }
}

static string add_suffix(string str, string suffix)
{
    return has_suffix(str, suffix) ? str : str + suffix;
}

static string capitalize(string str)
{
    if (strlen(str) && str[0] >= 'a' && str[0] <= 'z') {
        str[0] = str[0] - 'a' + 'A';
    }
    return str;
}

static string lower_case(string str)
{
    int i, len;

    len = strlen(str);
    for (i = 0; i < len; ++i) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] += 'a' - 'A';
        }
    }
    return str;
}

static string upper_case(string str)
{
    int i, len;

    len = strlen(str);
    for (i = 0; i < len; ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] += 'A' - 'a';
        }
    }
    return str;
}

static string align_left(string str, int width) {
    while (strlen(str) < width) {
        str += "                                                             ";
    }
    return str[.. width - 1];
}

static string align_right(string str, int width) {
    while (strlen(str) < width) {
        str = "                                                        " + str;
    }
    return str[strlen(str) - width ..];
}

static string repeat_string(string str, int count) {
    int len;

    len = strlen(str) * count;
    if (len <= 0) {
        return "";
    }
    while (strlen(str) < len) {
        str += str;
    }
    return str[.. len - 1];
}

static string break_string(string str, varargs int width)
{
    int i, len, last, best;

    len = strlen(str);
    if (!len || str[len - 1] == '\n') {
        return str;
    }
    if (!width) {
        width = 69;
    }
    if (len <= width) {
        return str + "\n";
    }
    last = best = -1;
    for (i = 0; i < len; ++i) {
        if (str[i] == ' ') {
            if (i - last - 1 <= width) {
                best = i;
            } else {
                if (best == -1) {
                    str[i] = '\n';
                    last = i;
                } else {
                    str[best] = '\n';
                    last = best;
                    best = i;
                }
            }
        }
    }
    if (len - last - 1 > width && best != -1) {
        str[best] = '\n';
    }
    return str + "\n";
}

static string indent_string(string str, varargs int width)
{
    int len;

    len = strlen(str);
    if (!len || str[len - 1] == '\n') {
        return str;
    }
    return "  " + break_string("__" + str, width)[2 ..];
}
