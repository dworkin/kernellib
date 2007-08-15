# include <limits.h>
# include <game/action.h>
# include <game/armor.h>
# include <game/attribute.h>
# include <game/description.h>
# include <game/gender.h>
# include <game/guild.h>
# include <game/language.h>
# include <game/message.h>
# include <game/race.h>
# include <game/string.h>
# include <game/thing.h>
# include <system/assert.h>

inherit aff    LIB_ATTRIBUTE_AFFECTOR;
inherit thing  LIB_THING;

private inherit UTIL_ATTRIBUTE;
private inherit UTIL_DESCRIPTION;
private inherit UTIL_LANGUAGE;
private inherit UTIL_MESSAGE;
private inherit UTIL_STRING;

string gender_;

object LIB_RACE   race_;
object LIB_GUILD  guild_;

int *wielded_;
int *worn_;

float    level_;
mapping  base_attributes_, attributes_;
float    health_;
float    power_;

static float rnd()
{
    return (float) random(INT_MAX) / (float) INT_MAX;
}

static mapping make_base_attributes()
{
    mapping attributes;

    return normalize_attributes(([ STRENGTH_ATTRIBUTE:    rnd(),
                                   SPEED_ATTRIBUTE:       rnd(),
                                   PERCEPTION_ATTRIBUTE:  rnd(),
                                   CHARISMA_ATTRIBUTE:    rnd() ]));
}

static void update_attributes()
{
    mapping attributes;

    attributes = base_attributes_;
    if (race_) {
        attributes = add_attributes(attributes,
                                    race_->query_attribute_shares());
    }
    if (guild_) {
        attributes = add_attributes(attributes,
                                    guild_->query_attribute_shares());
    }
    attributes_ = scale_attributes(normalize_attributes(attributes), level_);
}

void set_level(float level)
{
    level_ = level;
    update_attributes();
}

float query_level()
{
    return level_;
}

static void create()
{
    aff::create();
    thing::create();
    add_event("observe");
    add_event("error");

    gender_ = random(2) ? "female" : "male";
    race_ = find_object(HUMAN_RACE);
    guild_ = find_object(WARRIOR_GUILD);

    wielded_ = ({ });
    worn_ = ({ });

    attributes_ = ([ ]);
    health_ = 0.5 + 0.5 * rnd();
    power_ = 0.5 + 0.5 * rnd();

    base_attributes_ = make_base_attributes();
    set_level(10.0 + 10.0 * rnd());
}

int has_singular_noun(string str)
{
    return race_ && race_->has_singular_noun(str)
        || guild_ && guild_->has_singular_noun(str)
        || ::has_singular_noun(str);
}

int has_plural_noun(string str)
{
    return race_ && race_->has_plural_noun(str)
        || guild_ && guild_->has_plural_noun(str)
        || ::has_plural_noun(str);
}

void set_gender(string gender)
{
    ASSERT_ARGUMENT(gender == MALE_GENDER || gender == FEMALE_GENDER);
    gender_ = gender;
}

string query_gender()
{
    return gender_;
}

void set_race(object LIB_RACE race)
{
    ASSERT_ARGUMENT(race);
    race_ = race;
    update_attributes();
}

object LIB_RACE query_race()
{
    return race_;
}

void set_guild(object LIB_GUILD guild)
{
    guild_ = guild;
    update_attributes();
}

object LIB_GUILD query_guild()
{
    return guild_;
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

void add_wielded(object LIB_WEAPON weapon)
{
    ASSERT_ARGUMENT(weapon);
    wielded_ |= ({ object_number(weapon) });
}

void remove_wielded(object LIB_WEAPON weapon)
{
    ASSERT_ARGUMENT(weapon);
    wielded_ -= ({ object_number(weapon) });
}

object LIB_WEAPON *query_wielded()
{
    object LIB_WEAPON *wielded;

    wielded = to_objects(wielded_) - ({ nil });
    if (sizeof(wielded) != sizeof(wielded_)) {
        wielded_ = to_numbers(wielded);
    }
    return wielded;
}

void add_worn(object LIB_ARMOR_PIECE armor_piece)
{
    ASSERT_ARGUMENT(armor_piece);
    worn_ |= ({ object_number(armor_piece) });
}

void remove_worn(object LIB_ARMOR_PIECE armor_piece)
{
    ASSERT_ARGUMENT(armor_piece);
    worn_ -= ({ object_number(armor_piece) });
}

object LIB_ARMOR_PIECE *query_worn()
{
    object LIB_ARMOR_PIECE *worn;

