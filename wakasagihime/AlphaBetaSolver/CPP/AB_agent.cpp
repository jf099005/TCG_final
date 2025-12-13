#include"AB_agent.h"

void MoveOrderer::ordering_move(MoveList<> &move){
    return;
}



const static int non_capture_penalty = 10;

double ACDC::Negamax(Position pos, int depth, int remain_moves, int alpha, int beta){
    assert(depth >= 0);
    visited_states++;
    // debug <<"\ncall Negamax of "<< pos;
    // debug << "\t\t winner:" << pos.winner() <<std::endl;
    // debug << "\t\t due up:" <<pos.due_up() <<std::endl;
    // debug <<"\t\t depth:" << depth << '\n';
    if(pos.winner() != NO_COLOR){
        // if(pos.winner() == pos.due_up()){
        //     return score_mx;
        // }
        // if(pos.winner() == Mystery and pos.due_up() != solver_color){
        //     return score_mx;
        // }
        if(pos.winner() == Mystery)
            return 0;
        if(pos.winner() == pos.due_up())
            return score_mx;
        return -score_mx;
    }
    if(remain_moves == 0){
        // if(pos.due_up() != solver_color)
        //     return score_mx;
        return 0;//-score_mx;
    }
    
    if(depth == 0){
        int score =  CDCEvaluate::calculate_score(pos, remain_moves);//endgame_evaluator::evaluate(pos, solver_color);
        // if(pos.due_up() == solver_color)
        //     return score;
        return score;
    }

    int opt = -score_mx;
    MoveList<> nx_moves(pos);
    for(Move nx_move: nx_moves){
        // pos_nx.do_move(nx_move);
        // if(pos_nx.winner() != NO_COLOR){
        //     if(pos_nx.winner() == solver_color)
        //         return score_mx;
        //     // continue;
        // }
        int v = Move_Evaluate(pos, nx_move, depth-1, remain_moves-1, -beta, -opt);
        
        // if(pos.due_up() == solver_color)
        //     v -= move_penalty;
        // else
        //     v += move_penalty;

        // if(depth == depth_limit){
        //     debug << nx_move <<":" << v <<std::endl;
        // }
        if(v > opt){
            opt = v;
            // if(depth == depth_limit){
            //     opt_solution = nx_move;
            // }
        }

        if(v >= beta){
            return v;
        }
        // opt = max(opt, v);
    }
    return opt;
}

double ACDC::Move_Evaluate(Position pos, Move move, int depth, int remain_moves, int alpha, int beta){
    //the move is an ordinary moves of a stone
    // debug <<"call move_Evaluate\n";
    // debug << "move:" <<move<<'\n';
    if(move.type() != Flipping){
        bool is_capture_move = (pos.peek_piece_at(move.to()).type != NO_PIECE);
        pos.do_move(move);
        // debug <<"\t not flipping\n";
        return -Negamax(pos, depth, remain_moves, alpha, beta) - (!is_capture_move)*non_capture_penalty;
    }

    //otherwise, the move will be a flipping operation, perform star-algorithm
    // int player_color = pos.due_up();
    Position pos_copy(pos);

    pos_copy.do_move(move);//so the player is changed
    
    Square hidden_sq = move.from();
    int total_mass = 0;
    long long total_score = 0;
    // debug <<"start iterate\n";
    // debug << "start:" << General <<", end:" << Soldier <<std::endl;
    for(int piecetype = General; piecetype <= Soldier; piecetype++){
        for(int color = Black; color <= Red; color++){
            int branch_mass = remain_hidden_pieces[color][piecetype];

            if(!branch_mass)
                continue;
            
            Piece piece(static_cast<Color>(color), static_cast<PieceType>(piecetype));
            pos_copy.place_piece_at(piece, hidden_sq);
            double eval;
            if(depth >= 1)
                eval = -Negamax(pos_copy, depth-1, 30, -score_mx, score_mx);
            else
                eval = -Negamax(pos_copy, depth, 30, -score_mx, score_mx);
            
            total_mass += branch_mass;
            total_score += branch_mass * eval;

            
            // debug << "\t\t piece " << piecetype <<", color " << color <<'\n';
            // debug << "\t\t\t mass:" << branch_mass << '\n';
            // debug << "\t\t\t accmulate:" << total_score <<std::endl;

        }
    }
    // debug << "end iterate\n";
    return double(total_score) / (total_mass);
}

Move ACDC::opt_solution(Position pos, int depth, int remain_moves){
    MoveList<> nx_moves(pos);
    Move opt_move = nx_moves[0];
    debug << "evaluate the first move:\n" << opt_move; 
    double opt_score = Move_Evaluate(pos, opt_move, depth-1, 30, -score_mx, score_mx);

    debug << opt_move<<":\n";
    debug << '\t' << opt_score << std::endl;

    for(int i=1; i<nx_moves.size(); i++){
        debug << nx_moves[i] << ":\n";
        reset();

        double move_score = Move_Evaluate(pos, nx_moves[i], depth-1, remain_moves-1, -opt_move, score_mx);

        if(move_score > opt_score){
            opt_move = nx_moves[i];
            opt_score = move_score;
        }

        debug << '\t' << move_score <<std::endl;
        debug << "\t visited nodes:" << visited_states << std::endl;
    }
    return opt_move;
}