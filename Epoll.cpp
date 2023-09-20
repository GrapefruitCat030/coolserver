#include "Epoll.h"
#include "Channel.h"
#include "test-util.h"
#include<string.h>
#include<unistd.h>


Epoll::Epoll() {
    eplist_fd = epoll_create1(0);
    errif(eplist_fd == -1, \
            "epoll fd inital");

    ready_events = new struct epoll_event[MAX_EVENTS];
    errif(ready_events == nullptr, \
           "epoll events inital");
    memset(ready_events, 0, sizeof(struct epoll_event) * MAX_EVENTS);

}

Epoll::~Epoll(){
    if(eplist_fd != -1) {
        close(eplist_fd);
        eplist_fd = -1;
    }
    delete[] ready_events;
}


void Epoll::simple_add(int fd, uint32_t evop = EPOLLIN) {
    struct epoll_event ev;   
    memset(&ev, 0, sizeof(ev));
    ev.events  = evop;       // 默认为可读事件
    ev.data.fd = fd;

    errif(epoll_ctl(eplist_fd, EPOLL_CTL_ADD, fd, &ev) == -1, \
            "epoll addfd");
}

void Epoll::channel_update(Channel *chan) {

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events   = chan->getEvop();
    ev.data.ptr = (void *)chan; 

    // The channel is in this eplist. modify it.    
    if(chan->isInEP(this)) {
        errif(epoll_ctl(eplist_fd, EPOLL_CTL_MOD, chan->getFd(), &ev) == -1, \
                "epoll modchannel");
    }
    // Else, add the channel to the eplist.
    else {
        errif(epoll_ctl(eplist_fd, EPOLL_CTL_ADD, chan->getFd(), &ev) == -1, \
                "epoll addchannel");
        chan->setInEP(this);
    } 
}

std::vector<struct epoll_event> Epoll::poll(int timeout) {
    std::vector<struct epoll_event> active_events;
    int nfds = epoll_wait(eplist_fd, ready_events, MAX_EVENTS, timeout);
    errif(nfds == -1, \
            "epoll wait");
    for(int i = 0; i < nfds; i++)
        active_events.push_back(ready_events[i]);
    return active_events;
}

std::vector<Channel *> Epoll::chan_poll(int timeout) {
    std::vector<Channel *> active_channels;
    int nfds = epoll_wait(eplist_fd, ready_events, MAX_EVENTS, timeout);
    errif(nfds == -1, \
            "epoll wait");
    for(int i = 0; i < nfds; i++) {
        Channel *temp = (Channel *)ready_events[i].data.ptr;
        temp->setReop(ready_events[i].events); 
        active_channels.push_back(temp);
    }
    return active_channels;

}


