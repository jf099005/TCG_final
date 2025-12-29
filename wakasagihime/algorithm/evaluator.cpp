#include"evaluator.h"

Score CDCEvaluate::distance_score(const Position& pos, Color side, Color opponent, PieceType opponent_tp){
    
    // const int full_score_per_piece[7] = {
    //     20*6+10,//General
    //     20+10*2, //Advisor
    //     20*3+10*2, //Elephant
    //     20*5+10*2, //Chariot
    //     20*7+10*2, //Horse
    //     20*9, //Cannon
    //     20*9+10*5 //Soldier
    // };

    Score total_score = 0;

    assert(opponent_tp != ALL_PIECES);
    
    total_score += full_distance_score_per_piece[opponent_tp]*\
                        (InitialPiecesNumber[opponent_tp] - pos.count(opponent, opponent_tp));

    for(Square sq_opponent: BoardView(pos.pieces(opponent, opponent_tp))){
        int distance_score = 0;
        int capture_pieces = 0;
        int tie_pieces = 0;
        if(opponent_tp == Soldier)
            continue;

        for(int side_tp_int = 0; side_tp_int<= Soldier; side_tp_int++){
            PieceType side_tp = static_cast<PieceType>(side_tp_int);
            if(!(side_tp > opponent_tp) or side_tp == Cannon){
                continue;
            }

            int side_piece_cnt = pos.count(side, side_tp);
            capture_pieces += (side_tp != opponent_tp? side_piece_cnt:0);
            tie_pieces += (side_tp == opponent_tp? pos.count(side, side_tp):0);

            int side_score = 0;
            for(Square sq_side: BoardView(pos.pieces(side, side_tp))){
                int pieces_dis = distance(sq_opponent, sq_side);
                pieces_dis = pieces_dis > 3? (pieces_dis << 1):pieces_dis;

                int pair_score = (20 - pieces_dis);

                side_score += pair_score;
            }

            side_score = side_score >> (side_tp == opponent_tp);
            distance_score += side_score;
        }
        int panelty = (capture_pieces < 2 && capture_pieces + tie_pieces <=2) +
                (capture_pieces == 1 && tie_pieces == 0) + (capture_pieces == 0 && tie_pieces <= 2);

        distance_score = distance_score >> panelty;

        total_score += distance_score;
    }

    return total_score;
}


Score CDCEvaluate::calculate_score(const Position& pos, int remain_moves, unsigned short remain_hidden_pieces[2][8]){
    Color opponent = Opponent[pos.due_up()];

    int piece_score = pieces_score(pos, pos.pieces(pos.due_up()))\
            - pieces_score(pos, pos.pieces(opponent));

    // int General_dis_score = distance_score(pos, pos.due_up(), opponent, General);
    // int Advisor_dis_score = distance_score(pos, pos.due_up(), opponent, Advisor);

    //dis score for each stone: 
    double dis_score = 0;
    int side_dis_score = 0;
    int opponent_dis_score = 0;
    for(int piecetype = General; piecetype <= Soldier; piecetype = piecetype + 1){
        int side_score = distance_score(pos, pos.due_up(), opponent, static_cast<PieceType>(piecetype));
        int opponent_score = distance_score(pos, opponent, pos.due_up(), static_cast<PieceType>(piecetype));
        bool essential_pieces = (piecetype == General || piecetype == Advisor || piecetype == Elephant || piecetype == Cannon);
        // if(side_score < 0 || opponent_score < 0){
        //     debug << "piece:" << piece_name[piecetype] <<"\n";
        //     debug << "side score:" << side_score <<", opponent:" << opponent_score << '\n';
        //     debug << pos;
        // }
        // assert(side_score >= 0);
        // assert(opponent_score >= 0);
        side_score = side_score << (essential_pieces*2);
        opponent_score = opponent_score << (essential_pieces*2);
        // side_dis_score += side_score;
        // opponent_dis_score += opponent_score;
        // dis_score += (side_score - opponent_score);
        // debug << "\npiecetype " << piece_name[piecetype] <<":\n";
        // debug << "\tpositive dis score: " << side_score <<", negative:" << opponent_score <<'\n';
        // debug << "\tnormalized positive dis score: " << side_score/Piece_Value[piecetype]\
        //      <<", negative:" << opponent_score/Piece_Value[piecetype] <<'\n';

        dis_score += (Piece_Value[piecetype])*double(side_score - opponent_score) / full_distance_score[piecetype];
    }

    double piece_weight = 10;
    double dis_weight = 1;

    Score pawn_score = pawn_evaluate(pos, pos.due_up(), remain_hidden_pieces);
    pawn_score -= pawn_evaluate(pos, opponent, remain_hidden_pieces);

    // debug << "piece score: " << piece_score << ", distance score:" << dis_score << '\n';
    // return piece_weight*piece_score + dis_weight*dis_score;
    return piece_weight*piece_score + dis_weight*dis_score + pawn_score;
}

Score CDCEvaluate::pawn_evaluate(const Position& pos, Color side, unsigned short remain_hidden_pieces[2][8]){
    Color opponent = Opponent[side];
    bool opponent_General_exist = pos.count(opponent, General) || remain_hidden_pieces[opponent][General];
    int safe_pawns = 0;
    int danger_pawns = 0;
    int free_pawns = 0;
    bool attack_opponent_General = false;

    for(Square sq: BoardView( pos.pieces(side, Soldier)) ){
        bool is_safe = true;
        for(int adj_idx=0; adj_idx < num_Adjacent[sq] && is_safe; adj_idx++){
            Piece adj_piece = pos.peek_piece_at( Adjacent[sq][adj_idx] );
            is_safe &=  adj_piece.side == side || adj_piece.side == NO_COLOR ||\
                        (adj_piece.side == opponent && \
                            adj_piece.type > Soldier && adj_piece.type != Cannon);

            attack_opponent_General |= is_safe && (adj_piece.side == opponent && adj_piece.type == General);
        }

        bool is_free = true;
        for(int diag_idx=0; diag_idx < num_Diagonal[sq] && is_free; diag_idx++){
            Piece diag_piece = pos.peek_piece_at( Diagonal[sq][diag_idx] );
            is_free &= diag_piece.side == side || diag_piece.side == NO_COLOR ||\
                        (diag_piece.side == opponent && \
                            diag_piece.type > Soldier && diag_piece.type != Cannon);
        }

        safe_pawns += is_safe;
        danger_pawns += !is_safe;
        free_pawns += is_safe && is_free;
    }

    int valid_pawns = safe_pawns + (danger_pawns >> 2) + remain_hidden_pieces[side][Soldier];
    valid_pawns = std::max(valid_pawns, int(danger_pawns > 0));
    if(!opponent_General_exist){
        return valid_pawns;
    }

    if(valid_pawns == 5){
        return 0;
    }

    static constexpr int penalty_table[] = {
        -1000,   // 0
        -500,  // 1
        -10,  // 2
        -2,  // 3
        -1   // 4
    };

    bool cannot_win = valid_pawns == 0 && pos.count(side, Cannon) == 0 && remain_hidden_pieces[side][Cannon] == 0;


    return penalty_table[valid_pawns] + free_pawns;
}

Score CDCEvaluate::cannon_evaluate(const Position& pos, Color side, unsigned short remain_hidden_pieces[2][8]){
    return 0;
}