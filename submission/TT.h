#ifndef TT_H
#define TT_H

#include "evaluator.h"

const int piece_type_numbers = 7;

const int8_t DEFAULT_DEPTH = -114;

struct TT_info{
    Score score;
    int8_t depth;
    bool is_exact_value;
    Move opt_move;
    //location of all hidden pieces
    Board hidden_pieces_location;

    TT_info(){
        score = 0;
        depth = DEFAULT_DEPTH;
        is_exact_value = false;
        hidden_pieces_location = 0;
    };

    TT_info(const Position& pos, double s, int d, Move nx, bool flag){
        score = s;
        depth = d;
        opt_move = nx;
        is_exact_value = flag;
    };
};


class CDCTranspositionTable{
    public:
        int num_query, num_success_query;
        void reset(){
            num_query = 0;
            num_success_query = 0;
        }

        CDCTranspositionTable(int hash_bits);
        ~CDCTranspositionTable();

        TT_info* query(const Position& pos, int depth);
        TT_info* query_for_flipping(const Position& pos, int depth, Square flip_sq);

        void write(const Position& pos, int depth, double score, Move opt_move, bool is_exact_value);
        
        void write_of_flipping(const Position& pos, int depth, Square flip_sq, double score, Move opt_move, bool is_exact_value);
        // void write(TT_info* info_ptr, const Position& pos, int depth, double score, Move opt_move, bool is_exact);
        // void write(TT_info* info_ptr, int depth, Square flip_sq, double score, Move opt_move, bool is_exact);

        //private:

        long long hash_pos(const Position& pos);

        long long piece_hash[SIDE_NB][piece_type_numbers][SQUARE_NB];
        long long hidden_piece_hash[SQUARE_NB];
        long long flipping_sq_hash[SQUARE_NB];
        long long SideToMove_hash[SIDE_NB];

        TT_info *Record;


        long long random_value_max;
};

#endif