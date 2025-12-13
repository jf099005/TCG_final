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

    for(int sq = SQ_A1; sq< SQUARE_NB; sq++)
        hidden_piece_hash[sq] = rng(random_value_max);

}

CDCTranspositionTable::~CDCTranspositionTable(){
    delete [] Record;
}

double CDCTranspositionTable::query(Position pos, int depth){
    long long pos_hash = hash_pos(pos);
    assert(pos_hash <= random_value_max);
    TT_info pos_info = Record[pos_hash];
    if(pos_info.score == notfound or pos_info.depth < depth){
        return notfound;
    }

    return pos_info.score;
}


long long CDCTranspositionTable::hash_pos(Position pos){
    long long hash_value = 0;
    for(Square sq: BoardView(pos.pieces(FACE_UP))){
        Piece piece = pos.peek_piece_at(sq);
        hash_value ^= piece_hash[ piece.side ][ piece.type ][ sq ];
    }

    for(Square sq: BoardView(pos.pieces(Hidden))){
        hash_value ^= hidden_piece_hash[sq];
    }

    hash_value ^= SideToMove_hash[ pos.due_up() ];
    return hash_value;
}