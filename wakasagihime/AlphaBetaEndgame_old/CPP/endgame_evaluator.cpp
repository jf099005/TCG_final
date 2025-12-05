#include"endgame_evaluator.h"
#include"board_analyzer.h"

int endgame_evaluator::evaluate(Position pos, Color solver_side){
    static const int C = 1000;
    Color opponent = (solver_side == Black? Red:Black);
    Board player_pieces = pos.pieces(solver_side);
    Board opponent_pieces = pos.pieces(opponent);

    int total_piece_distance = 0;
    int total_piece_score = pieces_score(pos, player_pieces) - pieces_score(pos, opponent_pieces);

    for(Square sq_p: BoardView(player_pieces)){
        for(Square sq_o: BoardView(opponent_pieces)){
            PieceType player_type = pos.peek_piece_at(sq_p).type;
            PieceType opponent_type = pos.peek_piece_at(sq_o).type;
            if(player_type > opponent_type and 
                                    !(opponent_type > player_type)){
                int piece_dis = distance<Square>(sq_p, sq_o);
                total_piece_distance += piece_dis;
            }
        }
    }

    // assert(total_piece_score > 0);
    
    int score = C*total_piece_score - total_piece_distance;
    if(pos.due_up() != solver_side)
        score = -score;
    return score;
}