static string normalize_whitespace(string str)
{
    if (sscanf(str, "%*s\n")) {
        str = implode(explode(str, "\n"), "");
    }
    if (sscanf(str, "%*s\r")) {
        str = implode(explode(str, "\r"), "");
    }
    if (sscanf(str, "%*s\t")) {
        str = implode(explode(str, "\t"), "");
    }
    if (sscanf(str, "%*s ")) {
        str = implode(explode(str, " ") - ({ "" }), " ");
    }
    return str;
}
