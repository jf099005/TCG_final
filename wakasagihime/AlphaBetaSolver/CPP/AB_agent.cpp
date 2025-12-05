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
        Position pos_nx(pos);

        pos_nx.do_move(nx_move);
        // if(pos_nx.winner() != NO_COLOR){
        //     if(pos_nx.winner() == solver_color)
        //         return score_mx;
        //     // continue;
        // }
        int v = -Negamax(pos_nx, depth-1, remain_moves-1, -beta, -opt);
        
        if(pos.due_up() == solver_color)
            v -= move_penalty;
        else
            v += move_penalty;

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