#include"evaluator.h"

Score CDCEvaluate::distance_score(const Position& pos, Color side, Color opponent){
    
    Score total_score = 0;
    
    for(Square sq_opponent: BoardView(pos.pieces(opponent))){
        int capture_distance = 0;
        int capture_pieces = 0;
        int tie_pieces = 0;
        int tie_distance = 0;
        PieceType opponent_tp = pos.peek_piece_at(sq_opponent).type;
        if(opponent_tp == Soldier)
            continue;

        for(Square sq_side: BoardView(pos.pieces(side))){
            PieceType side_tp = pos.peek_piece_at(sq_side).type;
            bool capture_state = side_tp > opponent_tp and (side_tp != Cannon)\
                                        and (opponent_tp != Cannon);

            bool tie_state = (side_tp == opponent_tp and capture_state);
            capture_state = capture_state & !tie_state;

            int pieces_dis = distance(sq_opponent, sq_side);
            pieces_dis = pieces_dis >= 3? (pieces_dis << 1):pieces_dis;

            capture_distance += (pieces_dis*capture_state);
            capture_pieces += capture_state;

            tie_distance += (distance(sq_opponent, sq_side)*tie_state);
            tie_pieces += tie_state;
        }
        
        bool strong_capture = (capture_pieces >= 2) or (capture_pieces >=1 and tie_pieces >= 1); 
        bool can_capture = tie_pieces >= 2 or strong_capture;
        bool can_threat = (capture_pieces > 0) or can_capture;

        bool essential = (can_capture or Piece_Value[opponent_tp] >= Piece_Value[Elephant]);
        int dist = essential ? capture_distance : (capture_distance >> 7);
        Score score = (strong_capture + can_capture + can_threat)*(dist);
        total_score += score;
    }

    return total_score;
}


Score CDCEvaluate::calculate_score(const Position& pos, int remain_moves){
    Color opponent = pos.due_up() == Red? Black:Red;
    // return pieces_score(pos, pos.pieces(pos.due_up()))\
    //          - pieces_score(pos, pos.pieces(opponent));



    int piece_score = pieces_score(pos, pos.pieces(pos.due_up()))\
            - pieces_score(pos, pos.pieces(opponent));
    int dis_score = distance_score(pos, pos.due_up(), opponent);

    // return piece_score;

    double piece_weight = 10;
    double dis_weight = 0.01;
    return piece_weight*piece_score - dis_weight*dis_score;
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