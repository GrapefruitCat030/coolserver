#include "Channel.h"
#include "Epoll.h"
#include "Eventloop.h"

Channel::Channel(int _fd, Eventloop *_loop): \
                fd(_fd), loop(_loop),\
                evop(0), ready_evop(0), \
                inep_flag(false),\
                use_tpool(false) {
}
Channel::~Channel() {}

uint32_t Channel::update_Evop(uint32_t evop) {
    uint32_t old_op = get_Evop();
    this->evop = old_op | evop;
    loop->channel_update(this);
    return old_op;
}


bool Channel::is_inEP()  { return inep_flag; }
int Channel::get_fd() { return fd;}
uint32_t Channel::get_Evop() { return evop;}
uint32_t Channel::get_Reop() { return ready_evop;}

/* Setting something. */

void Channel::set_inEP() { inep_flag = true; }

uint32_t Channel::set_Reop(uint32_t reop) {
    uint32_t old_op = get_Reop();
    this->ready_evop = reop;
    return old_op;
}

void Channel::set_use_tpool(bool flag) {
    use_tpool = flag;
}

/* Set the callback function. */

void Channel::set_callbackfunc(std::function<void()> cb) {
    callback_func = cb;
}
void Channel::handle_events() {
    // callback_func();   
    if(use_tpool)
        loop->create_task(callback_func);
    else 
        callback_func();
}

/* Register the events. */

void Channel::register_readingLT() { 
    update_Evop(EPOLLIN);
}
void Channel::register_readingET() {
    update_Evop(EPOLLIN | EPOLLET);
}
