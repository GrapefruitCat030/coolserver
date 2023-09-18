#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "test-util.h"

int main(){

    int cli_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in target_sa;
    memset(&target_sa, 0, sizeof(target_sa));
    target_sa.sin_family      = AF_INET;
    target_sa.sin_port        = htons(9999);
    target_sa.sin_addr.s_addr = inet_addr("127.0.0.1");    

    /*
        当前connect不能做到non-blocking connect。只是在服务器端实现了可以多个client连接而已。
    */
    errif(connect(cli_sockfd, (struct sockaddr *)&target_sa, sizeof(target_sa)) == -1, "connection failure" LINEABORT);

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
        
        ssize_t write_bytes = write(cli_sockfd, wr_buff, sizeof(wr_buff));
        if(write_bytes == -1) {
            errif(true, "write buffer");
            break;
        }

        // read from server.
        ssize_t read_bytes = read(cli_sockfd, rd_buff, sizeof(rd_buff));
        if(read_bytes > 0) {
            printf("server: %s\n", rd_buff);
        } else if (read_bytes == 0) {
            printf("the server is disconnected. OVER.\n");
            close(cli_sockfd);
            break;
        } else if (read_bytes == -1){
            close(cli_sockfd);
            errif(true, "read buffer");
        } 

    }

}

