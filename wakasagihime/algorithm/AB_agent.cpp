#include"AB_agent.h"
#include<iomanip>
void MoveOrderer::ordering_move(const Position& pos, MoveList<> &moves){    

    assert(moves.size() <= 200);
    static short moves_score[200];
    for(int i=0; i<moves.size(); i++){
        moves_score[i] = evaluate_move(pos, moves[i]);
    }

    for(int i=1; i<moves.size(); i++){
        int j=i;
        while(j>0 and moves_score[j-1] < moves_score[j]){
            std::swap(moves_score[j-1], moves_score[j]);
            // std::swap(moves[j-1], moves[j]);
            Move tmp = moves[j-1];
            moves[j-1] = moves[j];
            moves[j] = tmp;
            j--;
        }
    }

    return;
}

//0-4
// inline int mobility(const Position& pos, PieceType piece_tp, const Move& move){
//     Square sq = move.to();
//     int mobility = 0;
//     for(auto dir:AllMoveDirections){
//         Square sq_nx = sq + dir;
//         PieceType nx_tp = pos.peek_piece_at(sq_nx).type;
//         mobility += (distance(sq, sq_nx) == 1) and\
//                 (nx_tp == NO_PIECE or piece_tp > nx_tp);
//     }
//     return mobility;
// }

short MoveOrderer::evaluate_move(const Position& pos, Move move){
    static const int normal_move_point = 2;
    static const int basic_capture_move_point = 1;
    static const int basic_flip_move_point = 2;

    if(move.type() == Flipping)
        return basic_flip_move_point;

    PieceType piece_tp = pos.peek_piece_at(move.from()).type;

    PieceType target_tp = pos.peek_piece_at(move.to()).type;

    int capture_score = 0;

    if( target_tp != NO_PIECE){
        capture_score += Piece_Value[target_tp];
        if(piece_tp == Cannon){
            capture_score -= (Piece_Value[piece_tp] >> 1);
        }
        else{
            capture_score += (Piece_Value[piece_tp] >> 3);
        }
    }
    
    
    Square sq_to = move.to();

    int mobility = 1;
    int attack_val = 0, attack_cnt = 0;
    bool threaten = false;

    for(int adj_dir = 0; adj_dir < num_Adjacent[sq_to] and !threaten; adj_dir++){
        Square sq_nx = Adjacent[sq_to][adj_dir];
        Piece nx_piece = pos.peek_piece_at(sq_nx);

        if(sq_nx < 0 || sq_nx >= 32 || (distance(sq_to, sq_nx) > 1) ||\
             nx_piece.side == pos.due_up() || nx_piece.type == Hidden )
            continue;

        bool can_move = (
                            nx_piece.type == NO_PIECE ||
                            (
                                piece_tp > nx_piece.type && 
                                nx_piece.type != piece_tp
                            )
                        );

        mobility += can_move;
        threaten |= nx_piece.type > piece_tp;
        bool can_attack = piece_tp > nx_piece.type &&\
                                 !threaten;
        
        attack_val += can_attack?Piece_Value[nx_piece.type]:0;
        attack_cnt+= can_attack;
    }

    capture_score += threaten?
            -(Piece_Value[piece_tp] + (Piece_Value[target_tp] >> 3)):\
            basic_capture_move_point;

    return (capture_score)*4 + (threaten? 0 : mobility*10 + 2*attack_val) + normal_move_point;
}



const static int non_capture_penalty = 10;



inline bool is_dangerous(const Position& pos, Square piece_sq){
    bool danger = false;

    Piece piece = pos.peek_piece_at(piece_sq);

    int opponent = !piece.side;
    assert(opponent == 0 || opponent == 1);

    for(int idx_sq = 0; !danger && idx_sq < num_Adjacent[piece_sq]; idx_sq++){
        Square sq_adj = Adjacent[piece_sq][idx_sq];
        Piece adj = pos.peek_piece_at(sq_adj);

        danger |= (adj.side == opponent && adj.type != Cannon && adj.type > piece.type);
    }
    return danger;
}

