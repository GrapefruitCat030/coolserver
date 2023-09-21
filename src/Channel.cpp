#include "Channel.h"
#include "Epoll.h"
#include "Eventloop.h"

Channel::Channel(int _fd, Eventloop *_loop): \
                fd(_fd), loop(_loop) {
                evop       = 0;
                ready_evop = 0;
                is_inep    = false;
}

uint32_t Channel::updateEvop(uint32_t evop) {
    uint32_t old_op = getEvop();
    this->evop = evop;
    loop->channel_update(this);
    return old_op;
}

bool Channel::isInEP()  { return is_inep; }
void Channel::setInEP() { is_inep = true; }

int Channel::getFd() { return fd;}

uint32_t Channel::getEvop() { return evop;      }
uint32_t Channel::getReop() { return ready_evop;}

uint32_t Channel::setReop(uint32_t reop) {
    uint32_t old_op = getReop();
    this->ready_evop = reop;
    return old_op;
}

/* Set the callback function. */

void Channel::setCallbackfunc(std::function<void()> cb) {
    callback_func = cb;
}
void Channel::handleEvents() {
    callback_func();   
}

/* Register the events. */

void Channel::watchReadingLT() { 
    updateEvop(EPOLLIN);
}
void Channel::watchReadingET() {
    updateEvop(EPOLLIN | EPOLLET);
}
