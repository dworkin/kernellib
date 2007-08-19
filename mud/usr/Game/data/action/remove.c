# include <game/action.h>
# include <game/armor.h>
# include <game/description.h>
# include <game/message.h>
# include <game/selector.h>
# include <game/thing.h>

inherit LIB_ACTION;
inherit UTIL_DESCRIPTION;
inherit UTIL_MESSAGE;

int armor_piece_;

static void create(object LIB_ARMOR_PIECE *armor_pieces)
{
    armor_piece_ = object_number(armor_pieces[0]);
}

void perform(object LIB_CREATURE actor)
{
    object LIB_ARMOR_PIECE armor_piece;

    armor_piece = find_object(armor_piece_);
    if (!armor_piece || (object LIB_THING) environment(armor_piece) != actor) {
        tell_object(actor, "You do not have that.");
        return;
    }

    if (!sizeof(actor->query_worn() & ({ armor_piece }))) {
        tell_object(actor, "You are not wearing that.");
        return;
    }

    actor->remove_worn(armor_piece);
    tell_object(actor, "You remove "
                + definite_description(armor_piece, actor) + ".");
    tell_audience(actor, definite_description(actor) + " removes "
                  + indefinite_description(armor_piece) + ".");
}
