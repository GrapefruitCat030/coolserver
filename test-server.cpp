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


#include "test-util.h"



// Can be set to SOMAXCONN, determined by the system.
#define MAXIMUM_CONNECT 10
#define MAX_EVENTS      100



int handleEvent(int conn_sockfd);


int main(){

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);  errif(server_sockfd <= 0, "server socket opening");

    struct sockaddr_in ser_sa;
    memset(&ser_sa, 0, sizeof(ser_sa));
    ser_sa.sin_family      = AF_INET;
    ser_sa.sin_port        = htons(9999);
    ser_sa.sin_addr.s_addr = inet_addr("127.0.0.1"); assert(ser_sa.sin_addr.s_addr != INADDR_NONE); 
    
    errif(bind(server_sockfd, (struct sockaddr *)&ser_sa, sizeof(ser_sa)) == -1, "binging error");
    errif(listen(server_sockfd, MAXIMUM_CONNECT) == -1 , "listening error");    // the socket_fd should have the type of SOCK_STREAM or SOCK_SEQPACKET.

    int eplist_fd = epoll_create1(0);  errif(eplist_fd == -1, "epoll_create");
    struct epoll_event temp_event, ready_events[MAX_EVENTS]; 
    temp_event.events  = EPOLLIN;        // the listen socket uses LT mode.
    temp_event.data.fd = server_sockfd;
    errif(epoll_ctl(eplist_fd, EPOLL_CTL_ADD, server_sockfd, &temp_event) == -1, "epoll_ctl");

    while(true) {

        int nfds = epoll_wait(eplist_fd, ready_events, MAX_EVENTS, -1);  errif(nfds == -1, "epoll_wait"); 
        for(int i = 0; i < nfds; i++) {
            // the listen_sock is ready for the connecting.
            if(ready_events[i].data.fd == server_sockfd) {
                struct sockaddr_in cli_sa;
                socklen_t cli_sa_len = sizeof(cli_sa);
                memset(&cli_sa, 0, sizeof(cli_sa));
                int conn_sockfd = accept(server_sockfd, (struct sockaddr *)&cli_sa, &cli_sa_len);  errif(conn_sockfd == -1, "accept socket");

                printf("new client fd %d! IP: %s Port: %d\n", conn_sockfd, inet_ntoa(cli_sa.sin_addr), ntohs(cli_sa.sin_port));

                setnoblocking(conn_sockfd);             // non-blocking for connection socket,
                temp_event.events  = EPOLLIN | EPOLLET; // used with ET mode.
                temp_event.data.fd = conn_sockfd;
                errif(epoll_ctl(eplist_fd, EPOLL_CTL_ADD, conn_sockfd, &temp_event) == -1, "conn_sock epoll_ctl");

            } else if(ready_events[i].events & EPOLLIN) { 
                // the connect_sock has data for reading.
                handleEvent(ready_events[i].data.fd);
            }
        }


    }
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









