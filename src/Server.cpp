#include "Server.h"
#include "Socket.h"
#include "Inetaddress.h"
#include "Epoll.h"
#include "Channel.h"
#include "test-util.h"
#include "Acceptor.h"
#include "Connection.h"
#include "Threadpool.h"

#include <assert.h>
#include <string.h>

Server::Server(Eventloop *_loop) : server_loop(_loop){
    server_accpetor = new Acceptor(_loop);
    std::function<void(Socket*, Inetaddress*)> cbfunc = std::bind(&Server::add_connection, this, std::placeholders::_1, std::placeholders::_2);
    server_accpetor->set_addconn_cb(cbfunc);
}

Server::~Server() {
    if(this->server_accpetor) 
        delete this->server_accpetor;
    for(auto p = connections_pool.begin(); p != connections_pool.end(); p++) {
        if(p->second) {
            delete p->second;
        }
    }
}

void Server::add_connection(Socket *conn_socket, Inetaddress *conn_addr) {
    Connection *temp_conn = new Connection(server_loop, conn_socket, conn_addr);
    std::function<void()> cbfunc = std::bind(&Server::del_connection, this, conn_socket);
    temp_conn->set_delconn_cb(cbfunc);
    connections_pool[conn_socket->getfd()] = temp_conn; 
} 


void Server::del_connection(Socket *conn_socket) {
    // connection fd closed in Socket class.
    Connection *temp_conn = connections_pool[conn_socket->getfd()];
    connections_pool.erase(conn_socket->getfd());
    delete temp_conn;    
}