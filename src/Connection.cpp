#include "Connection.h"
#include "Eventloop.h"
#include "Socket.h"
#include "Inetaddress.h"
#include "Channel.h"
#include "test-util.h"

#include<string.h>
#include<assert.h>


Connection::Connection(Eventloop *_loop, Socket *socket, Inetaddress *addr):\
                    conn_socket(socket), conn_addr(addr), server_loop(_loop) {

    // 将connection socket注册进入loop请求队列，以Channel形式
    conn_chan = new Channel(conn_socket->getfd(), server_loop);
    std::function<void()> cbfunc = std::bind(&Connection::reading_echo, this);
    conn_chan->set_callbackfunc(cbfunc); 
    conn_chan->watch_readingET();

}
Connection::~Connection() {
    delete this->conn_socket;
    delete this->conn_addr;
    delete this->conn_chan;
}


Socket*      Connection::get_connsocket(){ return this->conn_socket;}
Inetaddress* Connection::get_inetaddr()  { return this->conn_addr;  }

void Connection::set_delconn_cb(std::function<void()> cb) {
    del_connection_cb = cb;
}

void Connection::reading_echo() {
    int conn_sockfd = conn_socket->getfd();
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
            del_connection_cb();
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


