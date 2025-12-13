// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"

#include"AB_agent.h"
#include <chrono>
using namespace std::chrono;



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
        auto start = high_resolution_clock::now();

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

        info << acdc.opt_solution(pos, 6, remain_moves);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        debug << "time:" << double(duration.count())*microseconds::period::num/microseconds::period::den << std::endl;


        remain_moves -= 2;
        // int min_score = 100;
        // int chosen = 0;

        // for (int i = 0; i < moves.size(); i += 1) {
        //     Position copy(pos);
        //     copy.do_move(moves[i]);
        //     int local_score = 0;
        //     for (int j = 0; j < 20; j += 1) {
        //         /* Run some (20) simulations. */
        //         local_score += copy.simulate(strategy_random);
        //     }
        //     /*
        //      * The simulations started from the opponent's perspective,
        //      * so we choose the move that led to the MINIMUM score here.
        //      */
        //     if (local_score < min_score) {
        //         chosen = i;
        //         min_score = local_score;
        //     }
        //     debug << "This is a debug message " << local_score << "\n";
        //     std::fflush(stderr);
        // }
        // /* output the move */
        // info << moves[chosen];
    }
}
