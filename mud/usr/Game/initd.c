# include <limits.h>
# include <game/action.h>
# include <game/command.h>
# include <game/guild.h>
# include <game/race.h>
# include <game/selector.h>
# include <game/word.h>
# include <game/thing.h>
# include <system/assert.h>

# define BAG     "/usr/Game/obj/bag"
# define CRYPT   "/usr/Game/room/crypt"
# define DWARF   "/usr/Game/obj/dwarf"
# define ELF     "/usr/Game/obj/elf"
# define SHIELD  "/usr/Game/data/shield"
# define SWORD   "/usr/Game/data/sword"
# define TEMPLE  "/usr/Game/room/temple"

object wordd_;
object commandd_;
object temple_;

static void create()
{
    object LIB_ROOM crypt;

    compile_object(DROP_ACTION);
    compile_object(GIVE_ACTION);
    compile_object(GO_ACTION);
    compile_object(INVENTORY_ACTION);
    compile_object(LOOK_ACTION);
    compile_object(LOOK_TO_ACTION);
    compile_object(PUT_IN_ACTION);
    compile_object(RELEASE_ACTION);
    compile_object(REMOVE_ACTION);
    compile_object(SAY_ACTION);
    compile_object(SCORE_ACTION);
    compile_object(TAKE_ACTION);
    compile_object(TAKE_FROM_ACTION);
    compile_object(WEAR_ACTION);
    compile_object(WIELD_ACTION);

    compile_object(SIMPLE_SELECTOR);
    compile_object(ORDINAL_SELECTOR);
    compile_object(COUNT_SELECTOR);
    compile_object(ALL_OF_SELECTOR);
    compile_object(ALL_SELECTOR);
    compile_object(LIST_SELECTOR);
    compile_object(EXCEPT_SELECTOR);

    compile_object(DROP_COMMAND);
    compile_object(GIVE_COMMAND);
    compile_object(LOOK_AT_COMMAND);
    compile_object(PUT_IN_COMMAND);
    compile_object(RELEASE_COMMAND);
    compile_object(REMOVE_COMMAND);
    compile_object(SAY_TO_COMMAND);
    compile_object(TAKE_COMMAND);
    compile_object(TAKE_FROM_COMMAND);
    compile_object(WEAR_COMMAND);
    compile_object(WIELD_COMMAND);

    wordd_ = compile_object(WORDD);
    commandd_ = compile_object(COMMANDD);

    compile_object(ELF_RACE);
    compile_object(DWARF_RACE);
    compile_object(GOBLIN_RACE);
    compile_object(HUMAN_RACE);
    compile_object(LEPRECHAUN_RACE);
    compile_object(TROLL_RACE);

    compile_object(BARD_GUILD);
    compile_object(KNIGHT_GUILD);
    compile_object(MONK_GUILD);
    compile_object(PRIEST_GUILD);
    compile_object(RANGER_GUILD);
    compile_object(THIEF_GUILD);
    compile_object(WARRIOR_GUILD);
    compile_object(WIZARD_GUILD);

    compile_object(CREATURE);

    temple_ = compile_object(TEMPLE);
    crypt = compile_object(CRYPT); 
    compile_object(DWARF);
    compile_object(ELF);
    compile_object(CORPSE);
    compile_object(BAG);
    compile_object(SWORD);
    compile_object(SHIELD);
    compile_object(COIN);

    move_object(clone_object(BAG), temple_);
    move_object(clone_object(DWARF), crypt);
    move_object(new_object(SWORD), crypt);
    move_object(new_object(SHIELD), crypt);
    move_object(new_object(COIN, "silver", 1 + random(13)), crypt);
}

static string select_element(string *array)
{
    return array[random(sizeof(array))];
}

static string select_gender(string *options)
{
    string *genders;

    genders = ({ "female", "male" });
    options &= genders;
    return select_element(sizeof(options) ? options : genders);
}

static object LIB_RACE select_race(string *options)
{
    string *races, race;

    races = ({ "dwarf", "elf", "goblin", "human", "leprechaun", "troll" });
    options &= races;
    race = select_element(sizeof(options) ? options : races);
    return find_object(RACE_DIR + "/" + race);
}

static object LIB_GUILD select_guild(string *options)
{
    string *guilds, guild;

    guilds = ({ "bard", "knight", "monk", "priest", "ranger", "thief",
                "warrior", "wizard" });
    options &= guilds;
    guild = select_element(sizeof(options) ? options : guilds);
    return find_object(GUILD_DIR + "/" + guild);
}

object LIB_CREATURE make_creature(varargs string str)
{
    string  *options, gender;

    object LIB_RACE      race;
    object LIB_GUILD     guild;
    object LIB_CREATURE  creature;

    options = str ? explode(str, " ") - ({ "" }) : ({ });

    creature = clone_object(CREATURE);
    creature->set_gender(select_gender(options));
    creature->set_race(select_race(options));
    creature->set_guild(select_guild(options));
    
    move_object(creature, temple_);
    return creature;
}
