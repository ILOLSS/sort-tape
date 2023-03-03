
#include "../include/tape.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

namespace tape {

tape::tape(
    const std::string &file_path,
    const int numbers_length,
    const int sleep_time
)
    : _sleep_time(sleep_time),
      _numbers_length(numbers_length + 2) {  // + 2 for "\n" and plus or minus
    if (file_path.empty() || !fs::exists(file_path)) {
        std::cerr << "Not file" << std::endl;
    }

    _file_path = file_path;
    _stream.open(_file_path.string(), std::fstream::in | std::fstream::out);

    if (_stream.fail()) {
        std::cerr << "File not open" << std::endl;
    }

    _size = 0;
    std::string tmp_string;
    while (getline(_stream, tmp_string)) {
        _size++;
    }
    _stream.clear();
    _stream.seekg(0, std::ios::beg);
}

bool tape::next() {
    std::this_thread::sleep_for(std::chrono::milliseconds(tape::_sleep_time));
    const long long current_position = _stream.tellg();
    _stream.seekg(0, std::ios_base::end);
    if (current_position + _numbers_length >= _stream.tellg()) {
        _stream.seekg(current_position);
        return false;
    }
    _stream.seekg(current_position + _numbers_length);
    return true;
}

bool tape::prev() {
    std::this_thread::sleep_for(std::chrono::milliseconds(tape::_sleep_time));
    if (_stream.tellg() < _numbers_length) {
        _stream.seekg(0, std::ios::beg);
        return false;
    }
    _stream.seekg(-_numbers_length, std::ios::cur);
    return true;
}

void tape::go_next(const int cnt_step) {
    for (int i = 0; i < cnt_step; i++) {
        next();
    }
}

void tape::go_prev(const int cnt_step) {
    for (int i = 0; i < cnt_step; i++) {
        prev();
    }
}

void tape::go_begin() {
    tape::go_prev(_size);
}

void tape::go_end() {
    tape::go_next(_size);
}

int tape::read() {
    std::this_thread::sleep_for(std::chrono::milliseconds(tape::_sleep_time));
    std::string str;
    getline(tape::_stream, str);
    _stream.clear();
    _stream.seekg(-tape::_numbers_length, std::ios::cur);
    return to_int(str);
}

void tape::write(const int number) {
    std::this_thread::sleep_for(std::chrono::milliseconds(tape::_sleep_time));
    tape::_stream << tape::to_string(number);
    _stream.flush();
    _stream.seekg(-tape::_numbers_length + 1, std::ios::cur);
}

std::string tape::to_string(const int number) const {
    const bool is_negative = number < 0;
    int temp = number;
    if (is_negative) {
        temp = -temp;
    }
    const std::string tmp_string = std::to_string(temp);
    const size_t size = tmp_string.size();
    if (is_negative) {
        return "-" + std::string(_numbers_length - 2 - size, '0') + tmp_string;
    }
    return std::string(_numbers_length - 1 - size, '0') + tmp_string;
}

int tape::to_int(const std::string &str) const {
    if (!std::any_of(str.begin(), str.end(), ::isdigit)) {
        return 0;
    }
    return stoi(str);
}
}  // namespace tape
