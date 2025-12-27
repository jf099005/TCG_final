// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include"AB_agent.h"
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
    /* read input board state */
    std::string testdata[] = {
        "k6A/8/8/8 r", 
        "k5A1/8/8/8 r",
        "k6A/8/8/7A",
        "1r1a3A/4P1K1/1Pk2rPn/5e1P r",
        "1r1a3A/5PK1/2P2rPn/5e1P r",
        
    };

    // ACDC agent;
    for(auto line: testdata) {
        debug << "------------new data-----------------\n";
        Position pos(line);
        debug << pos;
        debug << "calculated score:" << CDCEvaluate::calculate_score(pos, 30) << '\n' << '\n';

    }
}
