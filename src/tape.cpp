
#include "../include/tape.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <libconfig.h++>
#include <string>
#include <thread>

namespace tape {

tape::tape(const std::string &file_path, const int size)
    : _sleep_time(0), _numbers_length(11) {
    libconfig::Config cfg;

    try {
        cfg.readFile("../config.cfg");
    } catch (const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while reading configuration file." << std::endl;
    } catch (const libconfig::ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << std::endl;
    }

    try {
        _sleep_time = cfg.lookup("configurations.sleep.milliseconds");
        _numbers_length = cfg.lookup("configurations.content.number.length");
    } catch (const libconfig::SettingNotFoundException &nfex) {
        std::cerr << "No configuration" << std::endl;
    }

    if (file_path == "" || !fs::exists(file_path)) {
        char filename_template[] = "/Users/ilolss/Desktop/yadro/tmp/XXXXXX";
        _file_path = mktemp(filename_template);
        _file_path += ".tmp";
        _stream.open(
            _file_path.string(),
            std::fstream::in | std::fstream::out | std::fstream::trunc
        );

        if (_stream.fail()) {
            std::cerr << "File not open" << std::endl;
        }

        _size = size;
        for (int i = 0; i < _size; i++) {
            _stream << tape::to_string(0) + "\n";
        }
        _stream.flush();
        _stream.seekg(0, std::ios::beg);
    } else {
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
};

void tape::next() {
    const int current_position = _stream.tellg();
    _stream.seekg(0, std::ios_base::end);
    if (current_position + 11 >= _stream.tellg()) {
        _stream.seekg(current_position);
        return;
    }
    _stream.seekg(current_position + 11);
}

void tape::prev() {
    if (_stream.tellg() < _numbers_length) {
        _stream.seekg(0, std::ios::beg);
        return;
    }
    _stream.seekg(-11, std::ios::cur);
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
    const std::string tmp_string = std::to_string(number);
    const int size = tmp_string.size();
    return std::string(_numbers_length - 1 - size, '0') + tmp_string;
}

int tape::to_int(const std::string &str) const {
    if (!std::any_of(str.begin(), str.end(), ::isdigit)) {
        return 0;
    }
    return stoi(str);
}

void tape::sort(tape &out_tape) {
    const int sz = this->_size;
    this->go_begin();  // cursor to begin
    tape tmp1 = tape(sz);
    tape tmp2 = tape(sz);
    tape tmp3 = tape(sz);
    tape tmp4 = tape(sz);
    // copy this to all tmp;
    for (int i = 0; i < sz; i++) {
        const int cur = this->read();
        tmp1.write(cur);
        tmp2.write(cur);
        tmp3.write(cur);
        tmp4.write(cur);
        this->next();
        tmp1.next();
        tmp2.next();
        tmp3.next();
        tmp4.next();
    }
    // cursor to begin
    this->go_begin();
    tmp1.go_begin();
    tmp2.go_begin();
    tmp3.go_begin();
    tmp4.go_begin();
    // merge sort
    int k = 0;
    while (true) {
        if ((1 << k) > sz) {
            if (k - 1 % 2) {
                // copy tmp3 to out_tape
                tmp3.go_begin();
                out_tape.go_begin();
                for (int i = 0; i < sz; i++) {
                    const int cur = tmp3.read();
                    out_tape.write(cur);
                    tmp3.next();
                    out_tape.next();
                }
            } else {
                // copy tmp1 to out_tape
                tmp1.go_begin();
                out_tape.go_begin();
                for (int i = 0; i < sz; i++) {
                    const int cur = tmp1.read();
                    out_tape.write(cur);
                    tmp1.next();
                    out_tape.next();
                }
            }
            break;
        }
        if (k % 2) {
            tape::merge(tmp1, tmp2, tmp3, tmp4, sz, k);
        } else {
            tape::merge(tmp3, tmp4, tmp1, tmp2, sz, k);
        }
        k++;
    }
    this->go_begin();
    out_tape.go_begin();
}

void tape::merge(
    tape &from1,
    tape &from2,
    tape &to1,
    tape &to2,
    const int sz,
    const int k
) {
    from1.go_begin();
    from2.go_begin();
    to1.go_begin();
    to2.go_begin();

    const int len = (1 << (k + 1));
    int cnt_blocks = sz / len;
    if (sz % len != 0) {
        cnt_blocks++;
    }
    // int t = 0;
    // int i = 0;
    // int j = (1 << k);
    from2.go_next(1 << k);
    for (int block = 0; block < cnt_blocks; block++) {
        const int beg = block * len;
        int beg_i = beg;
        int beg_j = beg + (1 << k);
        int cnt_i = 0;
        int cnt_j = 0;
        for (int q = 0; q < len; q++) {
            if ((cnt_i == (1 << k) || beg_i + cnt_i >= sz) &&
                (cnt_j == (1 << k) || beg_j + cnt_j >= sz)) {
                break;
            }
            if (cnt_i == (1 << k) || beg_i + cnt_i >= sz) {
                // in1[t] = out2[j];
                // in2[t] = out2[j];
                // j++;
                const int tmp = from2.read();
                to1.write(tmp);
                to2.write(tmp);
                from2.next();
                cnt_j++;
                // t++;
                to1.next();
                to2.next();
                continue;
            }
            if (cnt_j == (1 << k) || beg_j + cnt_j >= sz) {
                // in1[t] = out1[i];
                // in2[t] = out1[i];
                // i++;
                const int tmp = from1.read();
                to1.write(tmp);
                to2.write(tmp);
                from1.next();
                cnt_i++;
                // t++;
                to1.next();
                to2.next();
                continue;
            }
            const int cur_from1 = from1.read();
            const int cur_from2 = from2.read();
            if (cur_from1 < cur_from2) {
                // in1[t] = out1[i];
                // in2[t] = out1[i];
                // i++;
                to1.write(cur_from1);
                to2.write(cur_from1);
                from1.next();
                cnt_i++;
            } else {
                // in1[t] = out2[j];
                // in2[t] = out2[j];
                // j++;
                to1.write(cur_from2);
                to2.write(cur_from2);
                from2.next();
                cnt_j++;
            }
            // t++;
            to1.next();
            to2.next();
        }
        // i += (1 << k);
        // j += (1 << k);
        from1.go_next(1 << k);
        from2.go_next(1 << k);
    }

    from1.go_begin();
    from2.go_begin();
    to1.go_begin();
    to2.go_begin();
}
}  // namespace tape
