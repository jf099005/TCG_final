// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"

#include"AB_agent.h"
#include <chrono>
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

// le fishe
int main()
{
    /*
     * This is a simple Monte Carlo agent, it does
     *     - move generation
     *     - simulation
     *
     * To make it good MCTS, you still need:
     *     - a tree
     *     - Some UCB math
     *     - other enhancements
     *
     * You SHOULD create new files instead of cramming everything in this one,
     * it MAY affect your readability score.
     */
    std::string line;
    /* read input board state */
    ACDC acdc;
    int remain_moves = maximum_static_moves;
    int face_up_pieces = 0;
    int current_step = 0;
    while (std::getline(std::cin, line)) {
        Position pos(line);
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

        double time_constraint = 1.0;

        int exp_depth = 6;
        // info << acdc.opt_solution_exp(pos, exp_depth, remain_moves);
        info << acdc.opt_solution(pos, time_constraint, remain_moves);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        debug << "time:" << double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den << std::endl;



        // #ifdef TT_H
        // acdc.trace_PV(pos, exp_depth);
        // #endif
        remain_moves -= 2;
    }
}
