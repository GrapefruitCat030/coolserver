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


int main(){

    Socket      *cli_socket  = new Socket();
    Inetaddress *target_addr = new Inetaddress("127.0.0.1", 9999);


    /*
        当前connect不能做到non-blocking connect。只是在服务器端实现了可以多个client连接而已。
    */
    cli_socket->s_connect(cli_socket->getfd(), target_addr);

    while(true) {
        char wr_buff[1024], rd_buff[1024];
        memset(wr_buff, 0, sizeof(wr_buff));
        memset(rd_buff, 0, sizeof(rd_buff));
        // write to server.
        // firstly write to the wr_buff, then send to the cli_sockfd.
        printf("You:");
        errif(fgets(wr_buff, sizeof(wr_buff), stdin) == NULL, "client input");
        size_t len = strlen(wr_buff); // 移除换行符（如果存在）
        if (len > 0 && wr_buff[len - 1] == '\n') wr_buff[len - 1] = '\0';    
        
        ssize_t write_bytes = write(cli_socket->getfd(), wr_buff, sizeof(wr_buff));
        if(write_bytes == -1) {
            errif(true, "write buffer");
        }

        // read from server.
        ssize_t read_bytes = read(cli_socket->getfd(), rd_buff, sizeof(rd_buff));
        if(read_bytes > 0) {
            printf("server: %s\n", rd_buff);
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
    return 0;
}

