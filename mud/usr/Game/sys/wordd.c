# include <game/string.h>
# include <game/word.h>

inherit UTIL_STRING;

mapping words_;

string *query_words()
{
    return map_indices(words_);
}

int query_bits(string word)
{
    mixed bits;

    bits = words_[word];
    return bits ? bits : 0;
}

static void set_bits(string word, int bits)
{
    if (words_[word]) {
        words_[word] |= bits;
    } else {
        words_[word] = bits;
    }
}

static void clear_bits(string word, int bits)
{
    if (words_[word]) {
        words_[word] &= ~bits;
        if (!words_[word]) {
            words_[word] = nil;
        }
    }
}

static string *read_lines(string file)
{
    string str, *lines;
    int i, size;

    str = read_file(file);
    if (!str) {
        return ({ });
    }
    lines = explode("\n" + str + "\n", "\n");
    size = sizeof(lines);
    for (i = 0; i < size; ++i) {
        lines[i] = normalize_whitespace(lines[i]);
    }
    return lines;
}

static void load_adjectives()
{
    string *lines;
    int i, size;

    lines = read_lines("adjective.list");
    size = sizeof(lines);
    for (i = 0; i < size; ++i) {
        if (strlen(lines[i])) {
            if (sscanf(lines[i], "%*s ")) {
                error("Too many words on line " + (i + 1));
            }
            set_bits(lines[i], WORD_ADJECTIVE);
        }
    }
}

static string plural_form(string str)
{
    int len;

    len = strlen(str);
    if (len && str[len - 1] == 'f') {
        str = str[.. len - 2] + "ves";
    } else if (len && str[len - 1] == 's') {
        str += "es";
    } else {
        str += "s";
    }
    return str;
}

static void load_nouns()
{
    string *lines;
    int i, size;

    lines = read_lines("noun.list");
    size = sizeof(lines);
    for (i = 0; i < size; ++i) {
        string sing, plur;

        if (strlen(lines[i])) {
            if (sscanf(lines[i], "%s %s", sing, plur) == 2) {
                if (sscanf(plur, "%*s ")) {
                    error("Too many words on line " + (i + 1));
                }
                if (plur == plural_form(sing)) {
                    error("Redundant plural form on line " + (i + 1));
                }
            } else {
                sing = lines[i];
                plur = plural_form(sing);
            }
            set_bits(sing, WORD_NOUN | WORD_SINGULAR);
            set_bits(plur, WORD_NOUN | WORD_PLURAL);
        }
    }
}

static void create()
{
    words_ = ([ ]);
    load_adjectives();
    load_nouns();
}
