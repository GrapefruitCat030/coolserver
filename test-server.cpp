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

#include "src/Eventloop.h"
#include "src/Server.h"

int main(){

    Eventloop  *server_loop = new Eventloop();
    Server *server_instance = new Server(server_loop);
    server_loop->bootloop();
    return 0;
}

