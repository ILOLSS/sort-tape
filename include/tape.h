#ifndef TAPE_
#define TAPE_

#include <filesystem>
#include <fstream>
#include <iostream>
#include "interface_tape.h"

namespace fs = std::filesystem;

namespace tape {
class tape : public ITape {
private:
    fs::path _file_path;
    std::fstream _stream;
    int _sleep_time;
    int _numbers_length;
    int _size;

    std::string to_string(const int number) const;
    int to_int(const std::string &str) const;
    void merge(
        tape &from1,
        tape &from2,
        tape &to1,
        tape &to2,
        const int sz,
        const int k
    );

public:
    tape(const std::string &file_path, const int size);

    tape(const std::string &file_path) : tape(file_path, 0){};

    tape(const int size) : tape("", size){};

    ~tape() {
        _stream.close();
        if (_file_path.extension() == ".tmp") {
            fs::remove(_file_path);
        }
    };

    int size() const {
        return _size;
    };

    void next() override final;
    void prev() override final;
    void go_next(const int cnt_step) override final;
    void go_prev(const int cnt_step) override final;
    void go_begin() override final;
    void go_end() override final;
    int read() override final;
    void write(const int number) override final;
    void sort(tape &out_tape);
};
}  // namespace tape

#endif  // TAPE_
