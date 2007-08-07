static float sum_attributes(mapping attributes)
{
    float  *values, sum;
    int     i, size;

    values = map_values(attributes);
    sum = 0.0;
    size = sizeof(values);
    for (i = 0; i < size; ++i) {
        sum += values[i];
    }
    return sum;
}

static mapping scale_attributes(mapping attributes, float factor)
{
    string  *indices;
    int      i, size;

    attributes = attributes[..];
    indices = map_indices(attributes);
    size = sizeof(indices);
    for (i = 0; i < size; ++i) {
        attributes[indices[i]] *= factor;
    }
    return attributes;
}

static mapping normalize_attributes(mapping attributes)
{
    int    size;
    float  average;

    size = map_sizeof(attributes);
    if (!size) {
        return ([ ]);
    }
    average = sum_attributes(attributes) / (float) size;
    return average ? scale_attributes(attributes, 1.0 / average)
        : attributes[..];
}

private float nil_to_float(mixed val)
{
    return val == nil ? 0.0 : val;
}

static mapping add_attributes(mapping first, mapping second)
{
    string   *indices;
    int       i, size;
    mapping   map;

    indices = map_indices(first) | map_indices(second);
    map = ([ ]);
    size = sizeof(indices);
    for (i = 0; i < size; ++i) {
        map[indices[i]] = nil_to_float(first[indices[i]])
            + nil_to_float(second[indices[i]]);
    }
    return map;
}
