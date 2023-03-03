
#include "../include/tape_sort.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <utility>
#include <vector>
#include "../include/tape.h"

namespace fs = std::filesystem;

namespace tape {
void tape_sort::merge(
    const std::vector<std::unique_ptr<tape>> &tapes,
    tape &output_tape
) {
    std::set<std::pair<int, int>> queue;  // {number, index in tapes}
    for (int i = 0; i < tapes.size(); i++) {
        tapes[i]->go_begin();
        queue.insert(std::make_pair(tapes[i]->read(), i));
    }
    while (!queue.empty()) {
        const int cur = queue.begin()->first;
        const int ind = queue.begin()->second;
        queue.erase(queue.begin());
        output_tape.write(cur);
        output_tape.next();
        if (tapes[ind]->next()) {
            queue.insert(std::make_pair(tapes[ind]->read(), ind));
        }
    }
}

fs::path tape_sort::create_file(const size_t size, const int number_length) {
    fs::create_directory("tmp");
    char filename_template[] = "tmp/XXXXXX";
    fs::path file_path = mktemp(filename_template);
    file_path += ".tmp";
    std::fstream stream(
        file_path.string(),
        std::fstream::in | std::fstream::out | std::fstream::trunc
    );

    if (stream.fail()) {
        std::cerr << "File not open" << std::endl;
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < number_length + 1; j++) {  // + 1 for plus or minus
            stream << "0";
        }
        stream << "\n";
    }
    stream.flush();
    stream.seekg(0, std::ios::beg);
    stream.close();
    return file_path;
}

void tape_sort::sort(
    tape &input_tape,
    tape &output_tape,
    const int numbers_in_memory
) {
    input_tape.go_begin();
    const int tape_size = input_tape.size();
    const int number_length = input_tape.number_length();
    if (input_tape.number_length() != output_tape.number_length()) {
        std::cerr << "Number length is not equal\n";
        return;
    }
    if (input_tape.size() != output_tape.size()) {
        std::cerr << "Tapes size is not equal\n";
        return;
    }
    int i = 0;
    const int cur_numbers_in_file = 0;
    std::vector<std::unique_ptr<tape>> tapes;
    while (i < tape_size) {
        std::vector<int> tmp_vector;
        tmp_vector.reserve(numbers_in_memory);
        for (int j = 0; j < numbers_in_memory; j++) {
            if (i >= tape_size) {
                break;
            }
            const int cur = input_tape.read();
            tmp_vector.push_back(cur);
            input_tape.next();
            i++;
        }
        std::sort(tmp_vector.begin(), tmp_vector.end());
        tapes.emplace_back(std::make_unique<tape>(
            create_file(tmp_vector.size(), number_length), number_length
        ));
        for (const int j : tmp_vector) {
            tapes.back()->write(j);
            tapes.back()->next();
        }
    }

    // k-merge
    merge(tapes, output_tape);

    // delete tmp directory
    fs::remove_all("tmp");
}
}  // namespace tape
