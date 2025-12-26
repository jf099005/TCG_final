// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"

#include"AB_agent.h"
#include <chrono>
#include"TT.h"
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
    
    std::string test_board1 = "1r1P3A/3a1PK1/1Pk2rPn/5e1P r";
    Position pos(test_board1);
    MoveList<> nx_moves(pos);
    ACDC agent;
    debug << "original board:\n";
    debug << pos;
    for(auto mv: nx_moves){
        Position copy(pos);
        copy.do_move(mv);
        debug << "===========sub-board========\n";
        debug << mv;
        debug << copy;
        int depth = 1;
        Move opt = agent.opt_solution_with_fixed_depth(copy, depth, 30);
        debug << "opt move:" << opt;
        // debug << "value of depth " << depth << ": " << agent.Negamax(pos, depth, 30, -1e6, 1e6, Move(0)) << "\n\n"; 

        // agent.trace_PV(copy, depth);
    }
    debug << "=====calculate opt solution===\n";
    Move sol = agent.opt_solution_with_fixed_depth(pos, 2, 30);
    debug << "opt solution of all: " << sol << '\n';

}
