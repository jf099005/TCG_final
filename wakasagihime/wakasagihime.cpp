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
        if(mv.type() == Flipping)
            continue;

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
    Position prv_pos;
    std::ofstream record_ofs;
    record_ofs.open(game_record_path);

    while (std::getline(std::cin, line)) {

        Position pos(line);


        // MoveList moves(pos);
        auto start = std::chrono::high_resolution_clock::now();

        if(pos.count(FACE_UP) == 32 || pos.count(Hidden) == 32){
            debug << " ===========a new game=============\n";
            current_step = 0;
            remain_moves = maximum_static_moves;
        }
        else{
            if(pos.count(FACE_UP) != face_up_pieces){
                remain_moves = maximum_static_moves;
            }

            Move prv_move = get_move(prv_pos, pos);
            if(prv_move.type() == Flipping){
                Square flipping_sq = prv_move.from();
                acdc.flipping_piece( pos.peek_piece_at(flipping_sq) );
                acdc.check(pos);
            }
        }

        record_ofs << "@ step " << current_step <<'\n';
        record_ofs << pos;
        record_ofs <<"\t" << pos.toFEN()<<"\n\n";

        debug << pos << std::endl;
        debug << "\t remain time:" << pos.time_left() <<std::endl;
        debug << "\t remain moves:" << remain_moves <<std::endl;

        record_ofs << "\t remain static moves:" << remain_moves <<std::endl;
        
        if(pos.time_left() < 0){
            prv_pos = pos;
            remain_moves--;
            current_step++;
            continue;
        }

        double time_constraint = 1.0;

        Move opt = acdc.opt_solution(pos, time_constraint, remain_moves);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        debug << "time:" << double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den << std::endl;
        
        record_ofs << "optimal move:" << opt;
        record_ofs << "correct prediction:" << acdc.correct_prediction << '\n';
        record_ofs << "failed prediction:" << acdc.fail_prediction << "\n";
        record_ofs << "\tsuccess rate: " << std::fixed << std::setprecision(3) << double(acdc.correct_prediction) / \
                            double(acdc.correct_prediction + acdc.fail_prediction) << '\n';
        record_ofs << "time:" << double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den << std::endl;

        record_ofs << "depth: " << acdc.max_visited_depth << '\n';

        record_ofs << "visited positions:" << acdc.visited_states <<'\n';
        record_ofs << "visited criticals:" << acdc.visited_critical_states << '\n'; 
        // debug << "eval:" << CDCEvaluate::calculate_score(pos, 30) << "/" << \
        //         CDCEvaluate::distance_score(pos, Red, Black, ALL_PIECES) <<std::endl;

        // #ifdef TT_H
        // acdc.trace_PV(pos, exp_depth);
        // #endif
        face_up_pieces = pos.count(FACE_UP);
        remain_moves --;
        current_step++;
        prv_pos = pos;
        info << opt;

    }
}
