# include <game/action.h>
# include <game/armor.h>
# include <game/description.h>
# include <game/language.h>
# include <game/message.h>
# include <game/string.h>
# include <game/thing.h>
# include <system/assert.h>

inherit LIB_THING;
private inherit UTIL_DESCRIPTION;
private inherit UTIL_LANGUAGE;
private inherit UTIL_MESSAGE;
private inherit UTIL_STRING;

string race_;
string gender_;

int *weapons_;
int *armor_pieces_;

static void create()
{
    ::create();
    add_event("observe");
    add_event("error");
    race_ = "human";
    gender_ = random(2) ? "male" : "female";
    weapons_ = ({ });
    armor_pieces_ = ({ });
}

static void set_race(string race)
{
    ASSERT_ARG(race);
    race_ = race;
    add_noun(race);
}

string query_race()
{
    return race_;
}

static void set_gender(string gender)
{
    ASSERT_ARG(gender == "male" || gender == "female");
    gender_ = gender;
    add_noun(gender);
}

string query_gender()
{
    return gender_;
}

static object *to_objects(int *numbers)
{
    int      i, size;
    object  *objs;

    size = sizeof(numbers);
    objs = allocate(size);
    for (i = 0; i < size; ++i) {
        objs[i] = find_object(numbers[i]);
    }
    return objs;
}

static int *to_numbers(object *objs)
{
    int   i, size;
    int  *numbers;

    size = sizeof(objs);
    numbers = allocate_int(size);
    for (i = 0; i < size; ++i) {
        numbers[i] = object_number(objs[i]);
    }
    return numbers;
}

void add_weapon(object LIB_WEAPON weapon)
{
    ASSERT_ARG(weapon);
    weapons_ |= ({ object_number(weapon) });
}

void remove_weapon(object LIB_WEAPON weapon)
{
    ASSERT_ARG(weapon);
    weapons_ -= ({ object_number(weapon) });
}

object LIB_WEAPON *query_weapons()
{
    object LIB_WEAPON *weapons;

    weapons = to_objects(weapons_) - ({ nil });
    if (sizeof(weapons) != sizeof(weapons_)) {
        weapons_ = to_numbers(weapons);
    }
    return weapons;
}

void add_armor_piece(object LIB_ARMOR_PIECE armor_piece)
{
    ASSERT_ARG(armor_piece);
    armor_pieces_ |= ({ object_number(armor_piece) });
}

void remove_armor(object LIB_ARMOR_PIECE armor_piece)
{
    ASSERT_ARG(armor_piece);
    armor_pieces_ -= ({ object_number(armor_piece) });
}

object LIB_ARMOR_PIECE *query_armor_pieces()
{
    object LIB_ARMOR_PIECE *armor_pieces;

    armor_pieces = to_objects(armor_pieces_) - ({ nil });
    if (sizeof(armor_pieces) != sizeof(armor_pieces_)) {
        armor_pieces_ = to_numbers(armor_pieces);
    }
    return armor_pieces;
}

string query_look(varargs object LIB_THING observer)
{
    string name;
    
    name = query_name();
    return name ? capitalize(name) : indefinite_article(race_) + " " + race_;
}

int allow_subscribe(object obj, string name)
{
    return TRUE;
}

void observe(string mess)
{
    event("observe", mess);
}

void add_action(object LIB_ACTION action)
{
    ASSERT_ARG(action);
    call_out("act", 0, action);
}

static void act(object LIB_ACTION action)
{
    string error;

    error = catch(action->perform(this_object()));
    if (error) {
        event("error", error);
    }
}

static void drop_all()
{
    object LIB_ITEM  *inv;
    object LIB_ROOM   env;

    int i, size;

    env = environment(this_object());
    inv = inventory(this_object());
    size = sizeof(inv);
    for (i = 0; i < size; ++i) {
        move_object(inv[i], env);
    }
}

static void make_corpse()
{
    object LIB_ROOM env;

    env = environment(this_object());
    if (race_ && env) {
        move_object(new_object(CORPSE, race_), env);
    }
}

void die()
{
    drop_all();
    make_corpse();
    tell_audience(this_object(),
                  definite_description(this_object()) + " dies.");
    destruct_object(this_object());
}
