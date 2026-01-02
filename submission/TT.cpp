#include"TT.h"

CDCTranspositionTable::CDCTranspositionTable(int hash_bits){
    random_value_max = (1ll << hash_bits);

    Record = new TT_info[random_value_max+1];

    for(int side = Black; side <= Red; side++){
        for(int piece = General; piece <= Soldier; piece++){
            for(int sq = SQ_A1; sq < SQUARE_NB; sq++){
                piece_hash[side][piece][sq] = rng(random_value_max);
            }
        }
        SideToMove_hash[side] = rng(random_value_max);
    }

    for(int sq = SQ_A1; sq< SQUARE_NB; sq++){
        hidden_piece_hash[sq] = rng(random_value_max);
        flipping_sq_hash[sq] = rng(random_value_max);
    }

}

CDCTranspositionTable::~CDCTranspositionTable(){
    delete [] Record;
}

TT_info* CDCTranspositionTable::query(const Position& pos, int depth){
    long long pos_hash = hash_pos(pos);
    // assert(pos_hash <= random_value_max && pos_hash >= 0);

    TT_info* lookup = Record + pos_hash;
    // if(pos_info.score == notfound or pos_info.depth < depth){
    //     return notfound;
    // }
    num_query++;
    if( lookup->depth == DEFAULT_DEPTH || lookup->hidden_pieces_location != pos.pieces(Hidden)){
        return nullptr;
        // (*lookup) = TT_info();
    }
    else
        num_success_query++;
    return lookup;
}


TT_info* CDCTranspositionTable::query_for_flipping(const Position& pos, int depth, Square flip_sq){
    
    long long pos_hash = hash_pos(pos);
    pos_hash ^= flipping_sq_hash[flip_sq];
    // assert(pos_hash <= random_value_max && pos_hash >= 0);
    TT_info* lookup = Record + pos_hash;
    // if(pos_info.score == notfound or pos_info.depth < depth){
    //     return notfound;
    // }
    num_query++;
    if( lookup->depth == DEFAULT_DEPTH || lookup->hidden_pieces_location != pos.pieces(Hidden))
        return nullptr;
        // *lookup = TT_info();
    else
        num_success_query++;
    return lookup;
}



void CDCTranspositionTable::write(const Position& pos, int depth, double score, Move opt_move, bool is_exact_value){
    long long pos_hash = hash_pos(pos);
    assert(pos_hash <= random_value_max && pos_hash >= 0);
    Record[pos_hash].score = score;
    Record[pos_hash].depth = depth;
    Record[pos_hash].opt_move = opt_move; //= TT_info(score, depth, opt_move);
    Record[pos_hash].hidden_pieces_location = pos.pieces(Hidden);
    Record[pos_hash].is_exact_value = is_exact_value;
}



void CDCTranspositionTable::write_of_flipping(const Position& pos, int depth, Square flip_sq, double score, Move opt_move, bool is_exact_value){
    long long pos_hash = hash_pos(pos);
    pos_hash ^= flipping_sq_hash[flip_sq];
    assert(pos_hash <= random_value_max && pos_hash >= 0);
    Record[pos_hash].score = score;
    Record[pos_hash].depth = depth;
    Record[pos_hash].opt_move = opt_move; //= TT_info(score, depth, opt_move);
    Record[pos_hash].hidden_pieces_location = pos.pieces(Hidden);
    Record[pos_hash].is_exact_value = is_exact_value;
}


// void CDCTranspositionTable::write(TT_info* info_ptr, const Position& pos, int depth, double score, Move opt_move, bool is_exact){
//     assert(-100 <= depth && depth <= 100);
//     info_ptr->score = score;
//     info_ptr->depth = depth;
//     info_ptr->opt_move = opt_move; //= TT_info(score, depth, opt_move);
//     info_ptr->is_exact_value = is_exact;
//     info_ptr->hidden_pieces_location = pos.pieces(Hidden);
// }


long long CDCTranspositionTable::hash_pos(const Position& pos){
    long long hash_value = 0;
    for(Square sq: BoardView(pos.pieces(FACE_UP))){
        Piece piece = pos.peek_piece_at(sq);
        hash_value ^= piece_hash[ piece.side ][ piece.type ][ sq ];
    }

    for( Square sq: BoardView(pos.pieces(Hidden)) ){
        hash_value ^= hidden_piece_hash[sq];
    }

    hash_value ^= SideToMove_hash[ pos.due_up() ];
    return hash_value;
}