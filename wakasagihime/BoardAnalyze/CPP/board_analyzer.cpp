#include"../H/board_analyzer.h"

// Color is_endgame(Position pos){
//     Board red_pieces = pos.pieces(Red);
//     Board black_pieces = pos.pieces(Black);
//     PieceType mx_r = Soldier, mx_b = Soldier;

//     bool red_king_exist = false,\
//             red_cannon_exist = false,\
//                 red_pawn_exist = false;
//     bool black_king_exist = false,\
//             black_cannon_exist = false,\
//                 black_pawn_exist = false;
//     for(Square sq_r: BoardView(red_pieces)){
//         PieceType type_r = pos.peek_piece_at(sq_r).type;
//         if(type_r == General){
//             red_king_exist = true;
//         }
//         else if(type_r == Cannon){
//             red_cannon_exist = true;
//         }
//         else if(type_r == Soldier){
//             red_pawn_exist = true;
//         }
//         else{
//             if( type_r > mx_r )
//                 mx_r = type_r;
//         }
//     }

//     for(Square sq_b: BoardView(black_pieces)){
//         PieceType type_b = pos.peek_piece_at(sq_b).type;
//         if(type_b == General){
//             black_king_exist = true;
//         }
//         else if(type_b == Cannon){
//             black_cannon_exist = true;
//         }
//         else if(type_b == Soldier){
//             black_pawn_exist = true;
//         }
//         else{
//             if( type_b > mx_b )
//                 mx_b = type_b;
//         }
//     }

//     if(red_king_exist and\
//         (not black_cannon_exist) and\
//                 (not black_pawn_exist)){
//         return Red;
//     }




//     return NO_COLOR;
// }

Color is_endgame(Position pos){
    // bool red_win = true;
    // bool black_win = true;
    Board red_pieces = pos.pieces(Red);
    Board black_pieces = pos.pieces(Black);

    int num_black = pos.count(Black, ALL_PIECES);
    int num_red = pos.count(Red, ALL_PIECES);

    // debug << "num black: " << num_black <<"\n";
    // debug << "num red: " << num_red << "\n";

    bool red_win;
    for(Square sq_r : BoardView(red_pieces)){
        PieceType type_r = pos.peek_piece_at(sq_r).type;
        // debug << "red piece " << type_r << '\n';
        if(type_r == Cannon or type_r == Soldier){
            // debug << "\tcontinue\n";
            continue;
        }
        red_win = true;
        for(Square sq_b: BoardView(black_pieces)){
            PieceType type_b = pos.peek_piece_at(sq_b).type;
            red_win &= (type_r > type_b and (type_r != Cannon and num_red >= 3));
            // if(!red_win)debug << "red " << type_r << "cannot capture black " << type_b << "\n";
            red_win &= !(type_b > type_r and\
                    (type_b != Cannon or num_black >= 3));
        }
        if(red_win)
            return Red;
    }

    bool black_win;
    for(Square sq_b: BoardView(black_pieces)){
        PieceType type_b = pos.peek_piece_at(sq_b).type;
        if(type_b == Cannon or type_b == Soldier)
            continue;

        black_win = true;
        for(Square sq_r : BoardView(red_pieces)){
            PieceType type_r = pos.peek_piece_at(sq_r).type;
            
            black_win &= !(type_r > type_b and\
                    (type_r != Cannon or num_red >= 3));

            black_win &= (type_b > type_r and\
                    (type_b != Cannon or num_black >= 3));
        }
        if(black_win)
            return Black;
    }
    return NO_COLOR;
}

std::map<PieceType, int> Piece_Value = {
    {General, 7},
    {Advisor, 8},
    {Elephant, 6},
    {Chariot, 5},
    {Horse, 3},
    {Cannon, 9},
    {Soldier, 1}
};

int pieces_score(const Position &pos, Board pieces_location){
    int score = 0;
    // Board pieces_location = pos.pieces(side);
    for(Square sq: BoardView(pieces_location)){
        score += Piece_Value[ pos.peek_piece_at(sq).type ];
    }
    return score;
}
