#ifndef TAPE_SORT_
#define TAPE_SORT_

#include <filesystem>
#include "../include/tape.h"

namespace fs = std::filesystem;

namespace tape {
class tape_sort {
private:
    static void
    merge(const std::vector<std::unique_ptr<tape>> &tapes, tape &output_tape);
    static fs::path create_file(const size_t size, const int number_length);

public:
    static void
    sort(tape &input_tape, tape &output_tape, const int numbers_in_memory);
};
}  // namespace tape

#endif  // TAPE_SORT_
