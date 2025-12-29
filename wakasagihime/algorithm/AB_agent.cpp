#include"AB_agent.h"
#include<iomanip>
#include<cstdlib>
// #define OUTPUT_RECURSION_TREE 1

inline void swap_moves(MoveList<>& moves, int* moves_score, int i, int j){
    if(moves_score != nullptr)std::swap(moves_score[i], moves_score[j]);
    // std::swap(moves[j-1], moves[j]);
    Move tmp = moves[i];
    *(moves.begin() + i) = moves[j];
    *(moves.begin() + j) = tmp;
}

const Move PAUSE = Move(1919);

int MoveOrderer::ordering_move(const Position& pos, MoveList<> &moves, bool only_critical_move, bool skip_flipping){    
    assert(moves.size() <= 200 && moves.size());
    static int moves_score[200];

    int valid_moves_num = moves.size();

    //filter the critical moves
    if(only_critical_move){
        int L=0, R=0;
        while(R<moves.size()){
            if(is_critical_move(pos, moves[R])){
                if(L<R)swap_moves(moves, nullptr, L, R);
                L++;
            }
            R++;
        }
        if(!L)return 0;
        valid_moves_num = L;
    }
    else if(skip_flipping){
        int L=0, R=0;
        while(R<moves.size()){
            if(moves[R].type() != Flipping){
                if(L<R)swap_moves(moves, nullptr, L, R);
                L++;
            }
            R++;
        }
        if(!L)return 0;
        valid_moves_num = L;
    }
    // debug << "movelist before reordering:\n";
    // for(int i=0; i<valid_moves_num;i++)
    //     debug << moves[i];

    #ifdef ORDERING
    for(int i=0; i<valid_moves_num; i++){
        moves_score[i] = evaluate_move(pos, moves[i]);
    }

    for(int i=1; i<valid_moves_num; i++){
        int j=i;
        while(j>0 and ( moves_score[j-1] < moves_score[j]  )){
            swap_moves(moves, moves_score, j-1, j);
            j--;
        }
    }
    #endif

    return valid_moves_num;
}

