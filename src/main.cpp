
#include <fstream>
#include <iostream>
#include <libconfig.h++>
#include <string>
#include "../include/tape.h"
#include "../include/tape_sort.h"

signed main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Paths of input and output files" << std::endl;
        return 1;
    }

    libconfig::Config cfg;

    try {
        cfg.readFile("config.cfg");
    } catch (const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while reading configuration file." << std::endl;
    } catch (const libconfig::ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << std::endl;
    }

    int sleep_time = 0;
    int numbers_length = 10;
    int max_numbers_in_memory = 10;

    try {
        sleep_time = cfg.lookup("configurations.sleep.milliseconds");
        // length of numbers without plus or minus
        numbers_length = cfg.lookup("configurations.content.number.length");
        max_numbers_in_memory =
            cfg.lookup("configurations.sort.max_numbers_in_memory");
    } catch (const libconfig::SettingNotFoundException &nfex) {
        std::cerr << "No configuration" << std::endl;
    }

    tape::tape input_tape = tape::tape(argv[1], numbers_length, sleep_time);
    tape::tape output_tape = tape::tape(argv[2], numbers_length, sleep_time);

    tape::tape_sort::sort(input_tape, output_tape, max_numbers_in_memory);

    return 0;
}
