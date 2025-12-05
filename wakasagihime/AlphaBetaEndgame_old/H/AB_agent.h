#include"node.h"
#include"endgame_evaluator.h"
#ifndef __ALPHABETA__
#define __ALPHABETA__ 1

const int score_mx = 1e9;

class AlphaBetaEndgameSolver{
    public:
        Move opt_solution;
        AlphaBetaEndgameSolver(Color color, int depth){
            solver_color = color;
            depth_limit = depth;
        }
        
        int Negamax(Position pos, int depth, int remain_moves, int alpha = -score_mx, int beta = score_mx);
        int depth_limit;
        Color solver_color;
};

#endif