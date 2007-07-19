# include <config.h>
# include <type.h>
# include <game/string.h>

private inherit UTIL_STRING;

static mixed min_value(mixed a, mixed b)
{
    return a < b ? a : b;
}

static mixed max_value(mixed a, mixed b)
{
    return a > b ? a : b;
}

static string dump_value(mixed val, varargs mapping seen);

static string dump_nil()
{
    return "nil";
}

static string dump_int(int i)
{
    return (string) i;
}

static string dump_float(float f)
{
    string str;

    str = (string) f;
    if (!has_substring(str, ".") && !has_substring(str, "e")) {
        str += ".0";
    }
    return str;
}

static string dump_string(string str)
{
    str = replace_string(str, "\\", "\\\\");
    str = replace_string(str, "\"", "\\\"");
    str = replace_string(str, "\n", "\\n");
    str = replace_string(str, "\t", "\\t");
    return "\"" + str + "\"";
}

static string dump_object(object obj)
{
    return "<" + object_name(obj) + ">";
}

static string dump_array(mixed *arr, varargs mapping seen)
{
    string  str;
    int     i, size;

    if (seen && seen[arr]) {
        return "#" + (seen[arr] - 1);
    }
    if (!seen) {
        seen = ([ ]);
    }
    seen[arr] = map_sizeof(seen) + 1;

    str = "({ ";
    size = sizeof(arr);
    for (i = 0; i < size; ++i) {
        if (i) {
            str += ", ";
        }
        str += dump_value(arr[i], seen);
    }
    return str + " })";
}

static string dump_mapping(mapping map, varargs mapping seen)
{
    mixed  *inds, *vals;
    string  str;
    int     i, size;

    if (seen && seen[map]) {
        return "@" + (seen[map] - 1);
    }
    if (!seen) {
        seen = ([ ]);
    }
    seen[map] = map_sizeof(seen) + 1;

    inds = map_indices(map);
    vals = map_values(map);
    str = "([ ";

    size = sizeof(inds);
    for (i = 0; i < size; ++i) {
        if (i) {
            str += ", ";
        }
        str += dump_value(inds[i], seen) + ": "
            + dump_value(vals[i], seen);
    }
    return str + " ])";
}

static string dump_value(mixed val, varargs mapping seen)
{
    switch (typeof(val)) {
    case T_NIL:     return dump_nil();
    case T_INT:     return dump_int(val);
    case T_FLOAT:   return dump_float(val);
    case T_STRING:  return dump_string(val);
    case T_OBJECT:  return dump_object(val);
    case T_ARRAY:   return dump_array(val, seen);
    case T_MAPPING: return dump_mapping(val, seen);
    }
}