//return 0 if is not an attack move
inline int attack_gain(const Position& pos, Square piece_sq){
    int atk_gain = 0;

    Piece piece = pos.peek_piece_at(piece_sq);

    int opponent = !piece.side;
    assert(opponent == 0 || opponent == 1);

    for(int idx_sq = 0; idx_sq < num_Adjacent[piece_sq]; idx_sq++){
        Square sq_adj = Adjacent[piece_sq][idx_sq];
        Piece adj = pos.peek_piece_at(sq_adj);
        int pt = (adj.side == opponent && piece.type > adj.type)?Piece_Value[adj.type]:0;
        atk_gain = std::max(atk_gain, pt);
    }
    return atk_gain;
}

inline  bool is_unstable(const Position& pos, Move prv_move){
    int atk_gain = attack_gain(pos, prv_move.to());
    return is_dangerous(pos, prv_move.to()) or (atk_gain >= Piece_Value[Elephant]) or\
                (atk_gain > 0 and pos.count(pos.due_up()) <= 3);
};


double ACDC::Negamax(Position pos, int depth, int remain_moves, double alpha, double beta, Move prv, Square sq_danger){
    if(std::chrono::steady_clock::now() > deadline){
        return -CDCEvaluate::score_mx;
    }

    assert(depth >= -max_extend_depth);

    if(pos.winner() != NO_COLOR){
        return (pos.winner() == Mystery)? 0:\
        (pos.winner() == pos.due_up()) ?\
            CDCEvaluate::score_mx : -CDCEvaluate::score_mx;
    }

    if(remain_moves == 0){
        return 0;
    }
    
    if(depth <= 0){
        if(!is_unstable(pos, prv) or depth <= max_extend_depth){
            int score =  CDCEvaluate::calculate_score(pos, remain_moves);
            return score;
        }
    }

    #ifdef TT_H
    TT_info* tt_lookup = TT->query(pos, depth);
    if( tt_lookup->depth >= depth ){
        return tt_lookup->score;
    }
    #endif

    visited_states++;


    double opt = alpha;
    Move opt_move;
    MoveList<> nx_moves(pos);
    
    orderer->ordering_move(pos, nx_moves);

    // #ifdef TT_H
    
    // if(tt_lookup->depth > 0){
    //     for(int i=0; i<nx_moves.size(); i++){
    //         if(nx_moves[i] == tt_lookup->opt_move){
    //             Move tmp = nx_moves[0];
    //             nx_moves[0] = nx_moves[i];
    //             nx_moves[i] = tmp;
    //         }
    //     }
    // }

    // #endif

    for(int move_idx = 0; move_idx < nx_moves.size(); move_idx++){
        //
        Move nx_move = nx_moves[move_idx];
        if(nx_move.type() == Flipping and depth <= 1 and nx_moves[0].type() != Flipping)
            continue;

        double v = Move_Evaluate(pos, nx_move, depth-1, remain_moves-1, -beta, -opt);
        if(v > opt){
            opt = v;
            opt_move = nx_move;
        }

        if(v >= beta){

            #ifdef TT_H
            TT->write(tt_lookup, depth, opt, opt_move);
            #endif
            correct_prediction += (move_idx <= 2);
            fail_prediction += !(move_idx <= 2);
            return v;
        }
    }

    #ifdef TT_H
    TT->write(tt_lookup, depth, opt, opt_move);
    #endif

    bool prediction = (nx_moves.size() and opt_move == nx_moves[0]);
    prediction |= (nx_moves.size() >= 1 and opt_move == nx_moves[1]);
    prediction |= (nx_moves.size() >= 2 and opt_move == nx_moves[2]);
    
    correct_prediction += prediction;
    fail_prediction += !prediction;


    return opt;
}

