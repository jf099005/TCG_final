#include"AB_agent.h"


void MoveOrderer::ordering_move(MoveList<> &move){
    return;
}



const static int move_penalty = 10000;

int ACDC::Negamax(Position pos, int depth, int remain_moves, int alpha, int beta){
    if(pos.winner() != NO_COLOR){
        // if(pos.winner() == pos.due_up()){
        //     return score_mx;
        // }
        // if(pos.winner() == Mystery and pos.due_up() != solver_color){
        //     return score_mx;
        // }
        if(pos.winner() == Mystery)
            return 0;
        return -score_mx;
    }
    if(remain_moves == 0){
        if(pos.due_up() != solver_color)
            return score_mx;
        return -score_mx;
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
        int v = -Move_Evaluate(pos, nx_move, depth-1, remain_moves-1, -beta, -opt);
        
        // if(pos.due_up() == solver_color)
        //     v -= move_penalty;
        // else
        //     v += move_penalty;

        if(depth == depth_limit){
            debug << nx_move <<":" << v <<std::endl;
        }
        if(v > opt){
            opt = v;
            if(depth == depth_limit){
                opt_solution = nx_move;
            }
        }

        if(v >= beta){
            return v;
        }
        // opt = max(opt, v);
    }
    return opt;
}

int ACDC::Move_Evaluate(Position pos, Move move, int depth, int remain_moves, int alpha, int beta){
    //the move is an ordinary moves of a stone
    if(move.type() != Flipping){
        pos.do_move(move);
        return Negamax(pos, depth, remain_moves, alpha, beta);
    }

    //otherwise, the move will be a flipping operation, perform star-algorithm
    int player_color = pos.due_up();
    Position pos_copy(pos);

    pos_copy.do_move(move);//so the player is changed
    
    Square hidden_sq = move.from();
    int total_mass = 0;
    long long total_score = 0;
    for(int piecetype = General; piecetype <= Soldier; piecetype++){
        for(int color = Red; color <= Black; color++){
            int branch_mass = remain_hidden_pieces[color][piecetype];
            if(!branch_mass)
                continue;
            Piece piece(static_cast<Color>(color), static_cast<PieceType>(piecetype));
            pos_copy.place_piece_at(piece, hidden_sq);
            
            int eval = Negamax(pos_copy, depth, remain_moves, -score_mx, score_mx);
            total_mass += branch_mass;
            total_score += branch_mass * eval;
        }
    }

    return double(total_score) / (total_mass);
}