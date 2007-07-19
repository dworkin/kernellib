static int is_horizontal_direction(string str)
{
    switch (str) {
    case "east":
    case "north":
    case "northeast":
    case "northwest":
    case "south":
    case "southeast":
    case "southwest":
    case "west":
        return TRUE;

    default:
        return FALSE;
    }
}

static int is_vertical_direction(string str)
{
    switch (str) {
    case "down":
    case "up":
        return TRUE;

    default:
        return FALSE;
    }
}

static int is_direction(string str)
{
    return is_horizontal_direction(str) || is_vertical_direction(str);
}

static string reverse_direction(string str)
{
    switch (str) {
    case "east":      return "west";
    case "north":     return "south";
    case "northeast": return "southwest";
    case "northwest": return "southeast";
    case "south":     return "north";
    case "southeast": return "northwest";
    case "southwest": return "northeast";
    case "west":      return "east";

    case "down":      return "up";
    case "up":        return "down";

    default:          return nil;
    }
}
