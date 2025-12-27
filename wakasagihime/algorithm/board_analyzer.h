#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include<map>
#ifndef BOARD_ANALYZER_H
#define BOARD_ANALYZER_H
//tools for analyze board


inline constexpr int InitialPiecesNumber[8] = {
    1, 2, 2, 2, 2, 2, 5
};

inline constexpr Direction AllMoveDirections[] = {
    NORTH, EAST, SOUTH, WEST,
    // NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST
};

inline constexpr Direction AllDiagonalDirections[] = {
    // NORTH, EAST, SOUTH, WEST,
    NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST
};

inline Square Adjacent[32][4] = {
    { SQ_A2, SQ_B1, SQ_NONE, SQ_NONE,  },
    { SQ_B2, SQ_C1, SQ_A1, SQ_NONE,  },
    { SQ_C2, SQ_D1, SQ_B1, SQ_NONE,  },
    { SQ_D2, SQ_E1, SQ_C1, SQ_NONE,  },
    { SQ_E2, SQ_F1, SQ_D1, SQ_NONE,  },
    { SQ_F2, SQ_G1, SQ_E1, SQ_NONE,  },
    { SQ_G2, SQ_H1, SQ_F1, SQ_NONE,  },
    { SQ_H2, SQ_G1, SQ_NONE, SQ_NONE,  },
    { SQ_A3, SQ_B2, SQ_A1, SQ_NONE,  },
    { SQ_B3, SQ_C2, SQ_B1, SQ_A2,  },
    { SQ_C3, SQ_D2, SQ_C1, SQ_B2,  },
    { SQ_D3, SQ_E2, SQ_D1, SQ_C2,  },
    { SQ_E3, SQ_F2, SQ_E1, SQ_D2,  },
    { SQ_F3, SQ_G2, SQ_F1, SQ_E2,  },
    { SQ_G3, SQ_H2, SQ_G1, SQ_F2,  },
    { SQ_H3, SQ_H1, SQ_G2, SQ_NONE,  },
    { SQ_A4, SQ_B3, SQ_A2, SQ_NONE,  },
    { SQ_B4, SQ_C3, SQ_B2, SQ_A3,  },
    { SQ_C4, SQ_D3, SQ_C2, SQ_B3,  },
    { SQ_D4, SQ_E3, SQ_D2, SQ_C3,  },
    { SQ_E4, SQ_F3, SQ_E2, SQ_D3,  },
    { SQ_F4, SQ_G3, SQ_F2, SQ_E3,  },
    { SQ_G4, SQ_H3, SQ_G2, SQ_F3,  },
    { SQ_H4, SQ_H2, SQ_G3, SQ_NONE,  },
    { SQ_B4, SQ_A3, SQ_NONE, SQ_NONE,  },
    { SQ_C4, SQ_B3, SQ_A4, SQ_NONE,  },
    { SQ_D4, SQ_C3, SQ_B4, SQ_NONE,  },
    { SQ_E4, SQ_D3, SQ_C4, SQ_NONE,  },
    { SQ_F4, SQ_E3, SQ_D4, SQ_NONE,  },
    { SQ_G4, SQ_F3, SQ_E4, SQ_NONE,  },
    { SQ_H4, SQ_G3, SQ_F4, SQ_NONE,  },
    { SQ_H3, SQ_G4, SQ_NONE, SQ_NONE,  },
};
inline int num_Adjacent[32] = {2, 3, 3, 3, 3, 3, 3, 2, 3, 4, 4, 4, 4, 4, 4, 3, 3, 4, 4, 4, 4, 4, 4, 3, 2, 3, 3, 3, 3, 3, 3, 2};

