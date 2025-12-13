#ifndef __ALPHABETA__
#define __ALPHABETA__ 1

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include<algorithm>
#include"EvaluationFunction/H/evaluator.h"

const int score_mx = 1000;

class MoveOrderer{
    public:
        MoveOrderer(){};
        void ordering_move(MoveList<> & moves);
};

class ACDC{
    public:
        ACDC(){
            // solver_color = color;
            // depth_limit = depth;
            for(int piecetype = General; piecetype<=Soldier; piecetype++){
                int pieceNumber = piecetype==General?1:
                                        piecetype == Soldier? 5:2;
                remain_hidden_pieces[Red][piecetype] = pieceNumber;
                remain_hidden_pieces[Black][piecetype] = pieceNumber;
            }
            // std::fill(remain_hidden_pieces, remain_pieces + 2*8, 0);
        }
        
        double Negamax(Position pos, int depth, int remain_moves, int alpha = -score_mx, int beta = score_mx);
        double Move_Evaluate(Position pos, Move move, int depth, int remain_moves, int alpha = -score_mx, int beta = score_mx);
        
        Move opt_solution(Position pos, int depth, int remain_moves);

        unsigned short remain_hidden_pieces[2][8];

        int visited_states;
        void reset(){
            visited_states = 0;
        }
};

#endif