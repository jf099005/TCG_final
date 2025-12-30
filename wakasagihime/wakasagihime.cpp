// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"

#include"AB_agent.h"
#include <chrono>
#include<fstream>
#include<filesystem>
#include<iomanip>
#define RECORD_GAMES 1
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

#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>


#ifdef RECORD_GAMES
// acknowledge the help of Chatgpt
std::string generate_timestamped_record_path(const std::string& base_path)
{
    namespace fs = std::filesystem;

    fs::path base(base_path);
    fs::path parent = base.parent_path();
    std::string ext = base.extension().string();

    // 取得目前時間
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif

    // 格式化時間字串
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");

    std::string timestamp = oss.str();
    // std::string stem = "record_" + timestamp;
    std::string stem = base.stem().string() + timestamp;

    fs::path candidate = parent / (stem + ext);

    // 若剛好同名（同秒啟動），加上 _2, _3...
    if (!fs::exists(candidate))
        return candidate.string();

    for (int idx = 2; ; ++idx) {
        fs::path numbered =
            parent / (stem + "_" + std::to_string(idx) + ext);

        if (!fs::exists(numbered))
            return numbered.string();
    }
}
#endif

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

    if(n_diff > 1){
        debug << "Error in get_move function!\n";
        debug << "Previous Position:\n" << pos_prv << std::endl;
        debug << "Current Position:\n" << pos_cur << std::endl;
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

    // std::string algorithm_name = "pure_negamax";
    #ifdef NEGASCOUT
    std::string algorithm_name = "negascout";
    #else
    std::string algorithm_name = "pure_negamax";
    #endif
    
    #ifdef RECORD_GAMES
    std::string basic_record_path = "/home/course/select/b11201024/Desktop/TCG2025/TCG_final/wakasagihime/record/" + algorithm_name + ".txt";
    std::string game_record_path = generate_timestamped_record_path(basic_record_path);
    #endif

    Position prv_pos;
    std::ofstream record_ofs;
    record_ofs.open(game_record_path);

    bool is_first_board = true;


    int estimated_endgame_moves = 100;

    while (std::getline(std::cin, line)) {

        Position pos(line);


        // MoveList moves(pos);
        auto start = std::chrono::high_resolution_clock::now();

        if(pos.count(FACE_UP) == 32 || pos.count(Hidden) == 32){
            debug << " ===========a new game=============\n";
            current_step = 0;
            remain_moves = maximum_static_moves;
        }
        else if(is_first_board){
            debug << " ===========a new game=============\n";
            record_ofs << "====================New Game=================\n\n\n";
            current_step = 0;
            remain_moves = maximum_static_moves;
            is_first_board = false;
            if(pos.count(FACE_UP) != 0){
                for(Square sq: BoardView(pos.pieces(FACE_UP))){
                    acdc.flipping_piece( pos.peek_piece_at(sq) );
                }
            }
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
                remain_moves = maximum_static_moves;
            }
            else{
                if(prv_pos.peek_piece_at(prv_move.to()).type != NO_PIECE){
                    remain_moves = maximum_static_moves;
                }
            }
        }

        is_first_board = false;

        #ifdef RECORD_GAMES
        record_ofs << "@ step " << current_step <<'\n';
        record_ofs << pos;
        record_ofs <<"\t" << pos.toFEN()<<"\n\n";

        record_ofs << "\t remain static moves:" << remain_moves <<std::endl;

        #endif

        debug << pos << std::endl;
        debug << "\t remain time:" << pos.time_left() <<std::endl;
        debug << "\t remain moves:" << remain_moves <<std::endl;
        
        if(pos.time_left() < 0){
            prv_pos = pos;
            remain_moves--;
            current_step++;
            estimated_endgame_moves--;
            continue;
        }

        double time_min = 1.0, time_max = 1.0;

        // if(current_step >= 10 && current_step <= 50){
        //     time_max = 10.0;
        // }

        double remain_time = pos.time_left() / 1000;
        double move_times = remain_time / double(estimated_endgame_moves);
        move_times = std::min(move_times, 10.0);

        time_max = move_times;

        debug << "time_max for this move: " << time_max << '\n';

        Move opt = acdc.opt_solution(pos, time_min, time_max, 6, remain_moves);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        #ifdef RECORD_GAMES
        record_ofs << "depth: " << acdc.max_visited_depth << '\n';
        record_ofs << "finished branch:" << acdc.finished_branch << '\n';
        record_ofs << "optimal move:" << opt;
        record_ofs << "correct prediction:" << acdc.correct_prediction << '\n';
        record_ofs << "failed prediction:" << acdc.fail_prediction << "\n";
        record_ofs << "success rate: " << std::fixed << std::setprecision(3) << double(acdc.correct_prediction) / \
                            double(acdc.correct_prediction + acdc.fail_prediction) << '\n';
        
        record_ofs << "time:" << double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den << std::endl;
        record_ofs << "visited positions:" << acdc.visited_states <<'\n';
        record_ofs << "visited criticals:" << acdc.visited_critical_states << '\n';
        #endif

        debug << "time:" << double(duration.count())*std::chrono::microseconds::period::num/std::chrono::microseconds::period::den << std::endl;


        face_up_pieces = pos.count(FACE_UP);
        remain_moves --;
        estimated_endgame_moves--;
        current_step++;
        prv_pos = pos;
        info << opt;
        if(estimated_endgame_moves <= 15){
            estimated_endgame_moves += 10;
        }

    }
}
