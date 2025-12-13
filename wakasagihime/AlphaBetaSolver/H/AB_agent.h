#ifndef AB_H
#define AB_H

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include<algorithm>
#include"evaluator.h"
#include"TT.h"

class MoveOrderer{
    public:
        MoveOrderer()
        {};
        void ordering_move(const Position& pos, MoveList<> & moves);
        short evaluate_move(const Position& pos, Move move);

    //private:
};

class ACDC{
    public:
        ACDC(){
            // solver_color = color;
            // depth_limit = depth;

            #ifdef TT_H
            TT = new CDCTranspositionTable(28);
            #endif

            orderer = new MoveOrderer;

            for(int piecetype = General; piecetype<=Soldier; piecetype++){
                int pieceNumber = piecetype==General?1:
                                        piecetype == Soldier? 5:2;
                remain_hidden_pieces[Red][piecetype] = pieceNumber;
                remain_hidden_pieces[Black][piecetype] = pieceNumber;
            }
            // std::fill(remain_hidden_pieces, remain_pieces + 2*8, 0);
        }
        
        double Negamax(Position pos, int depth, int remain_moves,\
                    double alpha = -CDCEvaluate::score_mx, double beta = CDCEvaluate::score_mx);

        double Move_Evaluate(Position pos, Move move, int depth, int remain_moves,\
                    double alpha = -CDCEvaluate::score_mx, double beta = CDCEvaluate::score_mx);
        
        Move opt_solution(Position pos, int depth, int remain_moves);

        unsigned short remain_hidden_pieces[2][8];

        int visited_states;
        void reset(){
            visited_states = 0;
        }

        #ifdef TT_H
        CDCTranspositionTable *TT;
        #endif
        MoveOrderer *orderer;
};

#endif