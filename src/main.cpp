#include <fstream>
#include <iostream>
#include <string>
#include "../include/tape.h"

// /Users/ilolss/Desktop/yadro/tape_input.txt
// /Users/ilolss/Desktop/yadro/tape_output.txt

signed main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Paths of input and output files" << std::endl;
        return 1;
    }
    tape::tape input_tape = tape::tape(argv[1]);
    tape::tape output_tape = tape::tape(argv[2]);
    input_tape.sort(output_tape);
    return 0;
}
