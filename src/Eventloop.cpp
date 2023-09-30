#include "Eventloop.h"
#include "Epoll.h"
#include "Channel.h"
#include "Threadpool.h"
#include<functional>

Eventloop::Eventloop() : loop_over(false) {
    loop_ep = new Epoll();
    threads_pool = new Threadpool();
}
Eventloop::~Eventloop() {
    delete this->loop_ep;
}

void Eventloop::setover() { loop_over = true;}

void Eventloop::bootloop() {
    
    while (!loop_over) {
        std::vector<Channel *> active_channels = loop_ep->chan_poll();
        for(Channel *chan : active_channels) {
            chan->handle_events();
        }
    }

}

void Eventloop::channel_update(Channel *chan) {
    loop_ep->channel_update(chan);
}

void Eventloop::create_task(std::function<void()> cb) {
    threads_pool->enq_task(cb);
}