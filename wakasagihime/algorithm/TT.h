#ifndef TT_H
#define TT_H

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"

const int piece_type_numbers = 7;


struct TT_info{
    double score;
    short depth;
    Move opt_move;

    TT_info(){
        score = 114514;
        depth = -1;
    };

    TT_info(double s, int d, Move nx){
        score = s;
        depth = d;
        opt_move = nx;
    };
};


class CDCTranspositionTable{
    public:

        CDCTranspositionTable(int hash_bits = 32);
        ~CDCTranspositionTable();

        TT_info* query(const Position& pos, int depth);
        TT_info* query_for_flipping(const Position& pos, int depth, Square flip_sq);

        void write(const Position& pos, int depth, double score, Move opt_move);
        void write(TT_info* info_ptr, int depth, double score, Move opt_move);
        void write(TT_info* info_ptr, int depth, Square flip_sq, double score, Move opt_move);

        //private:

        long long hash_pos(const Position& pos);

        long long piece_hash[SIDE_NB][piece_type_numbers][SQUARE_NB];
        long long hidden_piece_hash[SQUARE_NB];
        long long flipping_sq_hash[SQUARE_NB];
        long long SideToMove_hash[SIDE_NB];




        TT_info *Record;


        long long random_value_max;

        const double notfound = 114514;
};

#endif