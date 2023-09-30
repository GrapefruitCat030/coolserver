#include "Connection.h"
#include "Eventloop.h"
#include "Socket.h"
#include "Inetaddress.h"
#include "Channel.h"
#include "Buffer.h"
#include "test-util.h"

#include<string.h>
#include<assert.h>

#define BUFFER_SIZE 8

Connection::Connection(Eventloop *_loop, Socket *socket, Inetaddress *addr):\
                    conn_socket(socket), conn_addr(addr), server_loop(_loop) {

    work_recvbuffer = new Buffer();
    work_sendbuffer = new Buffer();

    // 将connection socket注册进入loop请求队列，以Channel形式
    conn_chan = new Channel(conn_socket->getfd(), server_loop);
    std::function<void()> cbfunc = std::bind(&Connection::reading_echo, this);
    // std::function<void()> cbfunc = std::bind(&Connection::sleep_well, this);
    conn_chan->set_callbackfunc(cbfunc); 
    conn_chan->watch_readingET();

}
Connection::~Connection() {
    delete this->conn_socket;
    delete this->conn_addr;
    delete this->conn_chan;
    delete this->work_recvbuffer;
    delete this->work_sendbuffer;
}


Socket*      Connection::get_connsocket(){ return this->conn_socket;}
Inetaddress* Connection::get_inetaddr()  { return this->conn_addr;  }

void Connection::set_delconn_cb(std::function<void()> cb) {
    del_connection_cb = cb;
}

void Connection::reading_echo() {
    int conn_sockfd = conn_socket->getfd();
    char temp_buf[BUFFER_SIZE];

    // printf("ok i got it.(fd %d) sleep a while.\n", this->conn_socket->getfd());
    // sleep(10);

    while(true) {
        memset(temp_buf, 0, sizeof(temp_buf));
        
        ssize_t read_bytes = recv(conn_sockfd, temp_buf, sizeof(temp_buf), 0);

        if(read_bytes > 0) {
            // process the data normally.
            work_recvbuffer->buf_append(temp_buf, read_bytes);

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
                // the client interrupt normally.
                printf("continue reading.\n");
                continue;
            } else if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // the client finished its sending, or the receiving is over.
                work_sendbuffer->buf_copy(work_recvbuffer);
                printf("client %d: %s\n", conn_sockfd, work_recvbuffer->c_str());
                ssize_t write_bytes = send(conn_sockfd, work_sendbuffer->c_str(), work_sendbuffer->buf_size(), 0);
                if(write_bytes == -1) {
                    errif(true, "write buffer");
                    break;
                }
                work_recvbuffer->buf_clear();
                work_sendbuffer->buf_clear();
                break;
            }
        } else {
            assert(0);
        }
    }
}

void Connection::sleep_well() {
    printf("client %d begin sleep now!\n", this->conn_socket->getfd());
    sleep(10);
    printf("client %d awake!\n", this->conn_socket->getfd());
}
