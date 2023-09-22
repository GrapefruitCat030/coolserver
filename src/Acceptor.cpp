#include "Acceptor.h"
#include "Eventloop.h"
#include "Socket.h"
#include "Inetaddress.h"
#include "Channel.h"


Acceptor::Acceptor(Eventloop *_loop): server_loop(_loop) {

    server_socket = new Socket();
    server_addr   = new Inetaddress("127.0.0.1", 9999); 
    server_socket->s_bind(server_addr);
    server_socket->s_listen();

    // 将listen socket注册进入loop请求队列，以Channel形式
    Channel *acpt_chan = new Channel(server_socket->getfd(), server_loop);
    std::function<void()> chan_tempcb = std::bind(&Acceptor::accept_connection, this);
    acpt_chan->setCallbackfunc(chan_tempcb);
    acpt_chan->watchReadingLT(); 
}
Acceptor::~Acceptor() {
    delete this->server_addr;
    delete this->server_socket;
}

Socket*      Acceptor::get_sersocket(){ return this->server_socket; }
Inetaddress* Acceptor::get_inetaddr() { return this->server_addr; }


void Acceptor::set_acptconn_cb(std::function<void()> cb) {
    accept_connection_cb = cb;
}

void Acceptor::accept_connection() {
    this->accept_connection_cb();
}


