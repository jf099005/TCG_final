// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include"AlphaBetaSolver/H/AB_agent.h"
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
    std::string line;
    /* read input board state */
    while (std::getline(std::cin, line)) {
        Position pos(line);
        ACDC agent(pos.due_up(), 0);
        debug << " output pos:\n";
        debug << pos <<std::endl;

        MoveList<> nx_moves(pos);

        debug << "all moves:" << nx_moves.size() <<std::endl;
        debug <<"red pieces:" << pos.count(Red) <<", black count:" << pos.count(Black) <<'\n';
        for(int i=0; i<nx_moves.size(); i++){
            debug << nx_moves[i];
            debug <<"\t" << nx_moves[i].from() << '/' << nx_moves[i].to() << '\n';
        }

        debug << "static evaluation:"<< CDCEvaluate::calculate_score(pos) << std::endl;
        debug << "Negamax leaf evaluation:" << agent.Negamax(pos, 0, 30) <<std::endl;

    }
}
