#include "Server.h"
#include "Socket.h"
#include "Inetaddress.h"
#include "Epoll.h"
#include "Channel.h"
#include "test-util.h"
#include "Acceptor.h"

#include <assert.h>
#include <string.h>

Server::Server(Eventloop *_loop) : server_loop(_loop){
    server_accpetor = new Acceptor(_loop);
    std::function<void()> acptor_cb  = std::bind(&Server::accpet_connection, this, server_accpetor->get_sersocket());
    server_accpetor->set_acptconn_cb(acptor_cb);
}

Server::~Server() {
    delete this->server_loop;
    delete this->server_accpetor;
}

void Server::accpet_connection(Socket *server_socket) {
    Inetaddress *conn_addr = new Inetaddress();               // TODO: Memory leak
    int conn_fd = server_socket->s_accept(conn_addr); 
    Socket *conn_socket = new Socket(conn_fd); // TODO: Memory leak
    conn_socket->s_setnonblocking();
    
    printf("new client fd %d! IP: %s Port: %d\n",\
            conn_fd, inet_ntoa(conn_addr->sock_addr.sin_addr), ntohs(conn_addr->sock_addr.sin_port));

    // 将connection socket注册进入loop请求队列，以Channel形式
    Channel *conn_chan = new Channel(conn_socket->getfd(), server_loop);
    std::function<void()> handle_cbfunc = std::bind(&Server::reading_handle, this, conn_fd);
    conn_chan->setCallbackfunc(handle_cbfunc);
    conn_chan->watchReadingET();
}

void Server::reading_handle(int conn_sockfd) {

    char rd_buff[1024], wr_buff[1024];
    while(true) {
        memset(rd_buff, 0, sizeof(rd_buff));
        memset(wr_buff, 0, sizeof(wr_buff));
        
        ssize_t read_bytes = recv(conn_sockfd, rd_buff, sizeof(rd_buff), 0);

        if(read_bytes > 0) {
            // process the data normally.
            printf("client %d: %s\n", conn_sockfd, rd_buff);
            memcpy(wr_buff, rd_buff, sizeof(wr_buff)); 
            ssize_t write_bytes = send(conn_sockfd, wr_buff, sizeof(wr_buff), 0);
            if(write_bytes == -1) {
                errif(true, "write buffer");
                break;
            }
        } else if (read_bytes == 0) {
            // EOF, representing for the client's disconnection. 
            printf("the client %d disconnected.\n", conn_sockfd);
            close(conn_sockfd);
            break;
        } else if (read_bytes == -1){
            /*
                Within ET mode, the handle function should has a while loop with errno resolve.
                Because the non-blocking socket will send the error signal rather than blocking 
                when the current receiving is over.
            */
            if(errno == EINTR) {
                // the client intrrupt normally.
                continue;
            } else if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // the client finished its sending, or the receiving is over.
                break;
            }
        } else {
            assert(0);
        }
    }
}