double ACDC::Move_Evaluate(Position pos, Move move, int depth, int remain_moves, double alpha, double beta){
    //the move is an ordinary moves of a stone
    if(move.type() != Flipping){
        if(pos.peek_piece_at(move.to()).type != NO_PIECE)
            remain_moves = 30;

        pos.do_move(move);
        return -Negamax(pos, depth, remain_moves, alpha, beta, move);
    }

    //otherwise, the move will be a flipping operation, perform star-algorithm
    Square hidden_sq = move.from();

    #ifdef TT_H
    TT_info* tt_lookup = TT->query_for_flipping(pos, depth, hidden_sq);

    if(tt_lookup->depth >= depth){
        return tt_lookup->score;
    }
    #endif

    Position pos_copy(pos);

    pos_copy.do_move(move);//so the player is changed
    


    int C=0;
    // debug <<"start iterate\n";
    for(int piecetype = General; piecetype <= Soldier; piecetype++)
        for(int color = Black; color <= Red; color++)
            C += remain_hidden_pieces[color][piecetype];


    // debug << "start:" << General <<", end:" << Soldier <<std::endl;

    double CscoreMax = CDCEvaluate::score_mx;
    double CscoreMin = -CDCEvaluate::score_mx;

    int total_mass = 0;
    double total_score = 0;

    double A = C*(alpha - CDCEvaluate::score_mx) + CDCEvaluate::score_mx;
    double B = C*(beta - (-CDCEvaluate::score_mx)) + (-CDCEvaluate::score_mx);
    
    double Calpha = C*alpha;
    double Cbeta = C*beta;

    for(int piecetype = General; piecetype <= Soldier; piecetype++){
        for(int color = Black; color <= Red; color++){
            int branch_mass = remain_hidden_pieces[color][piecetype];

            if(!branch_mass)
                continue;
            
            Piece piece(static_cast<Color>(color), static_cast<PieceType>(piecetype));
            pos_copy.place_piece_at(piece, hidden_sq);

            double eval;
            if(depth >= 1)
                eval = -Negamax(pos_copy, depth-1, 30, -CDCEvaluate::score_mx, CDCEvaluate::score_mx, move);
            else
                eval = -Negamax(pos_copy, depth, 30, -CDCEvaluate::score_mx, CDCEvaluate::score_mx, move);
            
            total_mass += branch_mass;
            total_score += branch_mass * eval;

            CscoreMax = total_score + (C-total_mass)*CDCEvaluate::score_mx;
            CscoreMin = total_score - (C-total_mass)*CDCEvaluate::score_mx;

            if(CscoreMin >= Cbeta){
                // return beta;
                #ifdef TT_H
                TT->write(tt_lookup, depth, CscoreMin/C, move);
                #endif
                return CscoreMin/C;
            }

            if(CscoreMax <= Calpha){
                // return alpha;
                #ifdef TT_H
                TT->write(tt_lookup, depth, CscoreMax/C, move);
                #endif
                return CscoreMax/C;
            }

            // debug << "\t\t piece " << piecetype <<", color " << color <<'\n';
            // debug << "\t\t\t mass:" << branch_mass << '\n';
            // debug << "\t\t\t accmulate:" << total_score <<std::endl;

        }
    }
    // debug << "end iterate\n";
    #ifdef TT_H
    TT->write(tt_lookup, depth, total_score/C, move);    
    #endif
    return total_score / total_mass;
}

Move ACDC::opt_solution_with_fixed_depth(Position pos, int depth, int remain_moves){
    MoveList<> nx_moves(pos);
    Move opt_move = nx_moves[0];
    #ifdef TT_H
    TT_info* tt_lookup = TT->query(pos, depth);
    #endif
    // reset();
    // debug << "evaluate the first move:\n" << opt_move; 
    double opt_score = Move_Evaluate(pos, opt_move, depth-1, remain_moves-1, -CDCEvaluate::score_mx, CDCEvaluate::score_mx);

    // debug << opt_move<<":\n";
    // debug << '\t' << opt_score << std::endl;
    // debug << "\t visited nodes:" << visited_states << std::endl;

    for(int i=1; i<nx_moves.size(); i++){
        // debug << nx_moves[i] << ":\n";
        // reset();

        double move_score = Move_Evaluate(pos, nx_moves[i], depth-1, remain_moves-1, opt_score, CDCEvaluate::score_mx);

        if(move_score > opt_score){
            opt_move = nx_moves[i];
            opt_score = move_score;
        }

        // debug << '\t' << move_score <<std::endl;
        // debug << "\t visited nodes:" << visited_states << std::endl;
    }

    #ifdef TT_H
    TT->write(pos, depth, opt_score, opt_move);
    #endif

    return opt_move;
}

