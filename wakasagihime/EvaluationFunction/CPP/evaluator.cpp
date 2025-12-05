#include"evaluator.h"
Score CDCEvaluate::calculate_score(Position pos, int remain_moves){
    Color opponent = pos.due_up() == Red? Black:Red;
    return pieces_score(pos, pos.pieces(pos.due_up()))\
             - pieces_score(pos, pos.pieces(opponent));
}