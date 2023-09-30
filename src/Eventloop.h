#pragma once
#include<functional>

/*
    Eventloop类的作用：
    封装底层的Epoll类；
    作为服务器事件驱动队列的具体逻辑（bootloop函数）；
    给Channel类提供事件注册、更新的接口
*/

class Epoll;
class Channel;
class Threadpool;

class Eventloop {
private:
    /* data */
    Epoll *loop_ep;
    bool   loop_over;
    // Threads pool
    Threadpool *threads_pool;
public:
    Eventloop();
    ~Eventloop();

    void setover();
    void bootloop();
    
    // 注意这个update函数并不由Eventloop类在外头亲自调用，
    // 而是由Channel在各种注册行为时进行调用。
    void channel_update(Channel *);

    // 将Channel上的事件放到线程池处理
    void create_task(std::function<void()> cb);
};
