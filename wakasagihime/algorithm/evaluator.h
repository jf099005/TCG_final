#ifndef __BOARD_EVALUATOR__
#define __BOARD_EVALUATOR__

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include"board_analyzer.h"


typedef double Score;

namespace CDCEvaluate{
    const double score_mx = 1145140;


    const int full_distance_score_per_piece[7] = {
        20*5+10,//General
        20+10*2, //Advisor
        20*3+10*2, //Elephant
        20*5+10*2, //Chariot
        20*7+10*2, //Horse
        20*9, //Cannon
        20*9+10*5 //Soldier
    };

    const int full_distance_score[7] = {
        (20*5+10)*1,//General
        (20+10*2)*2, //Advisor
        (20*3+10*2)*2, //Elephant
        (20*5+10*2)*2, //Chariot
        (20*7+10*2)*2, //Horse
        20*9*2, //Cannon
        (20*9+10*5)*5 //Soldier
    };

    Score distance_score(const Position& pos, Color side, Color opponent, PieceType opponent_type);

    Score calculate_score(const Position& pos, int remain_moves = 30);
}

#endif