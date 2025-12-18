#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include<fstream>
#include"AB_agent.h"

int main(int argc, char *argv[]){
    // assert(argc == 1);
    std::string filepath = argv[1];
    std::cout << "filepath:" << filepath << std::endl;
    std::ofstream ofs;
    ofs.open(filepath);

    

    for(int side = Black; side <=Red ; side++){
        for(int piecetype=General; piecetype <= Soldier; piecetype++){
            // Board()
        }
    }

}