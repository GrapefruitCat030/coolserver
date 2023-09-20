#include<assert.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>

#include<vector>

#include "test-util.h"
#include "Inetaddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"


int handleEvent(int conn_sockfd);

int main(){
    Socket *server_socket    = new Socket();
    Inetaddress *server_addr = new Inetaddress("127.0.0.1", 9999); 
    server_socket->s_bind(server_addr);
    server_socket->s_listen();

    Epoll *server_ep = new Epoll();
    // server_ep->simple_add(server_socket->getfd(), EPOLLIN);
    Channel *server_chan = new Channel(server_socket->getfd(), EPOLLIN);
    server_ep->channel_update(server_chan);
    

    while(true) {
        // std::vector<struct epoll_event> active_events = server_ep->poll();
        std::vector<Channel *> active_channels = server_ep->chan_poll();
        int nfds = active_channels.size();
        for(Channel *chan : active_channels) {
            if(chan->getFd() == server_socket->getfd()) {

                Inetaddress *conn_addr = new Inetaddress();               // TODO: Memory leak
                int conn_fd = server_socket->s_accept(conn_addr); 

                printf("new client fd %d! IP: %s Port: %d\n",\
                        conn_fd, inet_ntoa(conn_addr->sock_addr.sin_addr), ntohs(conn_addr->sock_addr.sin_port));
              
                Socket *conn_socket = new Socket(conn_fd); // TODO: Memory leak
                conn_socket->s_setnonblocking();
                // server_ep->simple_add(conn_socket->getfd(), EPOLLIN | EPOLLET);
                Channel *conn_chan = new Channel(conn_socket->getfd(), EPOLLIN | EPOLLET);
                server_ep->channel_update(conn_chan);

            } else if(chan->getReop() & EPOLLIN) {
                // the connect_socket has data for reading.
                handleEvent(chan->getFd());
            } else {
                // something events that are not EPOLLIN. TODO.
            }
        }
    }

    delete server_socket;
    delete server_ep;

    return 0;
}

int handleEvent(int conn_sockfd) {

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
    return 0;
}

