#ifndef __ALPHABETA__
#define __ALPHABETA__ 1

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"

#include"EvaluationFunction/H/evaluator.h"

const int score_mx = 1000;

class MoveOrderer{
    public:
        MoveOrderer(){};
        void ordering_move(MoveList<> & moves);
};

class ACDC{
    public:
        Move opt_solution;
        ACDC(Color color, int depth){
            solver_color = color;
            depth_limit = depth;
        }
        
        int Negamax(Position pos, int depth, int remain_moves, int alpha = -score_mx, int beta = score_mx);
        int depth_limit;
        Color solver_color;
};

#endif