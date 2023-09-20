#include "Channel.h"
#include "Epoll.h"


Channel::Channel(int _fd, uint32_t _evop) : \
                fd(_fd), evop(_evop){
                ready_evop = 0;
                ep         = nullptr;
                is_inep    = false;
}

bool Channel::isInEP(Epoll *ep) {
    return (this->ep == ep) && this->is_inep;    
}

void Channel::setInEP(Epoll *ep) {
    if(isInEP(ep)) return;
    else {
        this->ep = ep;
        this->is_inep = true;
    }
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


uint32_t Channel::updateEvop(uint32_t evop) {
    uint32_t old_op = getEvop();
    this->evop = evop;
    this->ep->channel_update(this);
    return old_op;
}

uint32_t Channel::setReop(uint32_t reop) {
    uint32_t old_op = getReop();
    this->ready_evop = reop;
    return old_op;
}

