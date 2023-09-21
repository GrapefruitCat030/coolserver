#include "Server.h"
#include "Socket.h"
#include "Inetaddress.h"
#include "Epoll.h"
#include "Channel.h"
#include "test-util.h"

#include <assert.h>
#include <string.h>

Server::Server(Eventloop *_loop) : server_loop(_loop){

    Socket *server_socket    = new Socket();
    Inetaddress *server_addr = new Inetaddress("127.0.0.1", 9999); 
    server_socket->s_bind(server_addr);
    server_socket->s_listen();

    // 将listen socket注册进入loop请求队列，以Channel形式
    Channel *server_chan = new Channel(server_socket->getfd(), server_loop);
    std::function<void()> acpt_cbfunc = std::bind(&Server::accpet_connection, this, server_socket);
    server_chan->setCallbackfunc(acpt_cbfunc);
    server_chan->watchReadingLT(); 
}

Server::~Server() {
}

void Server::accpet_connection(Socket *server_socket) {
    Inetaddress *conn_addr = new Inetaddress();               // TODO: Memory leak
    int conn_fd = server_socket->s_accept(conn_addr); 

    printf("new client fd %d! IP: %s Port: %d\n",\
            conn_fd, inet_ntoa(conn_addr->sock_addr.sin_addr), ntohs(conn_addr->sock_addr.sin_port));
    
    Socket *conn_socket = new Socket(conn_fd); // TODO: Memory leak
    conn_socket->s_setnonblocking();
    
    // 将connection socket注册进入loop请求队列，以Channel形式
    Channel *conn_chan = new Channel(conn_socket->getfd(), server_loop);
    std::function<void()> hdle_cbfunc = std::bind(&Server::reading_handle, this, conn_fd);
    conn_chan->setCallbackfunc(hdle_cbfunc);
    conn_chan->watchReadingET();
}

void Server::reading_handle(int conn_sockfd) {

    while(true) {
        char rd_buff[1024], wr_buff[1024];
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
