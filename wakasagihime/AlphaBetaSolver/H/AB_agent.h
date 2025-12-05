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
        Move opt_solution;
        ACDC(Color color, int depth){
            solver_color = color;
            depth_limit = depth;
            for(int piecetype = General; piecetype<=Soldier; piecetype++){
                int pieceNumber = piecetype==General?1:
                                        piecetype == Soldier? 5:2;
                remain_hidden_pieces[Red][piecetype] = pieceNumber;
                remain_hidden_pieces[Black][piecetype] = pieceNumber;
            }
            // std::fill(remain_hidden_pieces, remain_pieces + 2*8, 0);
        }
        
        int Negamax(Position pos, int depth, int remain_moves, int alpha = -score_mx, int beta = score_mx);
        int Move_Evaluate(Position pos, Move move, int depth, int remain_moves, int alpha = -score_mx, int beta = score_mx);
        
        int depth_limit;
        unsigned short remain_hidden_pieces[2][8];
        Color solver_color;
};

#endif