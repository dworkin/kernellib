# include <game/string.h>
# include <game/thing.h>

private inherit UTIL_STRING;

static string normalize_message(string message)
{
    if (has_suffix(message, "\n")) {
        /* preformatted message: prepend blank line */
        return "\n" + message;
    }

    message = normalize_whitespace(message);
    if (!has_suffix(message, ".") && !has_suffix(message, "!")
        && !has_suffix(message, "?"))
    {
        message += ".";
    }
    return indent_string(message);
}

static void tell_object(object LIB_THING observer, string message)
{
    observer->observe(normalize_message(message));
}

static void tell_inventory(object LIB_ROOM room, string message)
{
    object LIB_THING *things;

    int i, size;

    things = inventory(room);
    size = sizeof(things);
    for (i = 0; i < size; ++i) {
        tell_object(things[i], message);
    }
}

static void tell_audience(object LIB_CREATURE actor, string message)
{
    object LIB_ROOM    room;
    object LIB_THING  *things;

    int i, size;

    room = environment(actor);
    if (!room) {
        return;
    }
    things = inventory(room) - ({ actor });
    size = sizeof(things);
    for (i = 0; i < size; ++i) {
        tell_object(things[i], message);
    }
}

static void tell_audience_except(object LIB_CREATURE actor,
                                 object LIB_THING *excluded, string message)
{
    object LIB_ROOM    room;
    object LIB_THING  *things;

    int i, size;

    room = environment(actor);
    if (!room) {
        return;
    }
    things = inventory(room) - ({ actor }) - excluded;
    size = sizeof(things);
    for (i = 0; i < size; ++i) {
        tell_object(things[i], message);
    }
}
