#ifndef __BOARD_EVALUATOR__
#define __BOARD_EVALUATOR__

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include"board_analyzer.h"


typedef float Score;

inline int evaluate_square(const Position& pos, Square sq, Piece piece, int basic_mobility){
    int mobility = basic_mobility;
    int attack_val = 0;
    bool threaten = false;
    Color opponent = Opponent[piece.side];
    for(int adj_dir = 0; adj_dir < num_Adjacent[sq] and !threaten; adj_dir++){
        Square sq_nx = Adjacent[sq][adj_dir];
        
        assert(is_okay(sq_nx));

        Piece nx_piece = pos.peek_piece_at(sq_nx);

        if(nx_piece.side != opponent && nx_piece.side != NO_COLOR )
            continue;

        bool can_move = (
                            nx_piece.type == NO_PIECE ||
                            (
                                piece.type > nx_piece.type && 
                                nx_piece.type != piece.type
                            )
                        );

        mobility += can_move;
        threaten |= nx_piece.type != Cannon && nx_piece.type != NO_PIECE && nx_piece.type > piece.type;
        bool can_attack = piece.type > nx_piece.type && nx_piece.type != NO_PIECE && !threaten;
        
        attack_val += can_attack?Piece_Value[nx_piece.type]:0;
    }

    int control_val = 0;
    for(int diag_dir = 0; diag_dir < num_Diagonal[sq] and !threaten; diag_dir++){
        Square sq_diag = Diagonal[sq][diag_dir];

        assert(is_okay(sq_diag));
        
        Piece diag_piece = pos.peek_piece_at(sq_diag);

        if(diag_piece.side != opponent && diag_piece.side != NO_COLOR)
            continue;
        bool can_control = piece.type > diag_piece.type && diag_piece.type != NO_PIECE && !threaten;
        
        control_val += can_control?Piece_Value[diag_piece.type]:0;
    }
    // <= 2*4 + 4*ATK_MAX + 4*ATK_VAL*2 = 250+8 = 258
    return (threaten? DANGER : mobility*2 + attack_val + control_val*2);
}

namespace CDCEvaluate{
    const double score_mx = 3000;


    const int full_distance_score_per_piece[7] = {
        20*5+10,//General
        20+10*2, //Advisor
        20*3+10*2, //Elephant
        20*5+10*2, //Chariot
        20*7+10*2, //Horse
        20*9, //Cannon
        20*9+10*5 //Soldier
    };

    const int full_distance_score[7] = {
        (20*5+10)*1,//General, 110
        (20+10*2)*2, //Advisor, 80
        (20*3+10*2)*2, //Elephant, 160
        (20*5+10*2)*2, //Chariot, 
        (20*7+10*2)*2, //Horse
        20*9*2, //Cannon
        (20*9+10*5)*5 //Soldier
    };

    Score calculate_score(const Position& pos, int remain_moves, unsigned short remain_hidden_pieces[2][8], int remain_hidden_pieces_num);

    Score hidden_pieces_score(const Position& pos, int remain_moves, unsigned short remain_hidden_pieces[2][8]);

    Score distance_score(const Position& pos, Color side, Color opponent, PieceType opponent_type, unsigned short remain_hidden_pieces[2][8]);


    Score pawn_evaluate(const Position& pos, Color side, unsigned short remain_hidden_pieces[2][8]);

    Score cannon_evaluate(const Position& pos, Color side, unsigned short remain_hidden_pieces[2][8]);

}

#endif