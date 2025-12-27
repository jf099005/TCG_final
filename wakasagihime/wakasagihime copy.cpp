// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"

#include"AB_agent.h"
#include <chrono>
#include<fstream>
#include<iomanip>
// using namespace std::chrono;



// Girls are preparing...
__attribute__((constructor)) void prepare()
{
    // Prepare the distance table
    for (Square i = SQ_A1; i < SQUARE_NB; i += 1) {
        for (Square j = SQ_A1; j < SQUARE_NB; j += 1) {
            SquareDistance[i][j] = distance<Rank>(i, j) + distance<File>(i, j);
        }
    }

    // Prepare the attack table (regular)
    Direction dirs[4] = { NORTH, SOUTH, EAST, WEST };
    for (Square sq = SQ_A1; is_okay(sq); sq += 1) {
        Board a = 0;
        for (Direction d : dirs) {
            a |= safe_destination(sq, d);
        }
        PseudoAttacks[sq] = a;
    }

    // Prepare magic
    init_magic<Cannon>(cannonTable, cannonMagics);
}

const int maximum_static_moves = 30;

inline Move get_move(Position pos_prv, Position pos_cur){
    MoveList<> moves(pos_prv);
    for(Move mv: moves){
        Position copy(pos_prv);
        copy.do_move(mv);
        if(copy.toFEN() == pos_cur.toFEN())
            return mv;
    }
    int n_diff = 0;
    Move mv;
    for(Square sq = SQ_A1; sq <= SQ_H4; sq = sq+1){
        if(pos_prv.peek_piece_at(sq).type != pos_cur.peek_piece_at(sq).type){
            n_diff++;
            mv = Move(sq, sq);
        }
    }
    assert(n_diff == 1);
    return mv;
}

// le fishe
int main()
{
    std::string line;
    /* read input board state */
    ACDC acdc;
    int remain_moves = maximum_static_moves;
    int face_up_pieces = 0;
    int current_step = 0;

    std::string game_record_path = "/mnt/20F408ADF408876E/TCG/TCG_final/wakasagihime/record.txt";
    std::ofstream record_ofs;

    record_ofs.open(game_record_path);

    Position prv_pos;

    while (std::getline(std::cin, line)) {
        Position pos(line);

        record_ofs << pos;
        record_ofs <<"\t" << pos.toFEN()<<"\n\n";

        if(pos.time_left() < 0){
            continue;
        }
        // MoveList moves(pos);
        auto start = std::chrono::high_resolution_clock::now();

        if(pos.count(Hidden) == 32){
            debug << " a new game\n";
            current_step = 0;
            remain_moves = maximum_static_moves;
        }

        if(pos.count(FACE_UP) != face_up_pieces){
            remain_moves = maximum_static_moves;
        }

        debug << pos << std::endl;
        debug << "\t remain time:" << pos.time_left() <<std::endl;
        debug << "\t remain moves:" << remain_moves <<std::endl;
        record_ofs << "\t remain moves:" << remain_moves <<std::endl;

        double time_constraint = 1.0;

        int exp_depth = 6;
        // info << acdc.opt_solution_exp(pos, exp_depth, remain_moves);
        Move opt = acdc.opt_solution(pos, time_constraint, remain_moves);
        info << opt;
        record_ofs << opt;
        record_ofs << "\tsuccess rate: " << std::fixed << std::setprecision(3) << double(acdc.correct_prediction) / \
                            double(acdc.correct_prediction + acdc.fail_prediction) << '\n';

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        debug << "time:" << double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den << std::endl;

        // debug << "eval:" << CDCEvaluate::calculate_score(pos, 30) << "/" << \
        //         CDCEvaluate::distance_score(pos, Red, Black, ALL_PIECES) <<std::endl;

        // #ifdef TT_H
        // acdc.trace_PV(pos, exp_depth);
        // #endif
        remain_moves -= 2;
    }
}
