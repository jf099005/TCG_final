#include"AB_agent.h"
#include<chrono>
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

short MoveOrderer::evaluate_move(const Position& pos, Move move){
    static const int normal_move_score = 2;
    static const int capture_move_score = 10;
    static const int flip_move_score = 1;


    if(move.type() == Flipping)
        return flip_move_score;

    PieceType s = pos.peek_piece_at(move.from()).type;

    PieceType e = pos.peek_piece_at(move.to()).type;

    if( e != NO_PIECE){
        int score = capture_move_score;
        score += Piece_Value[e];
        if(s == Cannon){
            score -= (Piece_Value[s] >> 1);
        }
        else{
            score += (Piece_Value[s] >> 1);
        }
        return score;
    }
    
    return normal_move_score;
}



const static int non_capture_penalty = 10;

double ACDC::Negamax(Position pos, int depth, int remain_moves, double alpha, double beta, Move prv){
    assert(depth >= 0);
    visited_states++;
    if(pos.winner() != NO_COLOR){
        if(pos.winner() == Mystery)
            return 0;
        if(pos.winner() == pos.due_up())
            return CDCEvaluate::score_mx;
        return -CDCEvaluate::score_mx;
    }
    if(remain_moves == 0){
        return 0;
    }
    
    if(depth == 0){
        int score =  CDCEvaluate::calculate_score(pos, remain_moves);
        return score;
    }

    #ifdef TT_H
    TT_info* tt_lookup = TT->query(pos, depth);
    if( tt_lookup->depth >= depth ){
        return tt_lookup->score;
    }
    #endif

    double opt = alpha;
    Move opt_move;
    MoveList<> nx_moves(pos);
    
    orderer->ordering_move(pos, nx_moves);

    #ifdef TT_H
    
    if(tt_lookup->depth > 0){
        for(int i=0; i<nx_moves.size(); i++){
            if(nx_moves[i] == tt_lookup->opt_move){
                Move tmp = nx_moves[0];
                nx_moves[0] = nx_moves[i];
                nx_moves[i] = tmp;
            }
        }
    }

    #endif

    for(Move nx_move: nx_moves){
        //

        if(nx_move.type() == Flipping and depth <= 1 and nx_move != nx_moves[nx_moves.size()-1])
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

            return v;
        }
    }

    #ifdef TT_H
    TT->write(tt_lookup, depth, opt, opt_move);
    #endif

    return opt;
}

double ACDC::Move_Evaluate(Position pos, Move move, int depth, int remain_moves, double alpha, double beta){
    //the move is an ordinary moves of a stone
    if(move.type() != Flipping){
        pos.do_move(move);
        return -Negamax(pos, depth, remain_moves, alpha, beta);
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
                eval = -Negamax(pos_copy, depth-1, 30, -CDCEvaluate::score_mx, CDCEvaluate::score_mx);
            else
                eval = -Negamax(pos_copy, depth, 30, -CDCEvaluate::score_mx, CDCEvaluate::score_mx);
            
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

Move ACDC::opt_solution(Position pos, double time_constaint, int remain_moves){
    int depth = 2;

    auto start = std::chrono::high_resolution_clock::now();
    Move opt;
    while(true){
        opt = opt_solution_with_fixed_depth(pos, depth, remain_moves);
        depth += 2;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double spent_time = double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den;

        debug << "depth: " << depth <<", total spent time:" << spent_time << std::endl;
        if(spent_time*100 > time_constaint){
            break;
        }
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