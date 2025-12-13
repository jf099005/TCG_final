#ifndef __BOARD_EVALUATOR__
#define __BOARD_EVALUATOR__

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include"BoardAnalyze/H/board_analyzer.h"


typedef int Score;

namespace CDCEvaluate{
    const double score_mx = 1000;
    Score calculate_score(Position pos, int remain_moves = 30);
}

#endif