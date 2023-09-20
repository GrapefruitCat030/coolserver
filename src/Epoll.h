#pragma once

#include<sys/epoll.h>
#include<vector>


#define MAX_EVENTS 100

class Channel;

class Epoll {
private:
    /* data */
    int eplist_fd;                      // The fd of ep instance. 
    struct epoll_event *ready_events;   // Ready list of ep instance. (为什么这样称呼？请看manual.)

public:
    Epoll();
    ~Epoll();

    // 将需要监听的文件描述符fd添加到轮询列表, evop为监听的事件种类
    void simple_add(int fd, uint32_t evop);

    // 将需要监听的文件描述符对应的Channel更新到轮询列表 (include add, modify)
    void channel_update(Channel *chan);
    


    /*
        轮询，然后返回发生的事件数组；
        其中事件数组长度为可操作的事件数目。
        重新返回一个数组而不是返回类成员ready_events，可以防止不可重入。 
    */
    std::vector<struct epoll_event> poll(int timeout = -1);
    std::vector<Channel *> chan_poll(int timeout = -1);
};



