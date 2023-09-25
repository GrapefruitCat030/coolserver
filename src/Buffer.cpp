#include "Buffer.h"


Buffer::Buffer(){

}

Buffer::~Buffer(){

}

void Buffer::buf_append(char *msg, ssize_t msg_len) {
    this->buf.append(msg, msg_len);
}

ssize_t Buffer::buf_size() {
    return this->buf.size();
} 

const char* Buffer::c_str() {
    return this->buf.c_str();
}

void Buffer::buf_clear() {
    this->buf.clear();
}


void Buffer::buf_copy(Buffer *src_buf) {
    this->buf_clear();
    this->buf = src_buf->buf; 
}
