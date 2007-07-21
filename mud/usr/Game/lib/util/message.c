# include <game/string.h>
# include <game/thing.h>

private inherit UTIL_STRING;

static string normalize_message(string mess)
{
    if (has_suffix(mess, "\n")) {
        return mess;
    }

    if (!has_suffix(mess, ".") && !has_suffix(mess, "!")
        && !has_suffix(mess, "?"))
    {
        mess += ".";
    }
    return capitalize(mess) + "\n";
}

static void tell_object(object LIB_THING obs, string mess)
{
    obs->observe(normalize_message(mess));
}

static void tell_inventory(object LIB_ROOM env, string mess)
{
    object LIB_THING  *inv;
    int                i, size;

    inv = inventory(env);
    size = sizeof(inv);
    for (i = 0; i < size; ++i) {
        tell_object(inv[i], mess);
    }
}

static void tell_audience(object LIB_CREATURE actor, string mess)
{
    object LIB_ROOM    env;
    object LIB_THING  *inv;
    int                i, size;

    env = environment(actor);
    if (!env) {
        return;
    }
    inv = inventory(env) - ({ actor });
    size = sizeof(inv);
    for (i = 0; i < size; ++i) {
        tell_object(inv[i], mess);
    }
}

static void tell_audience_except(object LIB_CREATURE actor,
                                 object LIB_THING *excl, string mess)
{
    object LIB_ROOM    env;
    object LIB_THING  *inv;
    int                i, size;

    env = environment(actor);
    if (!env) {
        return;
    }
    inv = inventory(env) - ({ actor }) - excl;
    size = sizeof(inv);
    for (i = 0; i < size; ++i) {
        tell_object(inv[i], mess);
    }
}
