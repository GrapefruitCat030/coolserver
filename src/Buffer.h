#pragma once

#include<string>

class Buffer {
private:
    /* data */
    std::string buf;
public:
    Buffer(/* args */);
    ~Buffer();

    void buf_append(char *msg, ssize_t msg_len);
    ssize_t buf_size();
    const char* c_str();
    void buf_clear();

    void buf_copy(Buffer *src_buf);
};
