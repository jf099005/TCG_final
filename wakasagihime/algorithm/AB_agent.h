#ifndef AB_H
#define AB_H

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include<algorithm>
#include<chrono>
#include"evaluator.h"
#include"TT.h"

class MoveOrderer{
    public:
        MoveOrderer()
        {};
        //return the number of valid moves, and the movelist is sorted
        int ordering_move(const Position& pos, MoveList<> & moves, bool only_critical_move, bool skip_flipping);
        int evaluate_move(const Position& pos, Move move);
        bool is_critical_move(const Position& pos, Move mv);

    //private:
};

const int MAX_DEPTH = 100;
const int TT_bits = 28;



inline bool cannon_capture(const Position& pos){
    MoveList<> nx_moves(pos, Cannon);
    bool capture_occur = false;
    for(int mv_idx = 0; mv_idx < nx_moves.size() && !capture_occur; mv_idx++){
        capture_occur |= pos.peek_piece_at(nx_moves[mv_idx].to()).side != NO_COLOR;
    }
    return capture_occur;
}

inline bool is_unstable(const Position& pos){
    bool capture_occur = cannon_capture(pos);
    // debug << "cannon capture:" << capture_occur;
    // debug << "color:" << pos.due_up()<<" , opponent: " << opponent <<'\n';
    for(Square sq: BoardView(pos.pieces(FACE_UP))){
        if(
            (pos.peek_piece_at(sq).side == Red || pos.peek_piece_at(sq).side == Black) &&
            pos.peek_piece_at(sq).type != Cannon
        ){    
            Color opponent = Opponent[pos.peek_piece_at(sq).side];
            for(int adj_idx = 0; adj_idx < num_Adjacent[sq]; adj_idx++){
                Square adj = Adjacent[sq][adj_idx];
                capture_occur |= ( pos.peek_piece_at(adj).side == opponent) &&
                    (pos.peek_piece_at(sq).type > pos.peek_piece_at(adj).type);
                // if(capture_occur){
                //     debug << "captured at sq of " << sq << ", captured by " << adj <<'\n';
                //     debug << '\t' << ( pos.peek_piece_at(adj).side) << " , " << ( opponent) <<'\n';
                //     debug << '\t' << ( pos.peek_piece_at(adj).side == opponent) <<'\n';
                // }
            }
        }

    }

    return capture_occur;
}

class ACDC{
    public:

        const int lim_extend_depth = -30;
        ACDC(){
            // solver_color = color;
            // depth_limit = depth;

            #ifdef TT_H
            TT = new CDCTranspositionTable(TT_bits);
            debug << "maximum for TT:" << (1ll<<TT_bits) << '\n';
            #endif

            orderer = new MoveOrderer;

            history = new Move[MAX_DEPTH];

            for(int piecetype = General; piecetype<=Soldier; piecetype++){
                int pieceNumber = piecetype==General?1:
                                        piecetype == Soldier? 5:2;
                remain_hidden_pieces[Red][piecetype] = pieceNumber;
                remain_hidden_pieces[Black][piecetype] = pieceNumber;
            }
            // std::fill(remain_hidden_pieces, remain_pieces + 2*8, 0);
        }
        
        double Negamax(Position pos, int depth, int remain_moves,\
                    double alpha, double beta, Move prv, Square sq_danger = SQ_NONE);

        double Move_Evaluate(Position pos, Move move, int depth, int remain_moves,\
                    double alpha = -CDCEvaluate::score_mx, double beta = CDCEvaluate::score_mx);
        
        Move opt_solution_with_fixed_depth(Position pos, int depth, int remain_moves);


        double start_time;
        std::chrono::steady_clock::time_point deadline;
        
        Move opt_solution(Position pos, double given_time, int remain_moves);

        unsigned short remain_hidden_pieces[2][8];

        int visited_states, visited_critical_states;
        int correct_prediction, fail_prediction;
        void reset(){
            visited_states = 0;
            visited_critical_states = 0;
            correct_prediction = 0;
            fail_prediction = 0;
        }

        #ifdef TT_H
        CDCTranspositionTable *TT;
        void trace_PV(Position pos, int depth);
        #endif
        MoveOrderer *orderer;

        Move* history;
};

#endif