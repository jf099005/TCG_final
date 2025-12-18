#ifndef __BOARD_EVALUATOR__
#define __BOARD_EVALUATOR__

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include"BoardAnalyze/H/board_analyzer.h"


typedef int Score;

namespace CDCEvaluate{
    const double score_mx = 100000;
    Score calculate_score(const Position& pos, int remain_moves = 30);

    // int calculate_chance(const Position& pos, int remain_moves = 30);
    std::pair<int,int> calculate_risk(const Position& pos, Color side, int remain_moves = 30);

}

#endif