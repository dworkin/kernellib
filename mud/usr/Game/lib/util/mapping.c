# include <game/value.h>

private inherit UTIL_VALUE;

static mapping make_mapping(mixed *indices, varargs mixed *values)
{
    int      i, size;
    mapping  map;

    map = ([ ]);
    if (values) {
        size = min_value(sizeof(indices), sizeof(values));
        for (i = 0; i < size; ++i) {
            map[indices[i]] = values[i];
        }
    } else {
        size = sizeof(indices);
        for (i = 0; i < size; ++i) {
            map[indices[i]] = TRUE;
        }
    }
    return map;
}