bool MoveOrderer::is_critical_move(const Position& pos, Move mv){
    return (mv.type() == Moving) && (
        pos.peek_piece_at(mv.to()).type != NO_PIECE || is_escape_move(pos, mv)
    );
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

int MoveOrderer::evaluate_move(const Position& pos, Move move){
    static const int normal_move_point = 2;
    static const int basic_capture_move_point = 1;
    static const int basic_flip_move_point = 2;

    if(move.type() == Flipping)
        return basic_flip_move_point;

    Piece piece = pos.peek_piece_at(move.from());

    PieceType target_tp = pos.peek_piece_at(move.to()).type;

    int capture_score = 0;

    if( target_tp != NO_PIECE){
        capture_score += Piece_Value[target_tp];
        // if(piece_tp != Cannon){
        //     capture_score += (Piece_Value[piece_tp] >> 3);
        // }
    }
    
    
    Square sq_to = move.to();

    int positional_score_before = evaluate_square(pos, move.from(), piece, 1);
    int positional_score_after = evaluate_square(pos, move.to(), piece, 1);

    positional_score_before = (positional_score_before==DANGER? - Piece_Value[piece.type]*4 : positional_score_before);
    positional_score_after = (positional_score_after == DANGER? - Piece_Value[piece.type]*4: positional_score_after);

    return capture_score*4 + positional_score_after - positional_score_before;

}



const static int non_capture_penalty = 10;

//in fact, NegaScout
Score ACDC::Negamax(Position pos, int depth, int remain_moves, Score alpha, Score beta, Move prv){
    #ifdef TIMING
    if(std::chrono::steady_clock::now() > deadline){
        return -CDCEvaluate::score_mx;
    }
    #endif 

    #ifdef OUTPUT_RECURSION_TREE


    debug << prv;
    debug <<"Negamax of depth " << depth << '\n';
    debug <<pos;
    debug << "\tis unstable:" << is_unstable(pos) << '\n';
    debug << "Press Enter to continue...";
    std::cin.get();    
    #endif

    assert(depth >= lim_extend_depth);

    if(pos.winner() != NO_COLOR){
        return (pos.winner() == Mystery)? 0:\
        (pos.winner() == pos.due_up()) ?\
            CDCEvaluate::score_mx + remain_moves*10: -CDCEvaluate::score_mx;
    }

    if(remain_moves == 0){
        return 0;
    }

    Score opt = -CDCEvaluate::score_mx;

    Move opt_move;

    #ifdef TT_H
    TT_info* tt_lookup = TT->query(pos, depth);
    if( tt_lookup->depth >= depth){
        #ifdef OUTPUT_RECURSION_TREE
        debug << "return from TT:" << tt_lookup->score <<", calculated score: " << CDCEvaluate::calculate_score(pos, remain_moves) << '\n';
        debug << "Press Enter to continue...";
        std::cin.get();    
        #endif
        if(tt_lookup->is_exact_value){
            return tt_lookup->score;
        }
        // opt = tt_lookup->score;
        // opt_move = tt_lookup->opt_move;
    }
    #endif
    
    // Move()
    visited_states++;

    if(depth <= 0){

        #ifdef OUTPUT_RECURSION_TREE
        debug << "return of leaf, calculated score: " << CDCEvaluate::calculate_score(pos, remain_moves) << '\n';
        debug << "Press Enter to continue...";
        std::cin.get();    
        #endif

        #ifndef QUIESCENT_SEARCH
        int score =  CDCEvaluate::calculate_score(pos, remain_moves);
        return score;
        #endif

        if(!is_unstable(pos) or depth <= lim_extend_depth){
            int score =  CDCEvaluate::calculate_score(pos, remain_moves);
            // TT->write(tt_lookup, depth, score, opt_move, true);
            return score;
        }
    }


    visited_critical_states += (depth <= 0);

    MoveList<> nx_moves(pos);
    
    bool critical_search = (depth <= 0);
    bool no_flipping = (depth <= 2);

    int num_valid_moves = nx_moves.size();
    num_valid_moves = orderer->ordering_move(pos, nx_moves, critical_search, no_flipping);

    if(critical_search){
        if(prv == PAUSE){
            opt = CDCEvaluate::calculate_score(pos, remain_moves);
        }
        else{
            Position pos_copy(pos);
            pos_copy.pass_turn();
            opt = -Negamax(pos_copy, depth-1, remain_moves-1, -beta, -alpha, PAUSE);
            if(opt >= beta)
                return opt;
        }
    }

    if(num_valid_moves == 0){
        if(prv == PAUSE){
            return CDCEvaluate::calculate_score(pos, remain_moves-1);
        }
        pos.pass_turn();
        return -Negamax(pos, depth-1, remain_moves-1, -beta, -alpha, PAUSE);
    }

    #ifdef TT_H
    
    #ifdef ORDERING
    if(tt_lookup->depth > 0){
        for(int i=0; i<num_valid_moves; i++){
            if(nx_moves[i] == tt_lookup->opt_move){
                Move tmp = nx_moves[0];
                nx_moves[0] = nx_moves[i];
                nx_moves[i] = tmp;
                break;
            }
        }
    }
    #endif

    #endif

    int num_visited = 1;

    #ifdef OUTPUT_RECURSION_TREE
    debug << "\tsearch branch:" << num_valid_moves <<'\n';
    #endif

    #ifdef NEGASCOUT
    opt = Move_Evaluate(pos, nx_moves[0], depth-1, remain_moves-1, -beta, -alpha);
    const int iteration_start = 1;
    if(opt >= beta){
        #ifdef TT_H
        if(depth > 0)
            TT->write(tt_lookup, depth, opt, opt_move, false);
        #endif
        correct_prediction += (1 <= 2);
        fail_prediction += !(1 <= 2);
        return opt;
    }
    alpha = std::max(alpha, opt);
    #else
    const int iteration_start = 0;
    #endif

    for(int move_idx = iteration_start; move_idx < num_valid_moves; move_idx++){
        Move nx_move = nx_moves[move_idx];    
        #ifdef OUTPUT_RECURSION_TREE
        debug << "\t\tsearch the branche of " << nx_move <<'\n';
        #endif

        #ifdef NEGASCOUT
        const static double EPS = 1e-6;
        //Scout search
        // Score v = Move_Evaluate(pos, nx_move, depth-1, remain_moves-1, -bound-1, -alpha);
        Score v = Move_Evaluate(pos, nx_move, depth-1, remain_moves-1, -opt-EPS, -opt);
        // alpha = std::max(alpha, v);
        if(v > opt){
            if(v >= beta){
                opt = v;
                opt_move = nx_move;
            }
            else{
                v = Move_Evaluate(pos, nx_move, depth-1, remain_moves-1, -beta, -alpha);
                opt = v;
                opt_move = nx_move;
            }
        }
        #else
        Score v= Move_Evaluate(pos, nx_move, depth-1, remain_moves-1, -beta, -alpha);
        #endif

        if(v > opt){
            opt = v;
            opt_move = nx_move;
        }

        if(v >= beta){

            #ifdef TT_H
            if(depth > 0)
                TT->write(tt_lookup, depth, opt, opt_move, false);
            #endif
            correct_prediction += (move_idx <= 2);
            fail_prediction += !(move_idx <= 2);
            return v;
        }
        num_visited++;

        alpha = std::max(alpha, opt);
    }

    assert(num_visited > 0);

    #ifdef TT_H
    // if(depth > 0)   
        TT->write(tt_lookup, depth, opt, opt_move, true);
    #endif

    bool prediction = (nx_moves.size() and opt_move == nx_moves[0]);
    prediction |= (nx_moves.size() >= 1 and opt_move == nx_moves[1]);
    prediction |= (nx_moves.size() >= 2 and opt_move == nx_moves[2]);
    
    correct_prediction += prediction;
    fail_prediction += !prediction;

    return opt;
}

Score ACDC::Move_Evaluate(Position pos, Move move, int depth, int remain_moves, Score alpha, Score beta){
    //the move is an ordinary moves of a stone
    if(move.type() != Flipping){
        if(pos.peek_piece_at(move.to()).type != NO_PIECE)
            remain_moves = 30;

        // Color opponent = pos.peek_piece_at(move.to()).side;
        // PieceType opponent_pt = pos.peek_piece_at(move.to()).type;
        // if(opponent_pt != NO_PIECE){
        //     remain_hidden_pieces[opponent][ opponent_pt ]--;
        // }
        pos.do_move(move);
        Score v = -Negamax(pos, depth, remain_moves, alpha, beta, move);
        // if(opponent_pt != NO_PIECE){
        //     remain_hidden_pieces[opponent][ opponent_pt ]++;
        // }

        return v;
    }

    //otherwise, the move will be a flipping operation, perform star-algorithm
    Square hidden_sq = move.from();

    #ifdef TT_H
    TT_info* tt_lookup = TT->query_for_flipping(pos, depth, hidden_sq);

    if(tt_lookup->depth >= depth){
        return tt_lookup->score;
    }
    #endif

    // if(depth <= 0){
    //     return -CDCEvaluate::calculate_score(pos, remain_moves);
    // }
    Position pos_copy(pos);
    pos_copy.do_move(move);//so the player is changed
    


    int C=0;
    for(int piecetype = General; piecetype <= Soldier; piecetype++)
        for(int color = Black; color <= Red; color++)
            C += remain_hidden_pieces[color][piecetype];


    // debug << "start:" << General <<", end:" << Soldier <<std::endl;

    Score CscoreMax = CDCEvaluate::score_mx;
    Score CscoreMin = -CDCEvaluate::score_mx;

    int total_mass = 0;
    Score total_score = 0;

    Score A = C*(alpha - CDCEvaluate::score_mx) + CDCEvaluate::score_mx;
    Score B = C*(beta - (-CDCEvaluate::score_mx)) + (-CDCEvaluate::score_mx);
    
    Score Calpha = C*alpha;
    Score Cbeta = C*beta;

    for(int piecetype = General; piecetype <= Soldier; piecetype++){
        for(int color = Black; color <= Red; color++){
            int branch_mass = remain_hidden_pieces[color][piecetype];

            assert(branch_mass >= 0);

            if(!branch_mass)
                continue;
            
            Piece piece(static_cast<Color>(color), static_cast<PieceType>(piecetype));
            pos_copy.place_piece_at(piece, hidden_sq);

            Score eval;

            remain_hidden_pieces[color][piecetype]--;

            if(depth >= 1)
                eval = -Negamax(pos_copy, depth-1, 30, -CDCEvaluate::score_mx, CDCEvaluate::score_mx, move);
            else
                eval = -Negamax(pos_copy, depth, 30, -CDCEvaluate::score_mx, CDCEvaluate::score_mx, move);

            // debug << "branch of type" << piece.type<<", color " << piece.side <<'\n';
            // debug << "square position:" << hidden_sq << '\n';
            // debug << "branch position:" << pos_copy;
            // debug <<"depth:" << depth<<", score: " << eval << "\n";
            // debug <<"\n\n";

            remain_hidden_pieces[color][piecetype]++;

            total_mass += branch_mass;
            total_score += branch_mass * eval;

            CscoreMax = total_score + (C-total_mass)*CDCEvaluate::score_mx;
            CscoreMin = total_score - (C-total_mass)*CDCEvaluate::score_mx;

            if(CscoreMin >= Cbeta){
                // return beta;
                #ifdef TT_H
                TT->write(tt_lookup, depth, CscoreMin/C, move, false);
                #endif
                return CscoreMin/C;
            }

            if(CscoreMax <= Calpha){
                // return alpha;
                #ifdef TT_H
                TT->write(tt_lookup, depth, CscoreMax/C, move, false);
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
    TT->write(tt_lookup, depth, total_score/C, move, true);    
    #endif
    return total_score / total_mass;
}

Move ACDC::opt_solution_with_fixed_depth(Position pos, int depth, int remain_moves){
    MoveList<> nx_moves(pos);
    #ifdef TT_H
    TT_info* tt_lookup = TT->query(pos, depth);
    #endif

    orderer->ordering_move(pos, nx_moves, false, depth <= 2);
    Move opt_move = nx_moves[0];
    debug << "branch " << opt_move;
    Score opt_score = Move_Evaluate(pos, opt_move, depth-1, remain_moves-1, -CDCEvaluate::score_mx, CDCEvaluate::score_mx);
    debug << "score:" << opt_score << '\n';
    // debug << "first move score:" << opt_score << '\n';
    for(int i=1; i<nx_moves.size(); i++){
        if(nx_moves[i].type() == Flipping and depth <= 2 and nx_moves[0].type() != Flipping)
            continue;

        Score move_score = Move_Evaluate(pos, nx_moves[i], depth-1, remain_moves-1, -CDCEvaluate::score_mx, -opt_score);

        debug << "branch " << nx_moves[i];
        debug << "score:" << move_score << '\n';
        if(move_score > opt_score){
            opt_move = nx_moves[i];
            opt_score = move_score;
        }

        // Position copy(pos);
        // copy.do_move(nx_moves[i]);
        // debug <<"pv of the move:===\n";
        // trace_PV(copy, depth-1);
        // debug << '\n' << '\n';
        // debug << "\t visited nodes:" << visited_states << std::endl;
    }

    debug << "\topt score: " << opt_score << '\n';

    #ifdef TT_H
    TT->write(pos, depth, opt_score, opt_move);
    #endif

    return opt_move;
}

Move ACDC::opt_solution(Position pos, double given_time, int remain_moves){

    reset();
    max_visited_depth = 2;
    // given_time = 1000;

    double time_constraint = given_time;

    auto start = std::chrono::steady_clock::now();

    int64_t time_us =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::duration<double>(time_constraint)
            ).count();

    this->deadline = start + std::chrono::microseconds(time_us);

    MoveList<> nx_moves(pos);
    orderer->ordering_move(pos, nx_moves, false, false);
    debug << "ordered next move: " << nx_moves[0] << '\n';
    int depth = 2;
    Move opt = nx_moves[0];

    while(true){
        // reset();
        debug << "depth " << depth <<std::endl;

        Move search_solution = opt_solution_with_fixed_depth(pos, depth, remain_moves);
        // auto end = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // double spent_time = double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den;

        debug << "\tvisited: " << visited_states << '\n';
        debug << "\tvisited critical: " << visited_critical_states << '\n';
        debug << "\tcorrect prediction:" << correct_prediction<<'\n';
        debug << "\tfail prediction:" << fail_prediction << "\n";
        debug << "\tsuccess rate: " << std::fixed << std::setprecision(3) << double(correct_prediction) / \
                            double(correct_prediction + fail_prediction) << '\n';

        if(std::chrono::steady_clock::now() > deadline)
            break;


        // debug << "depth: " << depth <<", total spent time:" << spent_time << std::endl;
        // if(spent_time*100 > given_time){
        //     break;
        // }
        debug << "search finished\n";
        debug << '\t' << search_solution;
        // trace_PV(pos, depth);
        opt = search_solution;
        depth += 2;
        max_visited_depth += 2;
        // if(depth > 4)
        //     break;
    }
    return opt;
}


#ifdef TT_H

void ACDC::trace_PV(Position pos, int depth){
    debug << "==============PV================\n";

    while(true){
        TT_info* tt_lookup = TT->query(pos, depth);
        debug << "depth:" << depth <<'\n';
        debug << pos;
        debug << pos.toFEN() << std::endl;
        debug << "\t TT score:" << tt_lookup->score << '\n';
        debug <<  "\t score:" << CDCEvaluate::calculate_score(pos) <<std::endl;
        debug << "\t opt move: " << tt_lookup->opt_move;
        pos.do_move( tt_lookup->opt_move );
        depth--;
        if(tt_lookup->opt_move.type() == Flipping)
            depth--;
        if(tt_lookup->score == 114514 || depth <= -30)
            break;
    }
    debug << "ended\n";
    debug << pos;
    debug << pos.toFEN() << std::endl;
    debug << '\t' << "score:" << CDCEvaluate::calculate_score(pos) << std::endl;
    debug << "================================\n";
}

#endif