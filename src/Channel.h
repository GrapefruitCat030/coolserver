#pragma once

/*
    Channel类作为一个对 fd + interested event + callback function 的封装，
    在epoll轮询完毕时返回的不再是简陋的、只能携带fd的events list(处理过程僵硬繁杂)
    而是一个Channel数组，server可以根据各个Channel代表的文件描述符的事件需求，
    调用fd对应的callback function 进行业务处理。

    注册过程包括：
    1.初始化：fd和loop的指定；
    2.回调函数绑定
    3.具体事件（读准备、写准备）注册

    Channel的注册在：
    Server类实例化时（listen socket）
    Listen socket 在 poll loop 中对客户端请求调用对应的回调函数（ accept ）时； 

*/

#include<unistd.h>
#include<stdint.h>
#include<functional>

class Eventloop;
class Epoll;

class Channel {
private:
    /* Data */
    int fd;
    Eventloop *loop;
    uint32_t evop;
    uint32_t ready_evop;
    std::function<void()> callback_func; // 函数更新于注册时

    bool inep_flag;

    // set new events option and return the old option. 
    uint32_t update_Evop(uint32_t evop);

    // use threads pool or not
    bool use_tpool;
public:
    Channel(int fd, Eventloop *loop);
    ~Channel();


    int get_fd();
    bool is_inEP();
    uint32_t get_Evop();
    uint32_t get_Reop();

    // Used by epoll function. Inform the ready events option and return the old option.
    uint32_t set_Reop(uint32_t reop);
    void set_inEP(); 
    void set_use_tpool(bool flag);
    
    void set_callbackfunc(std::function<void()> cb);    
    void handle_events();

    /* Register the events. */
    // 注册LT读准备事件
    void register_readingLT();  
    // 注册ET读准备事件
    void register_readingET(); 
};