Move ACDC::opt_solution_exp(Position pos, int depth, int remain_moves){
    MoveList<> nx_moves(pos);
    Move opt_move = nx_moves[0];
    #ifdef TT_H
    TT_info* tt_lookup = TT->query(pos, depth);
    #endif
    reset();

    debug << "initial evaluation value:" << CDCEvaluate::calculate_score(pos, remain_moves) << std::endl;

    debug << "evaluate the first move:\n" << opt_move; 
    double opt_score = Move_Evaluate(pos, opt_move, depth-1, remain_moves-1, -CDCEvaluate::score_mx, CDCEvaluate::score_mx);

    debug << opt_move<<":\n";
    debug << '\t' << opt_score << std::endl;
    debug << "\t visited nodes:" << visited_states << std::endl;


    for(int i=1; i<nx_moves.size(); i++){
        debug << nx_moves[i] << ":\n";
        reset();

        double move_score = Move_Evaluate(pos, nx_moves[i], depth-1, remain_moves-1, opt_score, CDCEvaluate::score_mx);

        if(move_score > opt_score){
            opt_move = nx_moves[i];
            opt_score = move_score;
        }

        debug << '\t' << move_score <<std::endl;
        debug << "\t visited nodes:" << visited_states << std::endl;
    }

    #ifdef TT_H
    TT->write(pos, depth, opt_score, opt_move);
    #endif

    return opt_move;
}

Move ACDC::opt_solution(Position pos, double given_time, int remain_moves){

    reset();

    double time_constraint = given_time;

    int depth = 2;
    Move opt;


    auto start = std::chrono::steady_clock::now();

    int64_t time_us =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::duration<double>(time_constraint)
            ).count();

    this->deadline = start + std::chrono::microseconds(time_us);

    MoveList<> nx_moves(pos);
    orderer->ordering_move(pos, nx_moves);
    debug << "ordered next move: " << nx_moves[0] << '\n';

    while(true){
        reset();

        Move search_solution = opt_solution_with_fixed_depth(pos, depth, remain_moves);
        // auto end = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // double spent_time = double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den;

        debug << "depth " << depth <<std::endl;
        debug << "\tvisited: " << visited_states << '\n';
        debug << "\tcorrect prediction:" << correct_prediction<<'\n';
        debug << "\tfail prediction:" << fail_prediction << "\n";
        debug << "\tsuccess rate: " << std::fixed << std::setprecision(3) << double(correct_prediction) / \
                            double(correct_prediction + fail_prediction) << '\n';

        if(std::chrono::steady_clock::now() > deadline)
            break;

        depth += 2;
        if(depth > 12)
            break;

        // debug << "depth: " << depth <<", total spent time:" << spent_time << std::endl;
        // if(spent_time*100 > given_time){
        //     break;
        // }
        debug << "search finished\n";
        debug << '\t' << search_solution;
        opt = search_solution;
    }
    return opt;
}


#ifdef TT_H

void ACDC::trace_PV(Position pos, int depth){
    debug << "==============PV================\n";

    while(depth>0){
        TT_info* tt_lookup = TT->query(pos, depth);
        debug << "depth:" << depth <<'\n';
        debug << pos;
        debug << pos.toFEN() << std::endl;
        debug <<  "\t score:" << CDCEvaluate::calculate_score(pos) <<std::endl;
        debug << "\t opt move: " << tt_lookup->opt_move;
        pos.do_move( tt_lookup->opt_move );
        depth--;
        if(tt_lookup->opt_move.type() == Flipping)
            depth--;
    }
    debug << "ended\n";
    debug << pos;
    debug << pos.toFEN() << std::endl;
    debug << "================================\n";
}

#endif