    worn = to_objects(worn_) - ({ nil });
    if (sizeof(worn) != sizeof(worn_)) {
        worn_ = to_numbers(worn);
    }
    return worn;
}

string query_look(varargs object LIB_THING observer)
{
    string name;
    
    name = query_name();
    if (name) {
        return capitalize(name);
    }

    if (race_ && guild_) {
        name = race_->query_name() + " " + guild_->query_name();
    } else if (race_) {
        name = race_->query_name();
    } else if (guild_) {
        name = guild_->query_name();
    } else {
        name = "creature";
    }
    name = lower_case(name);
    return indefinite_article(name) + " " + name;
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
    ASSERT_ARGUMENT(action);
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
    object LIB_ITEM  *items;
    object LIB_ROOM   room;

    int i, size;

    room = environment(this_object());
    items = inventory(this_object());
    size = sizeof(items);
    for (i = 0; i < size; ++i) {
        items[i]->move(room);
    }
}

static void make_corpse()
{
    object LIB_ROOM room;

    room = environment(this_object());
    if (race_ && room) {
        new_object(CORPSE, race_)->move(room);
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

int allow_move(object obj)
{
    return obj <- LIB_ITEM;
}

int move(object destination)
{
    if (destination
        && (!(destination <- LIB_ROOM)
            || !destination->allow_move(this_object())))
    {
        return FALSE;
    }
    move_object(destination);
    return TRUE;
}

static object LIB_ATTRIBUTE_AFFECTOR *query_attribute_affectors(string name)
{
    return ({ this_object(), race_, guild_ }) - ({ nil })
        + query_wielded() + query_worn();
}

float query_attribute(string name)
{
    float  value;
    int    i, size;

    object LIB_ATTRIBUTE_AFFECTOR *affectors;

    value = attributes_[name] ? attributes_[name] : 0.0;

    affectors = query_attribute_affectors(name);
    size = sizeof(affectors);
    for (i = 0; i < size; ++i) {
        value += affectors[i]->affect_attribute(this_object(), name);
    }
    return value;
}

float affect_attribute(object LIB_CREATURE creature, string name)
{
    switch (name) {
    case AGILITY_ATTRIBUTE:
        return (query_attribute(STRENGTH_ATTRIBUTE)
                + query_attribute(SPEED_ATTRIBUTE)) / 2.0;

    case ATTACK_ATTRIBUTE:
        return (query_attribute(STRENGTH_ATTRIBUTE)
                + query_attribute(SPEED_ATTRIBUTE)) / 2.0;

    case BARGAIN_ATTRIBUTE:
        return query_attribute(CHARISMA_ATTRIBUTE);

    case DAMAGE_ATTRIBUTE:
        return query_attribute(STRENGTH_ATTRIBUTE);

    case DEFENSE_ATTRIBUTE: 
        return query_attribute(SPEED_ATTRIBUTE);

    case DEXTERITY_ATTRIBUTE:
        return (query_attribute(SPEED_ATTRIBUTE)
                + query_attribute(PERCEPTION_ATTRIBUTE)) / 2.0;

    case LEADERSHIP_ATTRIBUTE:
        return (query_attribute(CHARISMA_ATTRIBUTE)
                + query_attribute(PERCEPTION_ATTRIBUTE)) / 2.0;

    case MAGIC_ATTRIBUTE:
        return query_attribute(PERCEPTION_ATTRIBUTE);

    case MUSIC_ATTRIBUTE:
        return query_attribute(CHARISMA_ATTRIBUTE);

    case PRAYER_ATTRIBUTE:
        return (query_attribute(CHARISMA_ATTRIBUTE)
                + query_attribute(PERCEPTION_ATTRIBUTE)) / 2.0;

    case PROTECTION_ATTRIBUTE:
        return query_attribute(STRENGTH_ATTRIBUTE);

    case STEALTH_ATTRIBUTE:
        return (query_attribute(SPEED_ATTRIBUTE)
                + query_attribute(PERCEPTION_ATTRIBUTE)) / 2.0;

    default:
        return 0.0;
    }
}

void set_health(float health)
{
    health_ = health;
    if (health_ <= 0.0) {
        health_ = 0.0;
        call_out("die", 0);
    }
}

float query_health()
{
    return health_;
}

void set_power(float power)
{
    power_ = power;
}

float query_power()
{
    return power_;
}
