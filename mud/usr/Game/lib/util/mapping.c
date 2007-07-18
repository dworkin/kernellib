# include <game/value.h>

private inherit UTIL_VALUE;

static mapping make_mapping(mixed *inds, varargs mixed *vals)
{
    int i, size;
    mapping map;

    map = ([ ]);
    if (vals) {
        size = min_value(sizeof(inds), sizeof(vals));
        for (i = 0; i < size; ++i) {
            map[inds[i]] = vals[i];
        }
    } else {
        size = sizeof(inds);
        for (i = 0; i < size; ++i) {
            map[inds[i]] = TRUE;
        }
    }
    return map;
}
