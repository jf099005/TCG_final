#ifndef __BOARD_EVALUATOR__
#define __BOARD_EVALUATOR__

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include"board_analyzer.h"


typedef double Score;

namespace CDCEvaluate{
    const double score_mx = 1000000;
    
    Score distance_score(const Position& pos, Color side, Color opponent, PieceType opponent_type);

    Score calculate_score(const Position& pos, int remain_moves = 30);

    // int calculate_chance(const Position& pos, int remain_moves = 30);
    std::pair<int,int> calculate_risk(const Position& pos, Color side, int remain_moves = 30);

}

#endif