#include "Eventloop.h"
#include "Epoll.h"
#include "Channel.h"


Eventloop::Eventloop() {
    loop_ep = new Epoll();
}
Eventloop::~Eventloop() {
    delete this->loop_ep;
}



void Eventloop::bootloop() {
    
    while (!loop_over) {
        std::vector<Channel *> active_channels = loop_ep->chan_poll();
        for(Channel *chan : active_channels) {
            chan->handleEvents();
        }
    }

}

void Eventloop::channel_update(Channel *chan) {
    loop_ep->channel_update(chan);
}