inline Square Diagonal[32][4] = {
    { SQ_B2, SQ_NONE, SQ_NONE, SQ_NONE,  },
    { SQ_C2, SQ_A2, SQ_NONE, SQ_NONE,  },
    { SQ_D2, SQ_B2, SQ_NONE, SQ_NONE,  },
    { SQ_E2, SQ_C2, SQ_NONE, SQ_NONE,  },
    { SQ_F2, SQ_D2, SQ_NONE, SQ_NONE,  },
    { SQ_G2, SQ_E2, SQ_NONE, SQ_NONE,  },
    { SQ_H2, SQ_F2, SQ_NONE, SQ_NONE,  },
    { SQ_G2, SQ_NONE, SQ_NONE, SQ_NONE,  },
    { SQ_B3, SQ_B1, SQ_NONE, SQ_NONE,  },
    { SQ_C3, SQ_C1, SQ_A1, SQ_A3,  },
    { SQ_D3, SQ_D1, SQ_B1, SQ_B3,  },
    { SQ_E3, SQ_E1, SQ_C1, SQ_C3,  },
    { SQ_F3, SQ_F1, SQ_D1, SQ_D3,  },
    { SQ_G3, SQ_G1, SQ_E1, SQ_E3,  },
    { SQ_H3, SQ_H1, SQ_F1, SQ_F3,  },
    { SQ_G1, SQ_G3, SQ_NONE, SQ_NONE,  },
    { SQ_B4, SQ_B2, SQ_NONE, SQ_NONE,  },
    { SQ_C4, SQ_C2, SQ_A2, SQ_A4,  },
    { SQ_D4, SQ_D2, SQ_B2, SQ_B4,  },
    { SQ_E4, SQ_E2, SQ_C2, SQ_C4,  },
    { SQ_F4, SQ_F2, SQ_D2, SQ_D4,  },
    { SQ_G4, SQ_G2, SQ_E2, SQ_E4,  },
    { SQ_H4, SQ_H2, SQ_F2, SQ_F4,  },
    { SQ_G2, SQ_G4, SQ_NONE, SQ_NONE,  },
    { SQ_B3, SQ_NONE, SQ_NONE, SQ_NONE,  },
    { SQ_C3, SQ_A3, SQ_NONE, SQ_NONE,  },
    { SQ_D3, SQ_B3, SQ_NONE, SQ_NONE,  },
    { SQ_E3, SQ_C3, SQ_NONE, SQ_NONE,  },
    { SQ_F3, SQ_D3, SQ_NONE, SQ_NONE,  },
    { SQ_G3, SQ_E3, SQ_NONE, SQ_NONE,  },
    { SQ_H3, SQ_F3, SQ_NONE, SQ_NONE,  },
    { SQ_G3, SQ_NONE, SQ_NONE, SQ_NONE,  }
};

inline int num_Diagonal[32] = {1, 2, 2, 2, 2, 2, 2, 1, 2, 4, 4, 4, 4, 4, 4, 2, 2, 4, 4, 4, 4, 4, 4, 2, 1, 2, 2, 2, 2, 2, 2, 1};


// void init_Adjacent_table();

const int Piece_Value_Max = 25;

// inline std::map<PieceType, int> Piece_Value = {
//     {General, 20},
//     {Advisor, 25},
//     {Elephant, 18},
//     {Chariot, 5},
//     {Horse, 3},
//     {Cannon, 18},
//     {Soldier, 1}
// };


inline std::string piece_name[7] = {
    "General",
    "Advisor",
    "Elephant",
    "Chariot",
    "Horse",
    "Cannon",
    "Soldier"
};

inline int Piece_Value[7] = {
    20, 25, 18, 5, 3, 18, 1
};

inline Color Opponent[] = {Red, Black};

inline bool is_dangerous(const Position& pos, Square piece_sq, Piece piece){
    bool danger = false;

    // Piece piece = pos.peek_piece_at(piece_sq);

    int opponent = Opponent[piece.side];
    assert(opponent == 0 || opponent == 1);

    for(int idx_sq = 0; !danger && idx_sq < num_Adjacent[piece_sq]; idx_sq++){
        Square sq_adj = Adjacent[piece_sq][idx_sq];
        Piece adj = pos.peek_piece_at(sq_adj);

        danger |= (adj.side == opponent && adj.type != Hidden && adj.type != NO_PIECE && adj.type != Cannon && adj.type > piece.type);
    }
    return danger;
}


inline bool is_escape_move(const Position& pos, Move mv){
    Piece mv_piece = pos.peek_piece_at(mv.from());
    return is_dangerous(pos, mv.from(), mv_piece) && !is_dangerous(pos, mv.to(), mv_piece);
}


const int DANGER = -114514;
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

        // bool can_move = (
        //                     diag_piece.type == NO_PIECE ||
        //                     (
        //                         piece.type > diag_piece.type
        //                     )
        //                 );

        // mobility += can_move;
        // threaten |= diag_piece.type > piece.type && nx_piece.type != Cannon;
        bool can_control = piece.type > diag_piece.type && diag_piece.type != NO_PIECE && !threaten;
        
        control_val += can_control?Piece_Value[diag_piece.type]:0;
    }
    return (threaten? DANGER : mobility*2 + attack_val + control_val*2);
}

inline constexpr short AllMoveDirections_size = 4;


Color is_endgame(Position pos);

int pieces_score(const Position &pos, Board pieces_location);
// int exp_pieces_score(const Position &pos, Board pieces_location);

#endif