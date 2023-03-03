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

public:
    tape(
        const std::string &file_path,
        const int numbers_length,
        const int sleep_time
    );

    tape(const std::string &file_path, const int numbers_length)
        : tape(file_path, numbers_length, 0){};

    ~tape() {
        _stream.close();
    };

    int size() const {
        return _size;
    };

    int number_length() const {
        return _numbers_length - 2;
    }

    bool next() override final;
    bool prev() override final;
    void go_next(const int cnt_step) override final;
    void go_prev(const int cnt_step) override final;
    void go_begin() override final;
    void go_end() override final;
    int read() override final;
    void write(const int number) override final;
};
}  // namespace tape

#endif  // TAPE_
