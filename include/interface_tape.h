#ifndef INTERFACE_TAPE_
#define INTERFACE_TAPE_

namespace tape {
class ITape {
public:
    virtual void next() = 0;
    virtual void prev() = 0;
    virtual void go_next(const int cnt_step) = 0;
    virtual void go_prev(const int cnt_step) = 0;
    virtual void go_begin() = 0;
    virtual void go_end() = 0;
    virtual int read() = 0;
    virtual void write(const int number) = 0;
};
}  // namespace tape

#endif  // INTERFACE_TAPE_
