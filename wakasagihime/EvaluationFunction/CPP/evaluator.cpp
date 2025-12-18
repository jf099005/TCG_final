#include"evaluator.h"
Score CDCEvaluate::calculate_score(const Position& pos, int remain_moves){
    Color opponent = pos.due_up() == Red? Black:Red;
    // return pieces_score(pos, pos.pieces(pos.due_up()))\
    //          - pieces_score(pos, pos.pieces(opponent));

    return exp_pieces_score(pos, pos.pieces(pos.due_up()))\
            - exp_pieces_score(pos, pos.pieces(opponent));

}


std::pair<int,int> CDCEvaluate::calculate_risk(const Position& pos, Color side, int remain_moves){
    int regain = 0, risk = 0;
    const static Direction dir[4] = {EAST, WEST, SOUTH, NORTH};
    while(true){
        for(Square sq: BoardView(pos.pieces(side))){
            Piece piece = pos.peek_piece_at(sq);
            
            Piece capture_piece = piece;
            for(int j=0; j<4; j++){
                Square adjacent = sq + dir[j];
                if(adjacent/8 != sq/8 and adjacent%8 != sq%8)
                    continue;
                
                Piece piece_adj = pos.peek_piece_at(adjacent);

                if(piece_adj.side != piece.side){
                    if(piece_adj.type > piece.type and piece_adj.type != Cannon){
                        if(capture_piece.side == piece.side or piece_adj.type > capture_piece.type)
                            capture_piece = piece_adj;
                        // break;
                    }
                }
            }
        }

        }

    return {regain, risk};
}