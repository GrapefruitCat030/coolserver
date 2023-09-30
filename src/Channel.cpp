#include "Channel.h"
#include "Epoll.h"
#include "Eventloop.h"

Channel::Channel(int _fd, Eventloop *_loop): \
                fd(_fd), loop(_loop) {
                evop       = 0;
                ready_evop = 0;
                inep_flag  = false;
}
Channel::~Channel() {}

uint32_t Channel::update_Evop(uint32_t evop) {
    uint32_t old_op = get_Evop();
    this->evop = evop;
    loop->channel_update(this);
    return old_op;
}

bool Channel::is_inEP()  { return inep_flag; }
void Channel::set_inEP() { inep_flag = true; }

int Channel::get_fd() { return fd;}

uint32_t Channel::get_Evop() { return evop;      }
uint32_t Channel::get_Reop() { return ready_evop;}

uint32_t Channel::set_Reop(uint32_t reop) {
    uint32_t old_op = get_Reop();
    this->ready_evop = reop;
    return old_op;
}

/* Set the callback function. */

void Channel::set_callbackfunc(std::function<void()> cb) {
    callback_func = cb;
}
void Channel::handle_events() {
    // callback_func();   
    loop->create_task(callback_func);
}

/* Register the events. */

void Channel::watch_readingLT() { 
    update_Evop(EPOLLIN);
}
void Channel::watch_readingET() {
    update_Evop(EPOLLIN | EPOLLET);
}
