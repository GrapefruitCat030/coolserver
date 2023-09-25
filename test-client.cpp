#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "src/test-util.h"
#include "src/Inetaddress.h"
#include "src/Socket.h"
#include "src/Buffer.h"

#define BUFFER_SIZE 1024


int main(){

    Socket      *cli_socket  = new Socket();
    Inetaddress *target_addr = new Inetaddress("127.0.0.1", 9999);
    Buffer      *recv_buf    = new Buffer();

    /*
        当前connect不能做到non-blocking connect。只是在服务器端实现了可以多个client连接而已。
    */
    cli_socket->s_connect(cli_socket->getfd(), target_addr);

    while(true) {
        char wr_buff[BUFFER_SIZE], rd_buff[BUFFER_SIZE];
        memset(wr_buff, 0, sizeof(wr_buff));
        memset(rd_buff, 0, sizeof(rd_buff));
        // write to server.
        // firstly write to the wr_buff, then send to the cli_sockfd.
        printf("You:");
        errif(fgets(wr_buff, sizeof(wr_buff), stdin) == NULL, "client input");
        size_t msg_len = strlen(wr_buff); // 移除换行符（如果存在）
        if (msg_len > 0 && wr_buff[msg_len - 1] == '\n'){ 
            wr_buff[msg_len - 1] = '\0';    
            msg_len = strlen(wr_buff);
        }

        ssize_t write_bytes = send(cli_socket->getfd(), wr_buff, msg_len, 0);
        if(write_bytes == -1) {
            errif(true, "write buffer");
        }

        // read from server.
        ssize_t read_bytes = recv(cli_socket->getfd(), rd_buff, sizeof(rd_buff), 0);
        if(read_bytes > 0) {
            recv_buf->buf_append(rd_buff, read_bytes);
            printf("server: %s\n", recv_buf->c_str());
            recv_buf->buf_clear();
        } else if (read_bytes == 0) {
            printf("the server is disconnected. OVER.\n");
            break;
        } else if (read_bytes == -1){
            delete cli_socket;
            delete target_addr;
            errif(true, "read buffer");
        } 

    }

    delete cli_socket;
    delete target_addr;
    delete recv_buf;
    return 0;
}

