#pragma once

/*
    Channel类作为一个对 fd + interested event + callback function 的封装，
    在epoll轮询完毕时返回的不再是简陋的、只能携带fd的events list(处理过程僵硬繁杂)
    而是一个Channel数组，server可以根据各个Channel代表的文件描述符的事件需求，
    调用fd对应的callback function 进行业务处理。

*/

#include<unistd.h>
#include<stdint.h>


class Epoll;

class Channel {
private:
    /* Data */
    int fd;
    uint32_t evop;
    uint32_t ready_evop;
    
    Epoll *ep;
    bool   is_inep;

    // set new events option and return the old option. 
    // TODO:设置完后应该使用epoll的chan_update更新吗？
    uint32_t updateEvop(uint32_t evop);

public:
    Channel(int fd, Epoll *ep);
    ~Channel();

    int getFd();

    uint32_t getEvop();
    uint32_t getReop();

    // set new ready events option and return the old option.
    uint32_t setReop(uint32_t reop);

    bool isInEP();
    void setInEP(); 
    
    void watchReadingLT();
    void watchReadingET();



    void *cb_func();
};