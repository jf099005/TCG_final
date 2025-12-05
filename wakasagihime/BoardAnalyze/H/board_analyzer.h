#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include<map>
#ifndef __BOARD_ANALYZER__
#define __BOARD_ANALYZER__

extern std::map<PieceType, int> Piece_Value;

int InitialPiecesNumber[8] = {
    1, 2, 2, 2, 2, 2, 5
};

Color is_endgame(Position pos);

int pieces_score(const Position &pos, Board pieces_location);

#endif