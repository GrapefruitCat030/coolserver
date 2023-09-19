#include "Epoll.h"
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


void Epoll::ctl_add(int fd, uint32_t evop = EPOLLIN) {
    struct epoll_event ev;   
    memset(&ev, 0, sizeof(ev));
    ev.events  = evop;       // 默认为可读事件
    ev.data.fd = fd;

    errif(epoll_ctl(eplist_fd, EPOLL_CTL_ADD, fd, &ev) == -1, \
            "epoll addfd");
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



