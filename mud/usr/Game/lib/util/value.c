# include <config.h>
# include <type.h>

static mixed min_value(mixed a, mixed b)
{
    return a < b ? a : b;
}

static mixed max_value(mixed a, mixed b)
{
    return a > b ? a : b;
}

static string format_value(mixed val, varargs mapping seen);

static string format_nil()
{
    return "nil";
}

static string format_int(int i)
{
    return (string) i;
}

static string format_float(float f)
{
    string str;

    str = (string) f;
    if (!sscanf(str, "%*s.") && !sscanf(str, "%*se")) {
        str += ".0";
    }
    return str;
}

static string format_string(string str)
{
    return "\"" + str + "\"";
}

static string format_object(object obj)
{
    return "<" + object_name(obj) + ">";
}

static string format_array(mixed *arr, varargs mapping seen)
{
    string str;
    int i, size;

    if (seen && seen[arr]) {
        return "({ ... })";
    }
    if (!seen) {
        seen = ([ ]);
    }
    seen[arr] = TRUE;

    str = "({ ";
    size = sizeof(arr);
    for (i = 0; i < size; ++i) {
        if (i) {
            str += ", ";
        }
        str += format_value(arr[i], seen);
    }
    return str + " })";
}

static string format_mapping(mapping map, varargs mapping seen)
{
    mixed *inds, *vals;
    string str;
    int i, size;

    if (seen && seen[map]) {
        return "([ ... ])";
    }
    if (!seen) {
        seen = ([ ]);
    }
    seen[map] = TRUE;

    inds = map_indices(map);
    vals = map_values(map);
    str = "([ ";

    size = sizeof(inds);
    for (i = 0; i < size; ++i) {
        if (i) {
            str += ", ";
        }
        str += format_value(inds[i], seen) + ": "
            + format_value(vals[i], seen);
    }
    return str + " ])";
}

static string format_value(mixed val, varargs mapping seen)
{
    switch (typeof(val)) {
    case T_NIL:     return format_nil();
    case T_INT:     return format_int(val);
    case T_FLOAT:   return format_float(val);
    case T_STRING:  return format_string(val);
    case T_OBJECT:  return format_object(val);
    case T_ARRAY:   return format_array(val, seen);
    case T_MAPPING: return format_mapping(val, seen);
    }
}
