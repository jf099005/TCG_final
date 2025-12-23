#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include<map>
#ifndef BOARD_ANALYZER_H
#define BOARD_ANALYZER_H
//tools for analyze board
extern std::map<PieceType, int> Piece_Value;


inline constexpr int InitialPiecesNumber[8] = {
    1, 2, 2, 2, 2, 2, 5
};

inline constexpr Direction AllMoveDirections[] = {
    NORTH, EAST, SOUTH, WEST,
    // NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST
};

inline Square Adjacent[32][4];
inline int num_Adjacent[32];

void init_Adjacent_table();

const int Piece_Value_Max = 8;

inline std::map<PieceType, int> Piece_Value = {
    {General, 20},
    {Advisor, 25},
    {Elephant, 18},
    {Chariot, 5},
    {Horse, 3},
    {Cannon, 18},
    {Soldier, 1}
};


inline constexpr short AllMoveDirections_size = 4;

inline Color Opponent[] = {Red, Black};

Color is_endgame(Position pos);

int pieces_score(const Position &pos, Board pieces_location);
int exp_pieces_score(const Position &pos, Board pieces_location);

#endif