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
    acpt_chan = new Channel(server_socket->getfd(), server_loop);
    std::function<void()> chan_tempcb = std::bind(&Acceptor::accept_connection, this);
    acpt_chan->set_callbackfunc(chan_tempcb);
    acpt_chan->watch_readingLT(); 
}
Acceptor::~Acceptor() {
    delete this->server_socket;
    delete this->server_addr;
    delete this->acpt_chan;
}

Socket*      Acceptor::get_sersocket(){ return this->server_socket; }
Inetaddress* Acceptor::get_inetaddr() { return this->server_addr; }


void Acceptor::set_addconn_cb(std::function<void(Socket*, Inetaddress*)> cb) {
    add_connection_cb = cb;
}

void Acceptor::accept_connection() {
    
    // Acceptor只负责accept创建socket，Connection建立由Server类负责
    
    Inetaddress *conn_addr = new Inetaddress();               // TODO: Memory leak
    Socket    *conn_socket = new Socket(server_socket->s_accept(conn_addr)); // TODO: Memory leak
    conn_socket->s_setnonblocking();
    
    printf("new client fd %d! IP: %s Port: %d\n",\
            conn_socket->getfd(), inet_ntoa(conn_addr->sock_addr.sin_addr), ntohs(conn_addr->sock_addr.sin_port));
    
    // 调用上层管理类Server的add_connection函数
    this->add_connection_cb(conn_socket, conn_addr);
}


