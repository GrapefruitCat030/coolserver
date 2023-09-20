#include "Channel.h"
#include "Epoll.h"


Channel::Channel(int _fd, Epoll *_ep): \
                fd(_fd), ep(_ep) {
                evop       = 0;
                ready_evop = 0;
                is_inep    = false;
}

uint32_t Channel::updateEvop(uint32_t evop) {
    uint32_t old_op = getEvop();
    this->evop = evop;
    this->ep->channel_update(this);
    return old_op;
}

bool Channel::isInEP() {
    return is_inep;    
}

void Channel::setInEP() {
    is_inep = true;
}


int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvop() {
    return evop;
}

uint32_t Channel::getReop() {
    return ready_evop;
}


uint32_t Channel::setReop(uint32_t reop) {
    uint32_t old_op = getReop();
    this->ready_evop = reop;
    return old_op;
}


void Channel::watchReadingLT() {
    updateEvop(EPOLLIN);
}

void Channel::watchReadingET() {
    updateEvop(EPOLLIN | EPOLLET);
}
